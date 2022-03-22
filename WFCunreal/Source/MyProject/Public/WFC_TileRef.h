#pragma once

#include "CoreMinimal.h"
#include "WFC_TileRef.generated.h"

USTRUCT(BlueprintType)
struct FWFC_TileRef
{
	GENERATED_BODY()
	
	//The Actor representing this tile.
	//Must have a WFC_Component attached to it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Source;
	
	//Heavier tiles are more likely to appear.
	//Limited to uint32 range.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=4294967295)) 
	int Weight = 100;
};