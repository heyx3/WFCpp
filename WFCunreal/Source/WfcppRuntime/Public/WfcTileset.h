#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "WfcTile.h"

#include "WfcTileset.generated.h"


using WfcTileID = int32;

UCLASS(BlueprintType)
class WFCPPRUNTIME_API UWfcTileset : public UObject
{
	GENERATED_BODY()
public:

	//The faces that tiles can have.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Nickname))
	TMap<int32, FWfcFacePrototype> FacePrototypes;

	//All tiles in this set.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FWfcTile> Tiles;

    //The width/height/depth of each tile.
    //Used when visualizing the tile in the editor, but you can also use it when placing tiles in the world.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TileLength = 1000.0f;
};