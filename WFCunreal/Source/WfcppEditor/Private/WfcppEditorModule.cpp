#include "WfcppEditorModule.h"

#include "Modules/ModuleManager.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "PropertyEditorModule.h"
#include "IDetailCustomization.h"

#include "AssetTypeActions_WfcTileset.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "PropertyCustomizationHelpers.h"
#include "SResetToDefaultMenu.h"

#include "WfcTilesetPropertiesBuilder_FacePrototypes.hpp"
#include "WfcTilesetEditor.h"


class UWfcTileset;
#define LOCTEXT_NAMESPACE "FWfcppEditorModule"

const FName WfcTilesetEditorAppIdentifier = FName(TEXT("CustomAssetEditorApp"));
DEFINE_LOG_CATEGORY(LogWfcppEditor);



struct FTilesetDetailsCustomization : public IDetailCustomization, IDetailCustomNodeBuilder
{
public:
    
    virtual void CustomizeDetails(IDetailLayoutBuilder& builder) override
    {
        layoutBuilder = &builder;
        
        const auto& selectedObjects = builder.GetSelectedObjects();
        check(selectedObjects.Num() > 0);
        if (selectedObjects.Num() == 1)
        {
            tileset = CastChecked<UWfcTileset>(selectedObjects[0].Get());

            const auto* detailsView = builder.GetDetailsView();
            detailsView->OnFinishedChangingProperties().AddSP(this, &FTilesetDetailsCustomization::FinishedChangingProperties);

            auto& structCategory = builder.EditCategory("WFC", LOCTEXT("WfcCategories", "Elements"));

            auto facePrototypesProp = builder.GetProperty(GET_MEMBER_NAME_CHECKED(UWfcTileset, FacePrototypes));
            structCategory.AddCustomBuilder(MakeShared<FWfcTilesetPropertiesBuilder_FacePrototypes>(facePrototypesProp));
            //TODO: Make something similar for tiles.
        }
    }

private:
    IDetailLayoutBuilder* layoutBuilder;
    TWeakObjectPtr<UWfcTileset> tileset;

    void FinishedChangingProperties(const FPropertyChangedEvent& event)
    {
        UE_LOG(LogWfcppEditor, Warning, TEXT("Property changed: %s, member %s, type %i"),
               *event.Property->GetFullName(),
               *event.MemberProperty->GetFullName(),
               static_cast<int>(event.ChangeType));
        
    }
};


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
