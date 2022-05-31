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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Data))
	TMap<int32, FWfcTile> Tiles;

    //The width/height/depth of each tile.
    //Used when visualizing the tile in the editor, but you can also use it when placing tiles in the world.
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TileLength = 1000.0f;

    //How a tile's data can be previewed in the editor.
    //Leave unset to use the default implementation (handles static meshes).
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UWfcTileVisualizer* PreviewVisualizer;
    
    
    //Provide callbacks to the editor for when this asset changes.
#if WITH_EDITOR    
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTilesetEdited, UWfcTileset*, FPropertyChangedEvent&);
    FOnTilesetEdited OnEdited;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& eventData) override
    {
        OnEdited.Broadcast(this, eventData);
    }
    
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTilesetChainEdited, UWfcTileset*, FPropertyChangedChainEvent&);
    FOnTilesetChainEdited OnChainEdited;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& eventData) override
    {
        OnChainEdited.Broadcast(this, eventData);
    }
#endif
};