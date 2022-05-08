#include "WfcTilesetEditorViewportClient.h"

void FWfcTilesetEditorViewportClient::Tick(float deltaSeconds)
{
    if (deltaSeconds > 0.0f)
    {
        PreviewScene->GetWorld()->Tick(LEVELTICK_All, deltaSeconds);
    }
}
void FWfcTilesetEditorViewportClient::Draw(FViewport* viewport, FCanvas* canvas)
{
    
}
