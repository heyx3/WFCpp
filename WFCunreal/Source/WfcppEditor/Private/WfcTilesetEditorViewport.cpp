#include "WfcTilesetEditorViewport.h"

#include "WfcTilesetEditorViewportClient.h"
#include "WfcTilesetEditorViewportToolBar.h"


void SWfcTilesetEditorViewport::Construct(const FArguments& args, FWfcTilesetEditorScene& inScene)
{
    scene = &inScene;
    SEditorViewport::Construct(SEditorViewport::FArguments());
}

//Toolbar interface:
TSharedRef<SEditorViewport> SWfcTilesetEditorViewport::GetViewportWidget()
{
    return SharedThis(this);
}
TSharedPtr<FExtender> SWfcTilesetEditorViewport::GetExtenders() const
{
    return MakeShareable(new FExtender);
}
void SWfcTilesetEditorViewport::OnFloatingButtonClicked()
{
    //Do nothing
}

//SEditorViewport interface:
TSharedRef<FEditorViewportClient> SWfcTilesetEditorViewport::MakeEditorViewportClient()
{
    viewportClient = MakeShareable(new FWfcTilesetEditorViewportClient(*scene));
    viewportClient->ViewportType = LVT_Perspective;
    viewportClient->bSetListenerPosition = false;

    return viewportClient.ToSharedRef();
}
TSharedPtr<SWidget> SWfcTilesetEditorViewport::MakeViewportToolbar()
{
    return SNew(SWfcTilesetEditorViewportToolBar, SharedThis(this))
            .Cursor(EMouseCursor::Default);
}
