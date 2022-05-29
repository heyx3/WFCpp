#include "WfcTilesetEditorScene.h"

#include "GameFramework/WorldSettings.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "WfcppEditorModule.h"
#include "WfcTileVisualizer.h"


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
            case PointID::a: symmetryText = TEXT("a"); break;
            case PointID::b: symmetryText = TEXT("b"); break;
            case PointID::c: symmetryText = TEXT("c"); break;
            case PointID::d: symmetryText = TEXT("d"); break;
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

    constexpr float initialTileSize = 1000.0f;
    FVector boxExtents(initialTileSize / 2.0f);
    float thickness = GetFaceThickness(initialTileSize);
    boxExtents[WFC::Tiled3D::GetAxisIndex(inOutData.Dir)] = thickness / 2.0f;
    box->SetBoxExtent(boxExtents);
    
    auto boxPos = inOutData.Pos;
    boxPos += (thickness / 2.0f) * ConvertVec(WFC::Tiled3D::GetFaceDirection(inOutData.Dir));
    box->SetWorldLocation(boxPos);
}


FWfcTilesetEditorScene::FWfcTilesetEditorScene(ConstructionValues cvs)
    : FAdvancedPreviewScene(cvs)
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
}

void FWfcTilesetEditorScene::Refresh(const UWfcTileset* tileset, TOptional<WfcTileID> tile, const FVector& camPos)
{
    //Error-checking.
    if (tileset != nullptr && tile.IsSet())
    {
        checkf(tileset->Tiles.Contains(*tile),
               TEXT("Gave nonexistent tile %i for tileset %s"),
               *tile, *tileset->GetFullName());
    }
    
    //Calculate properties in case the tileset/tile is null.
    float tileLength = (tileset == nullptr) ? 1000.0f : tileset->TileLength;
    
    for (auto& face : faces)
    {
        uint_fast8_t mainAxis, planeAxis1, planeAxis2;
        WFC::Tiled3D::GetAxes(face.Dir, mainAxis, planeAxis1, planeAxis2);

        //Update position.
        face.Pos = ConvertVec(WFC::Tiled3D::GetFaceDirection(face.Dir)) * (tileLength / 2.0f);
        face.Pos[mainAxis] += GetFaceThickness(tileLength) * 0.5f * FMath::Sign(face.Pos[mainAxis]);
        face.Shape->SetWorldLocation(face.Pos);

        //Update individual corners of the face.
        for (auto& point : face.Points)
        {
            //Update position.
            FVector pointPos;
            point.Pos[mainAxis] = face.Pos[mainAxis] * 1.15f;
            point.Pos[planeAxis1] = (tileLength / 2.0f) *
                                    (WFC::Tiled3D::IsFirstMin(point.CornerType) ? -1 : 1);
            point.Pos[planeAxis2] = (tileLength / 2.0f) *
                                    (WFC::Tiled3D::IsSecondMin(point.CornerType) ? -1 : 1);
            point.Shape->SetWorldLocation(point.Pos);
            point.Label->SetWorldLocation(point.Pos);

            //Update size.
            point.Shape->SetSphereRadius(GetCornerSphereRadius(tileLength));

            //Update labels.
            point.Label->SetWorldSize(GetLabelThickness(tileLength));
            point.Label->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(point.Pos, camPos));
            if (tileset != nullptr && tile.IsSet())
            {
                const auto& assetFace = tileset->Tiles[*tile].GetFace(face.Dir);
                const auto& assetFacePrototype = tileset->FacePrototypes[assetFace.PrototypeID];
                
                auto prototypeCornerType = assetFace.GetPrototypeCorner(point.CornerType);
                auto pointSymmetry = assetFacePrototype.GetPoint(prototypeCornerType);
                auto text = MakePointLabel(point.CornerType, pointSymmetry);
                
                point.Label->SetText(FText::FromString(MakePointLabel(point.CornerType, pointSymmetry)));
            }
            else
            {
                point.Label->SetText(FText::FromString(MakePointLabel(point.CornerType)));
            }
        }
    }
}