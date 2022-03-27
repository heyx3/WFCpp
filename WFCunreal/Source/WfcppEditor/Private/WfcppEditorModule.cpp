#include "WfcppEditorModule.h"

#include "Modules/ModuleManager.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "AssetTypeActions_WfcTileset.h"
#include "WfcTilesetEditor.h"


class UWfcTileset;
#define LOCTEXT_NAMESPACE "FWfcppEditorModule"

const FName WfcTilesetEditorAppIdentifier = FName(TEXT("CustomAssetEditorApp"));
DEFINE_LOG_CATEGORY(LogWfcppEditor);


class FWfcppEditorModule : public IWfcTilesetEditorModule
{
public:

	virtual void StartupModule() override
	{
		UE_LOG(LogTemp, Warning, TEXT("StartupModule() WFC-editor"));
		
		menuExtensibilityManager = MakeShareable(new FExtensibilityManager);
		toolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

		auto& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		RegisterAssetTypeAction(assetTools, MakeShareable(new FAssetTypeActions_WfcTileset));
	}
	virtual void ShutdownModule() override
	{
		UE_LOG(LogTemp, Warning, TEXT("ShutdownModule() WFC-editor"));

		menuExtensibilityManager.Reset();
		toolBarExtensibilityManager.Reset();

		//Unregister our custom assets from the AssetTools module.
		if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
		{
			auto& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
			for (const auto& action : createdAssetTypeActions)
				assetTools.UnregisterAssetTypeActions(action.ToSharedRef());
		}
		createdAssetTypeActions.Empty();
	}

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return menuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return toolBarExtensibilityManager; }

	virtual TSharedRef<IWfcTilesetEditor> CreateCustomAssetEditor(EToolkitMode::Type mode,
																  const TSharedPtr<IToolkitHost>& initToolkitHost,
																  UWfcTileset* tileset) override
	{
		TSharedRef<FWfcTilesetEditor> newEditor(new FWfcTilesetEditor);
		newEditor->InitWfcTilesetEditorEditor(mode, initToolkitHost, tileset);
		return newEditor;
	}

private:
	TSharedPtr<FExtensibilityManager> menuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> toolBarExtensibilityManager;
	TArray<TSharedPtr<IAssetTypeActions>> createdAssetTypeActions;

	void RegisterAssetTypeAction(IAssetTools& assetTools, TSharedRef<IAssetTypeActions> action)
	{
		assetTools.RegisterAssetTypeActions(action);
		createdAssetTypeActions.Add(action);
	}
};


IMPLEMENT_GAME_MODULE(FWfcppEditorModule, WfcppEditor);


#undef LOCTEXT_NAMESPACE
