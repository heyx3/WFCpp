#include "WfcGenerator.h"

float UWfcGenerator::GetProgress() const
{
	switch (GetStatus())
	{
		case WfcSimState::Failed: return 1.0f;
		case WfcSimState::Succeeded: return 1.0f;
		case WfcSimState::Off: return 0.0f;
		
		case WfcSimState::Running: {
			verify(state.IsSet());
			const auto& wfc = state.GetValue();
		
			int nFinished = 0;
			for (auto cell : WFC::Region3i(wfc.Output.GetDimensions()))
				if (wfc.Output[cell].IsSet())
					nFinished += 1;
		
			return static_cast<float>(nFinished) /
				     state.GetValue().Output.GetNumbElements();
		}
		
		default: return nanf(nullptr);
	}
}

bool UWfcGenerator::GetCell(const FIntVector& cell,
				    		WfcTileID& out_tileID,
						    UObject*& out_tileData,
						    FWFC_Transform3D& out_tileTransform) const
{
	checkf(GetStatus() != WfcSimState::Off, TEXT("Simulation hasn't started yet"));
	verify(state.IsSet());
	const auto& wfc = state.GetValue();

	WFC::Vector3i wfcPos(cell.X, cell.Y, cell.Z);
	WFC::Region3i wfcBounds(wfc.Output.GetDimensions());
	check(wfcBounds.Contains(wfcPos));
	const auto& wfcTile = wfc.Output[wfcPos];

	if (wfcTile.IsSet())
	{
		const auto& sourceData = tilesByID[wfcTile.Value.Value];
	    out_tileID = sourceData.Get<2>();
		out_tileData = sourceData.Get<1>();
		out_tileTransform = { static_cast<WFC_Rotations3D>(sourceData.Get<0>().Rot),
		                      sourceData.Get<0>().Invert };
		return true;
	}
	else
	{
		out_tileID = -1;
		out_tileData = nullptr;
		out_tileTransform = { WFC_Rotations3D::None, false };
		return false;
	}
}

void UWfcGenerator::Start(const UWfcTileset* tiles,
                          const FIntVector& gridSize,
			              FIntVector clearSize, int seed,
					      bool periodicX, bool periodicY, bool periodicZ)
{
	checkf(seed >= 0, TEXT("Seed value is less than 0: %i"), seed);
	checkf(seed <= std::numeric_limits<uint32_t>().max(),
	       TEXT("Seed value is more than the max of %i: %i"),
	       std::numeric_limits<uint32_t>().max(), seed);

	//Clean up from any previous runs.
	if (IsRunning())
		Cancel();
	failuresBuffer.Clear();
	failures.Empty();

    //TODO: Allocate reusable buffers for the input data setup, below.
    
    //Assign unique point ID's based on the face prototypes.
    //Each face prototype can have up to 4 values.
    TMap<int32, WFC::Tiled3D::PointID> prototypePointsFirstIDs;
    WFC::Tiled3D::PointID nextPointID = 1;
    for (const auto& facePrototype : tiles->FacePrototypes)
    {
        prototypePointsFirstIDs.Add(facePrototype.Get<0>(), nextPointID);
        nextPointID += 4;
    }

    //Each tile asset maps to multiple WFC tiles, through a set of transformations.
    tilesByID.Empty();
    wfcTileInputs.Clear();
    TSet<FWFC_Transform3D> tileTransforms;
    auto nextTileID = WFC::Tiled3D::TileID_FIRST_VALID;
    for (const auto& tile : tiles->Tiles)
    {
        tileTransforms.Empty();
        tile.Value.GetSupportedTransforms(tileTransforms);

        //Generate a WFC input tile using the un-transformed version of this asset, as a reference.
        WFC::Tiled3D::Tile referenceTile;
        referenceTile.Weight = static_cast<uint32_t>(tile.Value.WeightU32);
        for (int faceI = 0; faceI < N_DIRECTIONS3D; ++faceI)
        {
            auto dir = static_cast<WFC::Tiled3D::Directions3D>(faceI);
            const auto& assetFace = tile.Value.GetFace(dir);
            const auto& prototype = tiles->FacePrototypes[assetFace.PrototypeID];
            auto firstFaceID = prototypePointsFirstIDs[assetFace.PrototypeID];

            referenceTile.Data.Faces[faceI].Side = dir;
            
            for (int pointI = 0; pointI < WFC_N_FACE_POINTS; ++pointI)
            {
                auto localCorner = static_cast<WFC::Tiled3D::FacePoints>(pointI);
                auto prototypeCorner = assetFace.GetPrototypeCorner(localCorner);
                auto cornerID = prototype.GetPoint(prototypeCorner);

                //Convert the 0-3 symmetry value stored in the asset,
                //    into a unique index across all tile faces.
                auto cornerUniqueID = firstFaceID + static_cast<WFC::Tiled3D::PointID>(cornerID);
                referenceTile.Data.Faces[faceI].Points[pointI] = cornerUniqueID;
            }
        }

        //Generate each permutation of this tile.
        //TODO: Discard redundant permutations. 
        for (const auto& transform : tileTransforms)
        {
            WFC::Tiled3D::Tile permutedTile{ transform.Unwrap().ApplyToCube(referenceTile.Data),
                                             referenceTile.Weight };
            
            tilesByID.Add(nextTileID, MakeTuple(transform.Unwrap(), tile.Value.Data, tile.Key));
            wfcTileInputs.PushBack(WFC::Tiled3D::Tile());
            nextTileID += 1;
        }
    }

	wfcInput = WFC::Tiled3D::InputData(wfcTileInputs);

	//Start the algorithm.
	state = WFC::Tiled3D::State(wfcInput, WFC::Vector3i(gridSize.X, gridSize.Y, gridSize.Z),
								static_cast<uint32_t>(seed),
								periodicX, periodicY, periodicZ,
								WFC::Vector3i(clearSize.X, clearSize.Y, clearSize.Z));
	status = WfcSimState::Running;
}
void UWfcGenerator::Cancel()
{
    status = WfcSimState::Off;
    state = TOptional<WFC::Tiled3D::State>();
}

void UWfcGenerator::Tick()
{
    checkf(IsRunning(), TEXT("Can't Tick the WFC algorithm if it isn't running!"));
    check(state.IsSet());
	
    auto result = state.GetValue().Iterate(failuresBuffer);
    if (result.HasValue)
    {
        if (result.Value)
        {
            status = WfcSimState::Succeeded;
        }
        else
        {
            status = WfcSimState::Failed;
			
            //Load the failure data into an Unreal-friendly format.
            check(failures.Num() == 0);
            for (int i = 0; i < failuresBuffer.GetSize(); ++i)
                failures.Push(FIntVector(failuresBuffer[i].x, failuresBuffer[i].y, failuresBuffer[i].z));
        }
    }
}

bool UWfcGenerator::RunToEnd(int timeoutIterations)
{
    int i = 0;
    while (IsRunning() && i < timeoutIterations)
    {
        i += 1;
        Tick();
    }

    return GetStatus() == WfcSimState::Succeeded;
}