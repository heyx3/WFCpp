#include "WfcGenerator.h"

float UWfcGenerator::GetProgress() const
{
	switch (GetStatus())
	{
		case WfcSimState::Finished: return 1.0f;
		case WfcSimState::Off: return 0.0f;
		
		case WfcSimState::Running: {
			verify(state.IsSet());
			const auto& wfc = state.GetValue();
		
			int nFinished = 0;
			for (auto cell : WFC::Region3i(wfc.Grid.Cells.GetDimensions()))
				if (wfc.Grid.Cells[cell].IsSet())
					nFinished += 1;
		
			return static_cast<float>(nFinished) /
				     state.GetValue().Grid.Cells.GetNumbElements();
		}
		
		default: return nanf(nullptr);
	}
}

bool UWfcGenerator::GetCell(const FIntVector& cellPos,
				    		WfcTileID& out_tileID,
				    		UObject*& out_tileData,
						    FWFC_Transform3D& out_tileTransform) const
{
	checkf(GetStatus() != WfcSimState::Off, TEXT("Simulation hasn't started yet"));
	verify(state.IsSet());
	const auto& wfc = state.GetValue();

	WFC::Vector3i wfcPos(cellPos.X, cellPos.Y, cellPos.Z);
	WFC::Region3i wfcBounds(wfc.Grid.Cells.GetDimensions());
	check(wfcBounds.Contains(wfcPos));
	const auto& cell = wfc.Grid.Cells[wfcPos];

	if (cell.IsSet())
	{
	    out_tileID = wfcTileIDs[cell.ChosenTile];
	    out_tileData = tileset->Tiles[out_tileID].Data;
	    out_tileTransform = { static_cast<WFC_Rotations3D>(cell.ChosenPermutation.Rot),
	                          cell.ChosenPermutation.Invert };
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
    tileset = tiles;
    
	checkf(seed >= 0, TEXT("Seed value is less than 0: %i"), seed);
	checkf(seed <= static_cast<int64_t>(std::numeric_limits<uint32_t>().max()),
	       TEXT("Seed value is more than the max of %i: %i"),
	       std::numeric_limits<uint32_t>().max(), seed);
    checkf(tileset->Tiles.Num() > 0, TEXT("Must have at least 1 tile in the tileset!"))

	//Clean up from any previous runs.
	if (IsRunning())
		Cancel();

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

    //Convert each serialized UE4 tile into a WFC library tile.
    wfcTileInputs.Clear();
    wfcTileIDs.Empty();
    TSet<FWFC_Transform3D> buffer_supportedTransforms;
    for (const auto& tile : tiles->Tiles)
    {
        wfcTileIDs.Add(tile.Key);
        auto& wfcTile = wfcTileInputs.PushBack({ { } });
        
        wfcTile.Weight = static_cast<uint32_t>(tile.Value.WeightU32);

        //Convert the serialized UE4 permutation set into a WFC library permutation set.
        buffer_supportedTransforms.Empty();
        tile.Value.GetSupportedTransforms(buffer_supportedTransforms);
        for (auto transform : buffer_supportedTransforms)
            wfcTile.Permutations.Add(transform.Unwrap());

        //Convert the serialized UE4 face data into WFC library face data.
        for (int faceI = 0; faceI < WFC::Tiled3D::N_DIRECTIONS_3D; ++faceI)
        {
            auto dir = static_cast<WFC::Tiled3D::Directions3D>(faceI);
            wfcTile.Data.Faces[faceI].Side = dir;

            //Generate the point ID's for this face.
            const auto& assetFace = tile.Value.GetFace(dir);
            const auto& prototype = tiles->FacePrototypes[assetFace.PrototypeID];
            auto firstFaceID = prototypePointsFirstIDs[assetFace.PrototypeID];
            for (int pointI = 0; pointI < WFC::Tiled3D::N_FACE_POINTS; ++pointI)
            {
                auto localCorner = static_cast<WFC::Tiled3D::FacePoints>(pointI);
                auto prototypeCorner = assetFace.GetPrototypeCorner(localCorner);
                auto cornerID = prototype.GetPointSymmetry(prototypeCorner);

                //Convert the 0-3 symmetry value stored in the asset,
                //    into a unique index across all tile faces.
                auto cornerUniqueID = firstFaceID + static_cast<WFC::Tiled3D::PointID>(cornerID);
                wfcTile.Data.Faces[faceI].Points[pointI] = cornerUniqueID;
            }
        }
    }

	//Start the algorithm.
	state = WFC::Tiled3D::StandardRunner(
	    wfcTileInputs, WFC::Vector3i(gridSize.X, gridSize.Y, gridSize.Z),
	    nullptr,
	    WFC::PRNG(seed)
	);
	status = WfcSimState::Running;
}
void UWfcGenerator::Cancel()
{
    status = WfcSimState::Off;
    state = TOptional<WFC::Tiled3D::StandardRunner>();
}

void UWfcGenerator::Tick()
{
    checkf(IsRunning(), TEXT("Can't Tick the WFC algorithm if it isn't running!"));
    check(state.IsSet());
	
    bool isFinished = state.GetValue().Tick();
    if (isFinished)
        status = WfcSimState::Finished;
    else
        status = WfcSimState::Running;
}

bool UWfcGenerator::RunToEnd(int timeoutIterations)
{
    bool isFinished = state.GetValue().TickN(timeoutIterations);
    if (isFinished)
    {
        status = WfcSimState::Finished;
        return true;
    }
    else
    {
        status = WfcSimState::Running;
        return false;
    }
}