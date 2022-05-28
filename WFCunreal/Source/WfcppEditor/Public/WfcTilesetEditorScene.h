#pragma once

#include <array>

#include "Tiled3D/Transform3D.h"
#include "WfcTileset.h"

#include "AdvancedPreviewScene.h"


class UBoxComponent;
class USphereComponent;
class UTextRenderComponent;

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//Stores the actors/components in the 3D tile visualization scene.
//The runtime logic for the scene is handled by the "viewport client", FWfcTilesetEditorViewportClient.
//Displays a main tile visualization component,
//    plus another tile oriented so that it can line up with a specific face on the main tile.
class FWfcTilesetEditorScene : public FAdvancedPreviewScene
{
public:
    FWfcTilesetEditorScene(ConstructionValues cvs = ConstructionValues());

    UActorComponent* GetTileViz() const { return compTileViz; }
 
    void SetTileset(const UWfcTileset* tileset);
    void SetTile(WfcTileID tile, UActorComponent* tileVisualization);
    void SetFace(WFC::Tiled3D::Directions3D dir);

    //Makes sure the visualization is up-to-date with the tileset data and camera.
    void RefreshTileViz(const FVector& camPos);
    
private:
    
    struct FacePointViz
    {
        FVector Pos;
        WFC::Tiled3D::FacePoints CornerType;
        
        USphereComponent* Shape;
        UTextRenderComponent* Label;
    };
    void InitializeFacePointViz(WFC::Tiled3D::Directions3D face, FacePointViz& inOutData);
    
    struct FaceViz
    {
        FVector Pos;
        WFC::Tiled3D::Directions3D Dir;
        
        UBoxComponent* Shape;
        TStaticArray<FacePointViz, WFC_N_FACE_POINTS> Points;
    };
    void InitializeFaceViz(FaceViz& inOutData);

    void UpdateSizing(float newTileSize);

    
    const UWfcTileset* tileset;
    WfcTileID chosenTile;
    float currentTileSize;
    
    UActorComponent* compTileViz;
    TStaticArray<FaceViz, N_DIRECTIONS3D> faces;
};