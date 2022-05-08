#pragma once

#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//A slate widget acting as the outermost handler for the 3D tile visualization scene.
class SWfcTilesetEditorPreviewViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
public:

    SLATE_BEGIN_ARGS(SWfcTilesetEditorPreviewViewport) { }
    SLATE_END_ARGS()

    //Toolbar interface:
    virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
    virtual TSharedPtr<FExtender> GetExtenders() const override;
    virtual void OnFloatingButtonClicked() override;

protected:
    //SEditorViewport interface:
    virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
    virtual TSharedPtr<SWidget> MakeViewportToolbar() override;

private:
    TSharedPtr<FEditorViewportClient> viewportClient;
};