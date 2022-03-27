#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "WfcTile.h"

#include "WfcTileset.generated.h"


UCLASS(BlueprintType)
class WFCPPRUNTIME_API UWfcTileset : public UObject
{
	GENERATED_BODY()
public:

	//The faces that tiles can have.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int, FWfcFacePrototype> FacePrototypes;

	//All tiles in this set.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int, FWfcTile> Tiles;
};