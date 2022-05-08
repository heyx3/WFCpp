#include "WfcTilesetEditorScene.h"

#include "Components/DirectionalLightComponent.h"
#include "GameFramework/WorldSettings.h"


FWfcTilesetEditorScene::FWfcTilesetEditorScene(ConstructionValues cvs)
    : FPreviewScene(cvs)
{
    auto& world = *GetWorld();
    auto& worldSettings = *world.GetWorldSettings();

    //Start up the world.
    worldSettings.NotifyBeginPlay();
    worldSettings.NotifyMatchStarted();
    worldSettings.SetActorHiddenInGame(false);
    world.bBegunPlay = true;

    //Set up the lighting.
    DirectionalLight->SetRelativeLocation(FVector(-1024, 1024, 2048));
    DirectionalLight->SetRelativeScale3D(FVector(15.0f));
    SetLightBrightness(4.0f);
    DirectionalLight->InvalidateLightingCache();
    DirectionalLight->RecreateRenderState_Concurrent();

    //Set up the sky sphere.
    compSkySphere = NewObject<UStaticMeshComponent>();
    auto* skyMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/MapTemplates/Sky/SM_SkySphere.SM_SkySphere"),
                                            nullptr, LOAD_None, nullptr);
    auto* skyMat = LoadObject<UMaterial>(nullptr, TEXT("/Engine/EditorMaterials/PersonaSky.PersonaSky"),
                                                  nullptr, LOAD_None, nullptr);
    compSkySphere->SetStaticMesh(skyMesh);
    compSkySphere->SetMaterial(0, skyMat);
    FPreviewScene::AddComponent(compSkySphere, FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1000.0f)));
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
