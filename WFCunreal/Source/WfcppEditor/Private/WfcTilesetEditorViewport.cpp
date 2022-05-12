#include "WfcTilesetEditorViewport.h"

#include "WfcTilesetEditorViewportClient.h"
#include "WfcTilesetEditorViewportToolBar.h"


void SWfcTilesetEditorViewport::Construct(const FArguments& args)
{
    scene = MakeShareable(new FWfcTilesetEditorScene);
    
    SEditorViewport::Construct(SEditorViewport::FArguments());

    //TODO: See the tail end of SMaterialEditor3DPreviewViewport::Construct(), which adds a callback for when the asset's properties change!
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
