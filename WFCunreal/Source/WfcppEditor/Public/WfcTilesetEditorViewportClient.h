#pragma once

#include "EditorModeManager.h"
#include "EditorViewportClient.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//Controls the actors/components in the 3D tile visualization scene.
//The scene's setup and data is handled by the "preview scene", FWfcTilesetEditorScene.
class FWfcTilesetEditorViewportClient : public FEditorViewportClient
{
public:

    //NOTE: the 'nullptr' in the base constructor is a "FEditorModeTools", which I believe represents
    //    a mode of editor interaction, like how the main scene editor has Terrain, mesh placement, etc.
    FWfcTilesetEditorViewportClient() : FEditorViewportClient(nullptr) { }

    virtual void Draw(FViewport* viewport, FCanvas* canvas) override;
    virtual void Tick(float deltaSeconds) override;
    
private:
    
};