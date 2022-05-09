#include "WfcTilesetEditorViewportToolBar.h"

#include "WfcTilesetEditorViewport.h"


void SWfcTilesetEditorViewportToolBar::Construct(const FArguments& args, TSharedPtr<SWfcTilesetEditorViewport> viewport)
{
    SCommonEditorViewportToolbarBase::Construct(SCommonEditorViewportToolbarBase::FArguments(), viewport);
}
