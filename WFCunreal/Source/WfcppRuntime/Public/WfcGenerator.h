#pragma once

#include "Tiled3D/State.h"
#include "WfcTileset.h"

#include "WfcGenerator.generated.h"


UENUM(BlueprintType)
enum class WfcSimState : uint8
{
    Off, Running,
    Succeeded,
	
    //One or more grid cells cannot be solved given their neighbors,
    //    and "clearing" is turned off.
    Failed
};


//Encapsulates the running of the WFC algorithm on a tileset.
UCLASS(BlueprintType)
class UWfcGenerator : public UObject
{
    GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm", meta=(CompactNodeTitle="Status"))
    WfcSimState GetStatus() const { return status; }
	
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm", meta=(CompactNodeTitle="Running?"))
    bool IsRunning() const { return GetStatus() == WfcSimState::Running; }

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

    //Gets the tile assigned to the given cell, if one has been assigned yet.
    //If no tile has been assigned, returns "false".
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="WFC/Algorithm")
	bool GetCell(const FIntVector& cell,
				 int32& out_tileID,
				 UObject*& out_tileData,
				 FWFC_Transform3D& out_tileTransform) const;

	
	//-------------
	//  Operations
	//-------------
	
	//Kicks off the WFC algorithm with the given inputs.
    //If the algorithm was already running, that previous run will be canceled.
	//Note that if 'clearSize' is set to zero, then WFC will fail if it encounters an unsolvable grid cell.
	UFUNCTION(BlueprintCallable, Category="WFC/Ops", meta=(AdvancedDisplay=5))
	void Start(const UWfcTileset* tiles, const FIntVector& gridSize,
	           FIntVector clearSize,
	           int seedU32 = 1234567890,
	           bool periodicX = false,
	           bool periodicY = false,
	           bool periodicZ = false);

    //Fails if the algorithm isn't running.
	UFUNCTION(BlueprintCallable, Category="WFC/Ops")
	void Tick();

	UFUNCTION(BlueprintCallable, Category="WFC/Ops")
	void Cancel();
	
	//Runs WFC until the sim finishes (or fails). Stops at the given timeout count.
	//Returns whether it ended due to success.
	UFUNCTION(BlueprintCallable, Category="WFC/Ops")
	bool RunToEnd(int timeoutIterations = 10000);

	
private:
	WfcSimState status = WfcSimState::Off;
	TOptional<WFC::Tiled3D::State> state;

	//Arrays used for storing failure data.
	WFC::List<WFC::Vector3i> failuresBuffer;
	TArray<FIntVector> failures;

	//Arrays used to build sim inputs on startup.
	WFC::List<WFC::Tiled3D::Tile> wfcTileInputs;
	WFC::Tiled3D::InputData wfcInput;

	//Arrays used to translate the output of WFC back into Unreal.
	TMap<WFC::Tiled3D::TileID, TTuple<WFC::Tiled3D::Transform3D, UObject*, WfcTileID>> tilesByID;
};