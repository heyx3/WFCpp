#pragma once

#include "SCommonEditorViewportToolbarBase.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//A Slate widget displaying the overlay on the 3D viewport (e.x. lighting mode dropdown, perspective vs ortho options).
class SWfcTilesetEditorViewportToolBar : public SCommonEditorViewportToolbarBase
{
public:
    SLATE_BEGIN_ARGS(SWfcTilesetEditorViewportToolBar) { }
    SLATE_END_ARGS()

    void Construct(const FArguments& args, TSharedPtr<class SWfcTilesetEditorViewport> viewport);
};