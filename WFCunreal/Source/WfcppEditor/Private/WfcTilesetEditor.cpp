#include "WfcTilesetEditor.h"

#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Editor.h"

#include "WfcppEditorModule.h"


#define LOCTEXT_NAMESPACE "WfcTilesetEditor"

const FName ToolkitName(TEXT("WfcTilesetEditor"));
const FName FWfcTilesetEditor::PropertiesTabID(TEXT("WfcTilesetEditor_Properties"));


void FWfcTilesetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	//Add a new workspace menu category to the tab manager.
	WorkspaceMenuCategory = tabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_WfcTilesetEditor", "WFC Tileset Editor"));

	//We register the tab manager to the asset editor toolkit so we can use it in this editor.
	FAssetEditorToolkit::RegisterTabSpawners(tabManager);

	//Register the properties tab spawner within our tab manager.
	tabManager->RegisterTabSpawner(PropertiesTabID, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GeneratePropertiesTab))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}
void FWfcTilesetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	//Invoke base-class logic.
	FAssetEditorToolkit::UnregisterTabSpawners(tabManager);
	
	//Also remove our custom Properties tab.
	tabManager->UnregisterTabSpawner(PropertiesTabID);
}
TSharedRef<SDockTab> FWfcTilesetEditor::GeneratePropertiesTab(const FSpawnTabArgs& args) const
{
	check(args.GetTabId() == PropertiesTabID);

	return SNew(SDockTab)
		     .Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
			 .Label(LOCTEXT("GenericDetailsTitle", "Details"))
			 .TabColorScale(GetTabColorScale())
			 [
			     detailsView.ToSharedRef()
			 ];
}

void FWfcTilesetEditor::InitWfcTilesetEditorEditor(const EToolkitMode::Type mode,
												   const TSharedPtr<IToolkitHost>& initToolkitHost,
												   UWfcTileset* newTileset)
{
	SetAsset(newTileset);
	
	//Retrieve the property editor module and assign properties to the DetailsView.
	auto& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	detailsView = propertyEditorModule.CreateDetailView(FDetailsViewArgs(
		false, true, true,
		FDetailsViewArgs::ObjectsUseNameArea,
		false
	));

	//Create our editor's layout.
	auto standaloneDefaultLayout = FTabManager::NewLayout("Standalone_WfcTilesetEditor_Layout_v1")
	->AddArea(
		//Put the toolbar in a vertical area.
		FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split(
				FTabManager::NewSplitter()
				->Split(
					FTabManager::NewStack()
					->AddTab(PropertiesTabID, ETabState::OpenedTab)
				)
			)
	);

	//Start up the editor.
	FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, WfcTilesetEditorAppIdentifier,
									     standaloneDefaultLayout, true, true,
									     (UObject*)tileset);
	if (detailsView.IsValid())
		detailsView->SetObject((UObject*)tileset);
}

FWfcTilesetEditor::~FWfcTilesetEditor()
{
	//TODO: why do we manually release shared pointers in the destructor? Can't we let them clean themselves up?
	detailsView.Reset();
	propertiesTab.Reset();
}


FName FWfcTilesetEditor::GetToolkitFName() const { return ToolkitName; }
FText FWfcTilesetEditor::GetBaseToolkitName() const { return LOCTEXT("AppLabel", "WFC Tileset Editor"); }
FText FWfcTilesetEditor::GetToolkitToolTipText() const { return LOCTEXT("ToolTip", "WFC Tileset Editor"); }
FString FWfcTilesetEditor::GetWorldCentricTabPrefix() const { return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString(); }
FLinearColor FWfcTilesetEditor::GetWorldCentricTabColorScale() const { return FColor::Red; }


#undef LOCTEXT_NAMESPACE