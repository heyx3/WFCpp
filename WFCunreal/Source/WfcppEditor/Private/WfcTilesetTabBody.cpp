#include "WfcTilesetTabBody.h"

#include "WfcTilesetEditorScene.h"
#include "WfcTilesetEditorViewportClient.h"
#include "WfcTilesetEditorViewport.h"

void SWfcTilesetTabBody::Construct(const FArguments& inArgs, FWfcTilesetEditorScene& scene)
{
    viewportWidget = SNew(SWfcTilesetEditorViewport, scene);
    viewportClient = StaticCastSharedPtr<FWfcTilesetEditorViewportClient>(viewportWidget->GetViewportClient());

    TSharedPtr<SVerticalBox> viewportContainer = nullptr;
    this->ChildSlot [
        SAssignNew(viewportContainer, SVerticalBox)
            +SVerticalBox::Slot()
                .VAlign(VAlign_Fill)
                .HAlign(HAlign_Fill)
                .FillHeight(1.0f) [
                    SNew(SOverlay)
                        +SOverlay::Slot() [
                            viewportWidget.ToSharedRef()
                        ]
                ]
    ];
}
