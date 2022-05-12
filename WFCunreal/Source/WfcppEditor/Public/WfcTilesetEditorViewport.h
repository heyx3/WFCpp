#pragma once

#include "SEditorViewport.h"
#include "SCommonEditorViewportToolbarBase.h"

class FWfcTilesetEditorScene;

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

//A slate widget acting as the outermost handler for the 3D tile visualization scene.
class SWfcTilesetEditorViewport : public SEditorViewport, public ICommonEditorViewportToolbarInfoProvider
{
    //Slate stuff:
public:
    SLATE_BEGIN_ARGS(SWfcTilesetEditorViewport) { }
    SLATE_END_ARGS()
    void Construct(const FArguments& args);

    //Interface:
public:
    TSharedPtr<FWfcTilesetEditorScene> GetWfcScene() const { return scene; }
    
    //Toolbar interface:
public:
    virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
    virtual TSharedPtr<FExtender> GetExtenders() const override;
    virtual void OnFloatingButtonClicked() override;

    //SEditorViewport interface:
protected:
    virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
    virtual TSharedPtr<SWidget> MakeViewportToolbar() override;

    //Fields:
private:
    TSharedPtr<FEditorViewportClient> viewportClient;
    TSharedPtr<FWfcTilesetEditorScene> scene;
};