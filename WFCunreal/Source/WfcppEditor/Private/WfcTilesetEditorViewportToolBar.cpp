#include "WfcTilesetEditorViewportToolBar.h"

#include "WfcTilesetEditorPreviewViewport.h"


void SWfcTilesetEditorViewportToolBar::Construct(const FArguments& args, TSharedPtr<SWfcTilesetEditorPreviewViewport> viewport)
{
    SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), viewport);
}
