#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "State.h"
#include "WFC_TileRef.h"
#include "WFC_Ti.h"

#include "WFC_Contr.generated.h"


UENUM(BlueprintType)
enum class WFC_SimState : uint8
{
	Off, Running,
	Succeeded,
	
	//One or more grid cells cannot be solved given their neighbors,
	//    and "clearing" is turned off.
	Failed
};


//Manages a WFC algorithm state.
UCLASS(BlueprintType, DisplayName="WFC Controller")
class MYPROJECT_API UWFC_Contr : public UObject
{
	GENERATED_BODY()

public:

	//--------------------
	//  Algorithm queries
	//--------------------
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm", meta=(CompactNodeTitle="Status"))
	WFC_SimState GetStatus() const { return status; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm", meta=(CompactNodeTitle="Running?"))
	bool IsRunning() const { return GetStatus() == WFC_SimState::Running; }

	//Gets the grid cells that WFC considered unsolvable.
	//Only applicable if grid clearing is turned off.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm")
	const TArray<FIntVector>& GetFailures() const { return failures; }

	//Returns a progress indicator from 0 to 1.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm")
	float GetProgress() const;

	//TODO: More ways to get information about the algorithm

	//-----------------
	//  Result queries
	//-----------------

	//Gets the value at the given cell, if it exists.
	//Returns whether the value existed or not.
	//Outputs the original index of the tile (from the array you passed in on start),
	//    along with its associated Actor and its the particular permutation.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm")
	bool GetCell(FIntVector cell,
				 int& out_SourceIdx,
				 TSubclassOf<AActor>& out_SourceActor,
				 FWFC_Transform3D& out_tileTransform) const;

	
	//-------------
	//  Operations
	//-------------
	
	//Starts the algorithm with the given arguments.
	//Note that if 'clearSize' is set to zero, then WFC will fail if it encounters an unsolvable grid cell.
	UFUNCTION(BlueprintCallable, Category="WFC/Ops", meta=(AdvancedDisplay=5))
	void Start(float tileSize, const TArray<FWFC_TileRef>& tiles, FIntVector gridSize,
	           FIntVector clearSize,
	           int seedU32 = 1234567890,
	           bool periodicX = false,
	           bool periodicY = false,
	           bool periodicZ = false);
	
	UFUNCTION(BlueprintCallable, Category="WFC/Ops")
	void Tick();

	UFUNCTION(BlueprintCallable, Category="WFC/Ops")
	void Cancel();
	
	//Runs WFC until the sim finishes (or fails). Stops at the given timeout count.
	//Returns whether it ended due to success.
	UFUNCTION(BlueprintCallable, Category="WFC/Ops")
	bool RunToEnd(int timeoutIterations = 10000);

	
private:
	WFC_SimState status = WFC_SimState::Off;
	TOptional<WFC::Tiled3D::State> state;

	//Arrays used for storing failure data.
	WFC::List<WFC::Vector3i> failuresBuffer;
	TArray<FIntVector> failures;

	//Arrays used to build sim inputs on startup.
	WFC::List<WFC::Tiled3D::Tile> tilesBuffer;
	WFC::Tiled3D::InputData wfcInput;

	//Arrays used to translate the output of WFC back into Unreal.
	TMap<WFC::Tiled3D::TileID, TTuple<WFC::Tiled3D::Transform3D, TSubclassOf<AActor>, int>> tilesByID;
};