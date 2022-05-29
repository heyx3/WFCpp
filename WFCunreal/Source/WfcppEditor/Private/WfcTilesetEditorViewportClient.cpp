#include "WfcTilesetEditorViewportClient.h"

#include "WfcppEditorModule.h"

FWfcTilesetEditorViewportClient::FWfcTilesetEditorViewportClient(FWfcTilesetEditorScene& scene)
    //NOTE: the 'nullptr' in the base constructor is a "FEditorModeTools", which I believe represents
    //    a mode of editor interaction, like how the main scene editor has Terrain, mesh placement, etc.
    : FEditorViewportClient(nullptr, &scene)
{
    //Some example code from the Material preview viewport client, for reference.
    // DrawHelper.bDrawPivot = false;
    // DrawHelper.bDrawWorldBox = false;
    // DrawHelper.bDrawKillZ = false;
    // DrawHelper.bDrawGrid = false;
    // DrawHelper.GridColorAxis = FColor(80,80,80);
    // DrawHelper.GridColorMajor = FColor(72,72,72);
    // DrawHelper.GridColorMinor = FColor(64,64,64);
    // DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;

    //Note: some initialization can't be done here, such as centering the camera viewport.
    //Instead, it's done on the first tick.
}

void FWfcTilesetEditorViewportClient::Tick(float deltaSeconds)
{
    //Call normal client behavior.
    FEditorViewportClient::Tick(deltaSeconds);
    if (!GIntraFrameDebuggingGameThread)
        PreviewScene->GetWorld()->Tick(LEVELTICK_All, deltaSeconds);

    //Do late initialization.
    if (firstTick)
    {
        firstTick = false;

        SetViewRotation(FVector(1, 1, -1).Rotation());
        FocusViewportOnBox(FBox(FVector::OneVector * -500, FVector::OneVector * 500),
                           true);
    }

    OnTick.Broadcast(deltaSeconds);
}
void FWfcTilesetEditorViewportClient::Draw(FViewport* viewport, FCanvas* canvas)
{
    FEditorViewportClient::Draw(viewport, canvas);
}
