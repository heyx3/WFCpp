#include "WfcTilesetEditorScene.h"

#include "Components/DirectionalLightComponent.h"
#include "GameFramework/WorldSettings.h"


FWfcTilesetEditorScene::FWfcTilesetEditorScene(ConstructionValues cvs)
    : FAdvancedPreviewScene(cvs),
      compTileViz(nullptr)
{
    auto& world = *GetWorld();
    auto& worldSettings = *world.GetWorldSettings();

    //Start up the world.
    worldSettings.NotifyBeginPlay();
    worldSettings.NotifyMatchStarted();
    worldSettings.SetActorHiddenInGame(false);
    world.bBegunPlay = true;

    //Set up the lighting.
    // DirectionalLight->SetRelativeLocation(FVector(-1024, 1024, 2048));
    // DirectionalLight->SetRelativeScale3D(FVector(15.0f));
    // SetLightBrightness(4.0f);
    // DirectionalLight->InvalidateLightingCache();
    // DirectionalLight->RecreateRenderState_Concurrent();

    //Configure the scene.
    SetFloorVisibility(false);
}

void FWfcTilesetEditorScene::SetTileViz(UActorComponent* newTileViz)
{
    if (compTileViz != nullptr)
    {
        RemoveComponent(compTileViz);
        compTileViz->DestroyComponent();
    }
    compTileViz = newTileViz;
    
    AddComponent(newTileViz, FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1000.0f)));
}
