#pragma once

#include "CoreMinimal.h"

#include "WfcTileVisualizer.generated.h"

class UWfcTileset;

//A technique for visualizing a tile within a preview scene.
//Only used within the editor.
UCLASS(BlueprintType, Abstract)
class WFCPPRUNTIME_API UWfcTileVisualizer : public UObject
{
    GENERATED_BODY()
public:

    //Generates components to visualize a specific tile in a tileset.
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
    TMap<UActorComponent*, FTransform> Visualize(const UWfcTileset* tileset, int32 tileID);

    //The fallback implementation that can be used if no tile visualizer is specified.
    //This way you don't have to allocate an instance unless you need special behavior.
    static TMap<UActorComponent*, FTransform> DefaultBehavior(const UWfcTileset* tileset, int32 tileID);
};