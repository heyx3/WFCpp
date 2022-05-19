#include "WfcTilesetEditorScene.h"

#include "WfcppEditorModule.h"
#include "GameFramework/WorldSettings.h"
#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"


//TODO: Function for when the tileset asset (and therefore the tile size) is known.

FWfcTilesetEditorScene::FWfcTilesetEditorScene(ConstructionValues cvs)
    : FAdvancedPreviewScene(cvs),
      compTileViz(nullptr)
{
    facesViz.fill(nullptr);
    for (auto& inner : faceVerticesViz)
        inner.fill(nullptr);
    for (auto& inner : faceVerticesTextViz)
        inner.fill(nullptr);
    
    auto& world = *GetWorld();
    auto& worldSettings = *world.GetWorldSettings();

    //Start up the world.
    worldSettings.NotifyBeginPlay();
    worldSettings.NotifyMatchStarted();
    worldSettings.SetActorHiddenInGame(false);
    world.bBegunPlay = true;

    const float tileSize = 1000.0f; //TODO: Update the offset once the tileset asset's tile size is known.
    
    //Set up the visualization objects for each face.
    FLinearColor axisColors[6] = {
        //Negative XYZ:
        { 0.6f, 0.1f, 0.1f },
        { 0.1f, 0.6f, 0.1f },
        { 0.1f, 0.1f, 0.6f },
        //Positive XYZ:
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };
    for (int axis = 0; axis < 3; ++axis)
        for (int sign = -1; sign < 2; sign += 2)
        {
            int index = GetFaceIndex(axis, sign);
            auto color = axisColors[index];

            //Find the other two axes, in order.
            //They should line up with the ordering of the WFC::Tiled3D::FacePoints enum.
            int axis2 = (axis + 1) % 3,
                axis3 = (axis2 + 1) % 3;
            if (axis3 < axis2)
                Swap(axis2, axis3);
            
            FVector facePos = FVector::ZeroVector;
            facePos[axis] = sign * tileSize / 2.0f;
            
            //Add a wireframe plane for the face.
            //Actually a Box component, because there's no "plane" component.
            auto* box = NewObject<UBoxComponent>(GetTransientPackage());
            AddComponent(box, FTransform());
            facesViz[index] = box;
            box->ShapeColor = color.ToFColor(false);
            box->bVisualizeComponent = true;
            box->bHiddenInGame = false;
            FVector boxExtents;
            const float thickness = 20.0f;
            for (int i = 0; i < 3; ++i)
                boxExtents[i] = (i == axis) ? (thickness/2.0f) : (tileSize / 2.0f);
            box->SetBoxExtent(boxExtents);
            auto boxPos = facePos;
            boxPos[axis] += sign * thickness / 2.0f;
            box->SetWorldLocation(boxPos);

            //Set up the visualization of each point on the face.
            for (int vert = 0; vert < WFC_N_FACE_POINTS; ++vert)
            {
                auto cornerType = static_cast<WFC::Tiled3D::FacePoints>(vert);
                
                auto* sphere = NewObject<USphereComponent>(GetTransientPackage());
                AddComponent(sphere, FTransform());
                faceVerticesViz[index][vert] = sphere;

                sphere->ShapeColor = color.ToFColor(false);

                sphere->SetSphereRadius(10.0f);
                
                auto spherePos = facePos;
                spherePos[axis] *= 1.15f;
                FVector2D horzDir = { WFC::Tiled3D::IsFirstMin(cornerType) ? -1.0f : 1.0f,
                                      WFC::Tiled3D::IsSecondMin(cornerType) ? -1.0f : 1.0f };
                spherePos[axis2] = horzDir.X * tileSize / 2.0f;
                spherePos[axis3] = horzDir.Y * tileSize / 2.0f;
                sphere->SetWorldLocation(spherePos);

                //Put some text next to the sphere, indicating which corner this is.
                auto* text = NewObject<UTextRenderComponent>();
                AddComponent(text, FTransform());
                faceVerticesTextViz[index][vert] = text;
                text->TextRenderColor = FMath::Lerp(color, FLinearColor::White, 0.3f).ToFColor(false);
                text->SetWorldLocation(spherePos);
                //TODO: Face the camera.
                text->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
                text->VerticalAlignment = EVerticalTextAligment::EVRTA_TextBottom;
                switch (cornerType)
                {
                    case WFC::Tiled3D::FacePoints::AA: text->SetText(FText::FromString(TEXT("AA"))); break;
                    case WFC::Tiled3D::FacePoints::AB: text->SetText(FText::FromString(TEXT("AB"))); break;
                    case WFC::Tiled3D::FacePoints::BA: text->SetText(FText::FromString(TEXT("BA"))); break;
                    case WFC::Tiled3D::FacePoints::BB: text->SetText(FText::FromString(TEXT("BB"))); break;
                    default: check(false);
                }
            }
        }

    //Configure the scene.
    SetFloorVisibility(false);
}

void FWfcTilesetEditorScene::SetTileViz(UActorComponent* newTileViz)
{
    if (compTileViz != nullptr)
    {
        RemoveComponent(compTileViz);
    }
    
    compTileViz = newTileViz;

    if (compTileViz != nullptr)
    {
        AddComponent(newTileViz, FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(1.0f)));

        if (compTileViz->IsA<UChildActorComponent>())
        {
            auto* childActorComponent = Cast<UChildActorComponent>(compTileViz);
            childActorComponent->CreateChildActor();
            childActorComponent->BeginPlay();
        }
    }
}
