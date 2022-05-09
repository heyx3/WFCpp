#pragma once

#include "AdvancedPreviewScene.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//Stores the actors/components in the 3D tile visualization scene.
//The runtime logic for the scene is handled by the "viewport client", FWfcTilesetEditorViewportClient.
class FWfcTilesetEditorScene : public FAdvancedPreviewScene
{
public:
    FWfcTilesetEditorScene(ConstructionValues cvs = ConstructionValues());

    UActorComponent* GetTileViz() const { return compTileViz; }
    void SetTileViz(UActorComponent* newTileViz);
    
private:

    UActorComponent* compTileViz;
};