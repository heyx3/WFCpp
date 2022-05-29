#include "WfcTileVisualizer.h"

#include "WfcTileset.h"
#include "WfcppRuntimeModule.h"


TMap<UActorComponent*, FTransform> UWfcTileVisualizer::DefaultBehavior(const UWfcTileset* tileset, WfcTileID tileID, UObject* tileData)
{
    //If the data is null, don't output anything.
    if (tileset == nullptr || tileData == nullptr)
        return { };

    checkf(tileset->Tiles.Contains(tileID), TEXT("No tile with the ID %i in tileset '%s'"),
           tileID, *tileset->GetName());

    //If it's naming a specific class, try to visualize that class.
    if (tileData->IsA<UClass>())
    {
        auto dataClass = Cast<UClass>(tileData);
        if (dataClass->IsChildOf<AActor>())
        {
            UE_LOG(LogWFC, Error,
                   TEXT("Unable to spawn an Actor for a preview scene, because the editor crashes when it's destroyed."));
            return { };
        }
        else
        {
            UE_LOG(LogWFC, Error, TEXT("Unknown UObject type referenced: %s"), *dataClass->GetFullName());
            return { };
        }
    }

    //If it's naming a Blueprint, spawn that blueprint actor.
    if (tileData->IsA<UBlueprint>())
    {
        //TODO: It might be a component instead of an actor!
        
        UE_LOG(LogWFC, Error,
               TEXT("Unable to spawn an Actor in the preview scene, because the editor crashes when it's destroyed."));
        return { };
    }

    //If it's naming a static mesh, spawn that mesh.
    if (tileData->IsA<UStaticMesh>())
    {
        auto* component = NewObject<UStaticMeshComponent>(GetTransientPackage(), NAME_None, RF_Transient);
        component->SetStaticMesh(Cast<UStaticMesh>(tileData));
        return {
            { component, FTransform() }
        };
    }

    //Fallback: log an error.
    UE_LOG(LogWFC, Error, TEXT("Unknown UObject data referenced: %s"), *tileData->GetFullName());
    return { };
}
