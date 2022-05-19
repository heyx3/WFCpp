#pragma once

#include <array>

#include "Tiled3D/Transform3D.h"

#include "AdvancedPreviewScene.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


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

    //Returns the 6 cube faces used for visualizing.
    const auto& GetVizFaces() { return facesViz; }
    
private:

    UActorComponent* compTileViz;

    //Maps an axis (0-2) and sign (-1 or +1) to a unique index (0-5).
    int GetFaceIndex(int axis, int sign) { return axis + (((sign+1) / 2)*3); }
    
    std::array<UBoxComponent*, 6> facesViz;
    std::array<std::array<USphereComponent*, WFC_N_FACE_POINTS>, 6> faceVerticesViz;
    std::array<std::array<class UTextRenderComponent*, WFC_N_FACE_POINTS>, 6> faceVerticesTextViz;
};