#include "WfcppEditorModule.h"

#include "Modules/ModuleManager.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "PropertyEditorModule.h"
#include "IDetailChildrenBuilder.h"

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

	    //Register a custom Properties widget for tilesets.
	    auto& propEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	    // propEditorModule.RegisterCustomClassLayout(UWfcTileset::StaticClass()->GetFName(),
	    //                                            FOnGetDetailCustomizationInstance::CreateLambda([]() { return MakeShared(FTilesetDetailsCustomization()); }));
	    //TODO: Re-enable this
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

	    auto& propEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	    propEditorModule.UnregisterCustomClassLayout(UWfcTileset::StaticClass()->GetFName());
	}

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return menuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override { return toolBarExtensibilityManager; }

	virtual TSharedRef<IWfcTilesetEditor> CreateCustomAssetEditor(EToolkitMode::Type mode,
																  const TSharedPtr<IToolkitHost>& initToolkitHost,
																  UWfcTileset* tileset) override
	{
	    auto* editor = new FWfcTilesetEditor;
	    TSharedRef<IWfcTilesetEditor> editorRef(editor);
	    
		editor->InitWfcTilesetEditorEditor(mode, initToolkitHost, tileset);
		return editorRef;
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
