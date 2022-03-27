#pragma once

#include "CoreMinimal.h"
#include "WfcTileset.h"
#include "Toolkits/AssetEditorToolkit.h"

class UWfcTileset;


class IWfcTilesetEditor : public FAssetEditorToolkit
{
public:
	virtual UWfcTileset* GetAsset() const = 0;
	virtual void SetAsset(UWfcTileset* asset) = 0;
};


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
	virtual void SetAsset(UWfcTileset* asset) override { tileset = asset; }

	
private:
	static const FName PropertiesTabID;

	TSharedRef<SDockTab> GeneratePropertiesTab(const FSpawnTabArgs& args) const;
	
	UWfcTileset* tileset;

	TSharedPtr<SDockableTab> propertiesTab;
	TSharedPtr<class IDetailsView> detailsView;
};