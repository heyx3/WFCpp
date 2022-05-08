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


extern const FName WfcTileset_TabID_Properties, WfcTileset_TabID_TileSelector,
                   WfcTileset_TabID_TileScene;

//An editor for WFC Tileset data.
//Has pure virtual functions related to the visualization of the tile's data.
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

protected:
    //Makes some kind of preview thumbnail for the given tile.
    //By default, creates a simple label.
    virtual TSharedRef<SWidget> TileViz_MakePreview(int tileID);

    //Creates a 3D visualization of a given tile.
    //TODO: Default behavior looks for UClass* of AActor or UActorComponent, and creates an instance of them.
    virtual UActorComponent* TileViz_Create(int tileID) = 0;
    //Cleans up the 3D visualization for a given tile.
    //Does not need to actually call DestroyComponent(), just handle other cleanup.
    virtual void TileViz_Destroy(int tileID, UActorComponent* tileViz) { }

private:

	TSharedRef<SDockTab> GeneratePropertiesTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileSelectorTab(const FSpawnTabArgs& args);
    TSharedRef<SDockTab> GenerateTileSceneTab(const FSpawnTabArgs& args);

    void RefreshTileChoices();
    void OnTileSelected(TSharedPtr<FString> name, ESelectInfo::Type);

    // ReSharper disable once CppUninitializedNonStaticDataMember
    UWfcTileset* tileset;
    TArray<TSharedPtr<FString>> tilesetTileSelectorChoices;
    TArray<int> tilesetTileSelectorChoiceIDs;

	TSharedPtr<SDockTab> propertiesTab, tileSelectorTab, tileSceneTab;
	TSharedPtr<class IDetailsView> detailsView;
    TSharedPtr<STileView<TSharedPtr<int>>> tileSelector;
    
    TSharedPtr<class FAdvancedPreviewScene> tileScene;
    FSceneInterface* tileSceneController;
    UActorComponent* tileVizActor = nullptr;
};


class WFCPPEDITOR_API FWfcTilesetEditor_Actors : public FWfcTilesetEditor
{
protected:
    virtual UActorComponent* TileViz_Create(int tileID) override;
};