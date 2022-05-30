#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Views/STileView.h"

#include "WfcTileset.h"

class IWfcTilesetEditor : public FAssetEditorToolkit
{
public:
	virtual UWfcTileset* GetAsset() const = 0;
	virtual void SetAsset(UWfcTileset* asset) = 0;
};


extern const FName WfcTileset_TabID_Properties, WfcTileset_TabID_TileSelector;

//An editor for WFC Tileset data.
class WFCPPEDITOR_API FWfcTilesetEditor : public IWfcTilesetEditor
{
public:
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager) override;

	void InitWfcTilesetEditorEditor(const EToolkitMode::Type mode,
								    const TSharedPtr<class IToolkitHost>& initToolkitHost,
								    UWfcTileset* tileset);

    FWfcTilesetEditor();
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

    //Meant to be usd by WfcTilesetEditorSceneViewTab:
    TSharedRef<SWidget> SpawnSceneView();
    
private:

	TSharedRef<SDockTab> GeneratePropertiesTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileSelectorTab(const FSpawnTabArgs& args);

    void RefreshTileChoices();
    void OnTileSelected(TSharedPtr<FString> name, ESelectInfo::Type);

    void OnTilesetEdited(const FPropertyChangedEvent&);
    void OnSceneTick(float deltaSeconds);

    // ReSharper disable once CppUninitializedNonStaticDataMember
    UWfcTileset* tileset;
    TOptional<WfcTileID> tileToVisualize;
    TArray<TSharedPtr<FString>> tilesetTileSelectorChoices;
    TArray<int> tilesetTileSelectorChoiceIDs;

	TSharedPtr<SDockTab> propertiesTab, tileSelectorTab, tileSceneTab;
	TSharedPtr<IDetailsView> detailsView;
    TSharedPtr<STextComboBox> tileSelector;
    
    TSharedPtr<struct FWfcTilesetEditorSceneViewTab> tileSceneTabFactory;
    TSharedPtr<class SWfcTilesetTabBody> tileSceneTabBody;
};