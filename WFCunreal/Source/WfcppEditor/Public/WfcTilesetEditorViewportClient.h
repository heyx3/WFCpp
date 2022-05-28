#pragma once

#include "EditorModeManager.h"
#include "EditorViewportClient.h"

#include "WfcTilesetEditorScene.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//Controls the actors/components in the 3D tile visualization scene.
//The scene's setup and data is handled by the "preview scene", FWfcTilesetEditorScene.
class FWfcTilesetEditorViewportClient : public FEditorViewportClient
{
public:

    FWfcTilesetEditorViewportClient(FWfcTilesetEditorScene& scene);

    virtual void Draw(FViewport* viewport, FCanvas* canvas) override;
    virtual void Tick(float deltaSeconds) override;

private:

    bool firstTick = true;
};