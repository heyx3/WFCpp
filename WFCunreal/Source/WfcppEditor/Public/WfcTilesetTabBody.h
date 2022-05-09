#pragma once

#include "Widgets/SCompoundWidget.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//Slate widget representing the contents of the "3D tile visualization" tab.
class SWfcTilesetTabBody : public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SWfcTilesetTabBody) { }
    SLATE_END_ARGS()

public:
    void Construct(const FArguments& inArgs, class FWfcTilesetEditorScene& scene);
private:
    TSharedPtr<class FWfcTilesetEditorViewportClient> viewportClient;
    TSharedPtr<class SWfcTilesetEditorViewport> viewportWidget;
};