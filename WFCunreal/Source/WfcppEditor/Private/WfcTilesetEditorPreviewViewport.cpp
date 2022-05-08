#include "WfcTilesetEditorPreviewViewport.h"

#include "WfcTilesetEditorViewportClient.h"
#include "WfcTilesetEditorViewportToolBar.h"

//Toolbar interface:
TSharedRef<SEditorViewport> SWfcTilesetEditorPreviewViewport::GetViewportWidget()
{
    return SharedThis(this);
}
TSharedPtr<FExtender> SWfcTilesetEditorPreviewViewport::GetExtenders() const
{
    return MakeShareable(new FExtender);
}
void SWfcTilesetEditorPreviewViewport::OnFloatingButtonClicked()
{
    //Do nothing
}

//SEditorViewport interface:
TSharedRef<FEditorViewportClient> SWfcTilesetEditorPreviewViewport::MakeEditorViewportClient()
{
    viewportClient = MakeShareable(new FWfcTilesetEditorViewportClient);
    viewportClient->ViewportType = LVT_Perspective;
    viewportClient->bSetListenerPosition = false;

    return viewportClient.ToSharedRef();
}
TSharedPtr<SWidget> SWfcTilesetEditorPreviewViewport::MakeViewportToolbar()
{
    return SNew(SWfcTilesetEditorViewportToolBar, SharedThis(this))
            .Cursor(EMouseCursor::Default);
}
