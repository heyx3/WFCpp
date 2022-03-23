#include "WFC_Contr.h"

#include "Utils.hpp"

float UWFC_Contr::GetProgress() const
{
	switch (GetStatus())
	{
		case WFC_SimState::Failed: return 1.0f;
		case WFC_SimState::Succeeded: return 1.0f;
		case WFC_SimState::Off: return 0.0f;
		
		case WFC_SimState::Running: {
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

bool UWFC_Contr::GetCell(FIntVector cell,
						 int& out_SourceIdx,
						 TSubclassOf<AActor>& out_SourceActor,
						 FWFC_Transform3D& out_tileTransform) const
{
	UWFC_CHECK(GetStatus() != WFC_SimState::Off, "Simulation hasn't started yet");
	verify(state.IsSet());
	const auto& wfc = state.GetValue();

	WFC::Vector3i wfcPos(cell.X, cell.Y, cell.Z);
	WFC::Region3i wfcBounds(wfc.Output.GetDimensions());
	UWFC_CHECK(wfcBounds.Contains(wfcPos), "");
	const auto& wfcTile = wfc.Output[wfcPos];

	if (wfcTile.IsSet())
	{
		const auto& sourceData = tilesByID[wfcTile.Value.Value];
	    out_SourceIdx = sourceData.Get<2>();
		out_SourceActor = sourceData.Get<1>();
		out_tileTransform = FWFC_Transform3D{ sourceData.Get<0>().Invert,
											  (WFC_Rotations3D)sourceData.Get<0>().Rot
											};
		return true;
	}
	else
	{
		out_SourceIdx = -1;
		out_SourceActor = nullptr;
		out_tileTransform = FWFC_Transform3D{ false, WFC_Rotations3D::None };
		return false;
	}
}


void UWFC_Contr::Start(float tileSize, const TArray<FWFC_TileRef>& tiles, FIntVector gridSize,
					   FIntVector clearSize, int seed,
					   bool periodicX, bool periodicY, bool periodicZ)
{
	UWFC_CHECK(seed >= 0, "Seed value is less than 0: %i", seed);
	UWFC_CHECK(seed <= std::numeric_limits<uint32_t>().max(),
	           "Seed value is more than the max of %i: %i",
	              std::numeric_limits<uint32_t>().max(),
	              seed);

	//Clean up from any previous runs.
	if (IsRunning())
		Cancel();
	failuresBuffer.Clear();
	failures.Empty();

	//Grab data from each tile.
	tilesByID.Empty();
	tilesBuffer.Clear();
	int sourceTileID = 0;
	for (const auto& tileRef : tiles)
	{
		UWFC_CHECK(tileRef.Source.Get() != nullptr, "No Actor was given for tile %i", sourceTileID);
		auto* tileActor = tileRef.Source.GetDefaultObject();

		UWFC_CHECK(tileActor->GetClass()->ImplementsInterface(UWFC_Ti::StaticClass()),
			       "Tile actor %s doesn't implement the WFC_Ti interface",
			       *tileActor->GetClass()->GetName());
		auto tileFaces = IWFC_Ti::Execute_GetTileData(tileActor);

		//Generate the un-transformed tile.
		tilesBuffer.PushBack(WFC::Tiled3D::Tile{ tileFaces.ToInternal(), static_cast<uint32_t>(tileRef.Weight) });
		tilesByID.Add(tilesBuffer.GetSize() - 1,
					  MakeTuple(WFC::Tiled3D::Transform3D(), tileRef.Source.Get(), sourceTileID));

		//TODO: Generate permutations as requested

		sourceTileID += 1;
	}

	wfcInput = WFC::Tiled3D::InputData(tilesBuffer);

	//Start the algorithm.
	state = WFC::Tiled3D::State(wfcInput, WFC::Vector3i(gridSize.X, gridSize.Y, gridSize.Z),
								static_cast<uint32_t>(seed),
								periodicX, periodicY, periodicZ,
								WFC::Vector3i(clearSize.X, clearSize.Y, clearSize.Z));
	status = WFC_SimState::Running;
}
void UWFC_Contr::Cancel()
{
	status = WFC_SimState::Off;
	state = TOptional<WFC::Tiled3D::State>();
}

void UWFC_Contr::Tick()
{
	UWFC_CHECK(IsRunning(), "Can't Tick the WFC algorithm if it isn't running!");
	verify(state.IsSet());
	
	auto result = state.GetValue().Iterate(failuresBuffer);
	if (result.HasValue)
	{
		if (result.Value)
		{
			status = WFC_SimState::Succeeded;
		}
		else
		{
			status = WFC_SimState::Failed;
			
			//Load the failure data into an Unreal-friendly format.
			check(failures.Num() == 0);
			for (int i = 0; i < failuresBuffer.GetSize(); ++i)
				failures.Push(FIntVector(failuresBuffer[i].x, failuresBuffer[i].y, failuresBuffer[i].z));
		}
	}
}

bool UWFC_Contr::RunToEnd(int timeoutIterations)
{
	int i = 0;
	while (IsRunning() && i < timeoutIterations)
	{
		i += 1;
		Tick();
	}

	return GetStatus() == WFC_SimState::Succeeded;
}
