#pragma once

#include "PreviewScene.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//Stores the actors/components in the 3D tile visualization scene.
//The runtime logic for the scene is handled by the "viewport client", FWfcTilesetEditorViewportClient.
class FWfcTilesetEditorScene : public FPreviewScene
{
public:
    FWfcTilesetEditorScene(ConstructionValues cvs);

    UActorComponent* GetTileViz() const { return compTileViz; }
    UStaticMeshComponent* GetSkySphere() const { return compSkySphere; }
    
    void SetTileViz(UActorComponent* newTileViz);
    
private:

    UActorComponent* compTileViz;
    UStaticMeshComponent* compSkySphere;
};