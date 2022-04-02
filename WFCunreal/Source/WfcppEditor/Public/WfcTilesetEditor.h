#pragma once

#include "CoreMinimal.h"
#include "WfcTileset.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Views/STileView.h"

class UWfcTileset;


//TODO: Use the module Editor/AdvancedPreviewScene

class IWfcTilesetEditor : public FAssetEditorToolkit
{
public:
	virtual UWfcTileset* GetAsset() const = 0;
	virtual void SetAsset(UWfcTileset* asset) = 0;
};


extern const FName WfcTileset_TabID_Properties, WfcTileset_TabID_FaceSelector,
                   WfcTileset_TabID_TileSelector, WfcTileset_TabID_TileViewer;

class WFCPPEDITOR_API FWfcTilesetEditor : public IWfcTilesetEditor
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;

	void InitWfcTilesetEditorEditor(const EToolkitMode::Type mode,
								    const TSharedPtr<class IToolkitHost>& initToolkitHost,
								    UWfcTileset* tileset);

	virtual ~FWfcTilesetEditor() override;

	//IToolkit interface:
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override { return true; }

	//IWfcTilesetEditor interface:
	virtual UWfcTileset* GetAsset() const override { return tileset; }
	virtual void SetAsset(UWfcTileset* asset) override;

private:

	TSharedRef<SDockTab> GeneratePropertiesTab(const FSpawnTabArgs& args);
	TSharedRef<SDockTab> GenerateExperimentsTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateFaceSelectorTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileSelectorTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileViewerTab(const FSpawnTabArgs& args);

	//Callbacks for face prototype data editors:
	FReply GenerateFacePrototypeData();
	FReply RemoveFacePrototypeData(int ID);
	TSharedRef<ITableRow> GenerateFacePrototypeUI(TSharedPtr<int> item,
													 const TSharedRef<STableViewBase>& owner);
    
    //Callbacks for tile data editors:
    FReply GenerateTileData();
    FReply RemoveTileData(int ID);
    TSharedRef<ITableRow> GenerateTileUI(TSharedPtr<int> item,
                                         const TSharedRef<STableViewBase>& owner);
	
	UWfcTileset* tileset;
	//Internal references to the asset data for editing in particular formats.
    TArray<TSharedPtr<int>> tilesetBuf_PrototypeIDs, tilesetBuf_TileIDs;

	TSharedPtr<SDockTab> propertiesTab, faceSelectorTab, tileSelectorTab;
	TSharedPtr<class IDetailsView> detailsView;
	TSharedPtr<SListView<TSharedPtr<int>>> faceSelector;
    TSharedPtr<STileView<TSharedPtr<int>>> tileSelector;
};