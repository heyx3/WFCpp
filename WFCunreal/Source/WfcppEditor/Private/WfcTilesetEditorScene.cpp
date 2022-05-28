#include "WfcTilesetEditorScene.h"

#include "WfcppEditorModule.h"
#include "GameFramework/WorldSettings.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"


//Most visual settings are encapsulated in this namespace.
namespace
{
    const FLinearColor axisColors[6] = {
        //Note that this order should match the order of the enum WFC::Tiled3D::Directions3D.

        //Negative/Positive X:
        { 0.6f, 0.1f, 0.1f },
        { 1.0f, 0.0f, 0.0f },

        //Negative/Positive Y:
        { 0.1f, 0.6f, 0.1f },
        { 0.0f, 1.0f, 0.0f },

        //Negative/Positive Z:
        { 0.1f, 0.1f, 0.6f },
        { 0.0f, 0.0f, 1.0f }
    };

    float GetTileSize(const UWfcTileset* optionalTileset)
    {
        return (optionalTileset == nullptr) ?
                   1000.0f :
                   optionalTileset->TileLength;
    }

    float GetFaceThickness(float tileSize) { return tileSize / 50.0f; }
    float GetCornerSphereRadius(float tileSize) { return tileSize / 100.0f; }

    auto GetLabelColor(WFC::Tiled3D::Directions3D face)
    {
        return FLinearColor::LerpUsingHSV(axisColors[face], FLinearColor::White, 0.3f)
                    .ToFColor(false);
    }
    auto GetLabelThickness(float tileSize) { return 50.0f * (tileSize / 1000.0f); }

    auto ConvertVec(const WFC::Vector3i& v) { return FVector(v.x, v.y, v.z); }

    FString MakePointLabel(WFC::Tiled3D::FacePoints corner,
                           TOptional<PointID> symmetry = TOptional<PointID>())
    {
        const auto* cornerText = TEXT("ERROR");
        switch (corner)
        {
            case WFC::Tiled3D::FacePoints::AA: cornerText = TEXT("AA"); break;
            case WFC::Tiled3D::FacePoints::AB: cornerText = TEXT("AB"); break;
            case WFC::Tiled3D::FacePoints::BA: cornerText = TEXT("BA"); break;
            case WFC::Tiled3D::FacePoints::BB: cornerText = TEXT("BB"); break;
            default: check(false);
        }
        if (!symmetry.IsSet())
            return cornerText;

        const auto* symmetryText = TEXT("ERROR2");
        switch (symmetry.GetValue())
        {
            case PointID::_0: symmetryText = TEXT("0"); break;
            case PointID::_1: symmetryText = TEXT("1"); break;
            case PointID::_2: symmetryText = TEXT("2"); break;
            case PointID::_3: symmetryText = TEXT("3"); break;
            default: check(false);
        }

        return FString(cornerText) + " : " + symmetryText;
    }
}


void FWfcTilesetEditorScene::InitializeFacePointViz(WFC::Tiled3D::Directions3D face,
                                                    FacePointViz& inOutData)
{
    const auto& color = axisColors[face];
    
    auto* sphere = NewObject<USphereComponent>(GetTransientPackage());
    AddComponent(sphere, FTransform());
    inOutData.Shape = sphere;
    sphere->SetWorldLocation(inOutData.Pos);
    sphere->ShapeColor = color.ToFColor(false);

    //Put some text next to the sphere, indicating which corner this is.
    auto* text = NewObject<UTextRenderComponent>();
    AddComponent(text, FTransform());
    inOutData.Label = text;
    text->TextRenderColor = GetLabelColor(face);
    text->SetWorldLocation(inOutData.Pos);
    text->HorizontalAlignment = EHTA_Center;
    text->VerticalAlignment = EVRTA_TextBottom;
    text->SetText(FText::FromString(MakePointLabel(inOutData.CornerType)));
}
void FWfcTilesetEditorScene::InitializeFaceViz(FaceViz& inOutData)
{
    //Add a wireframe plane for the face.
    //Actually a Box component, because there's no "plane" component.
    
    auto* box = NewObject<UBoxComponent>(GetTransientPackage());
    AddComponent(box, FTransform());
    inOutData.Shape = box;
    
    box->ShapeColor = axisColors[inOutData.Dir].ToFColor(false);
    box->bVisualizeComponent = true; //TODO: Probably not needed.
    box->bHiddenInGame = false; //TODO: Probably not needed.
    
    FVector boxExtents(currentTileSize / 2.0f);
    float thickness = GetFaceThickness(currentTileSize);
    boxExtents[WFC::Tiled3D::GetAxisIndex(inOutData.Dir)] = thickness / 2.0f;
    box->SetBoxExtent(boxExtents);
    
    auto boxPos = inOutData.Pos;
    boxPos += (thickness / 2.0f) * ConvertVec(WFC::Tiled3D::GetFaceDirection(inOutData.Dir));
    box->SetWorldLocation(boxPos);
}



FWfcTilesetEditorScene::FWfcTilesetEditorScene(ConstructionValues cvs)
    : FAdvancedPreviewScene(cvs),
      compTileViz(nullptr)
{
    auto& world = *GetWorld();
    auto& worldSettings = *world.GetWorldSettings();

    //Start up the world.
    worldSettings.NotifyBeginPlay();
    worldSettings.NotifyMatchStarted();
    worldSettings.SetActorHiddenInGame(false);
    world.bBegunPlay = true;

    //Configure the scene.
    SetFloorVisibility(false);
    
    //Set up the visualization of each face.
    currentTileSize = 1000.0f;
    for (int i = 0; i < N_DIRECTIONS3D; ++i)
    {
        auto face = static_cast<WFC::Tiled3D::Directions3D>(i);
        faces[i].Dir = face;
        InitializeFaceViz(faces[i]);

        for (int j = 0; j < WFC_N_FACE_POINTS; ++j)
        {
            auto& cornerField = faces[i].Points[j];
            cornerField.CornerType = static_cast<WFC::Tiled3D::FacePoints>(j);
            InitializeFacePointViz(face, cornerField);
        }
    }
    UpdateSizing(currentTileSize);
}


void FWfcTilesetEditorScene::UpdateSizing(float newTileSize)
{
    currentTileSize = newTileSize;
    for (auto& face : faces)
    {
        uint_fast8_t mainAxis, planeAxis1, planeAxis2;
        WFC::Tiled3D::GetAxes(face.Dir, mainAxis, planeAxis1, planeAxis2);
        
        face.Pos = ConvertVec(WFC::Tiled3D::GetFaceDirection(face.Dir))
                     * (currentTileSize / 2.0f);
        face.Pos[mainAxis] += GetFaceThickness(newTileSize) * 0.5f *
                                FMath::Sign(face.Pos[mainAxis]);
        
        face.Shape->SetWorldLocation(face.Pos);

        for (auto& point : face.Points)
        {
            FVector pointPos;
            point.Pos[mainAxis] = face.Pos[mainAxis] * 1.15f;
            point.Pos[planeAxis1] = (currentTileSize / 2.0f) *
                                    (WFC::Tiled3D::IsFirstMin(point.CornerType) ? -1 : 1);
            point.Pos[planeAxis2] = (currentTileSize / 2.0f) *
                                    (WFC::Tiled3D::IsSecondMin(point.CornerType) ? -1 : 1);

            point.Shape->SetWorldLocation(point.Pos);
            point.Label->SetWorldLocation(point.Pos);

            point.Label->SetWorldSize(GetLabelThickness(newTileSize));
            
            point.Shape->SetSphereRadius(GetCornerSphereRadius(currentTileSize));
        }
    }
}
void FWfcTilesetEditorScene::RefreshTileViz(const FVector& camPos)
{
    //Make billboards face the camera.
    for (auto& face : faces)
        for (auto& point : face.Points)
            point.Label->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(point.Pos, camPos));
}

void FWfcTilesetEditorScene::SetTileset(const UWfcTileset* newTileset)
{
    tileset = newTileset;
    UpdateSizing(tileset->TileLength);

    if (compTileViz != nullptr)
        RemoveComponent(compTileViz);
    compTileViz = nullptr;
}
void FWfcTilesetEditorScene::SetTile(WfcTileID tile, UActorComponent* tileVisualization)
{
    checkf(tileset != nullptr, TEXT("Trying to visualize a tile from a null tileset"));
    checkf(tileset->Tiles.Contains(tile),
            TEXT("Tile with ID %i doesn't exist in tileset '%s'"),
            tile, *tileset->GetFullName());
    
    if (compTileViz != nullptr)
        RemoveComponent(compTileViz);
    
    AddComponent(tileVisualization, FTransform());
    compTileViz = tileVisualization;
}
void FWfcTilesetEditorScene::SetFace(WFC::Tiled3D::Directions3D dir)
{
    //TODO: Implement visualization of neighbor tile
}

//TODO: Try intercepting 'UChildActorComponent' and spawn the actor directly into the UWorld, instead of actually using that component.
