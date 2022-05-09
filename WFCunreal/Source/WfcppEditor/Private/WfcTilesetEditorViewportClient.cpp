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
}

void FWfcTilesetEditorViewportClient::Tick(float deltaSeconds)
{
    FEditorViewportClient::Tick(deltaSeconds);
    if (!GIntraFrameDebuggingGameThread)
        PreviewScene->GetWorld()->Tick(LEVELTICK_All, deltaSeconds);
}
void FWfcTilesetEditorViewportClient::Draw(FViewport* viewport, FCanvas* canvas)
{
    FEditorViewportClient::Draw(viewport, canvas);
}
