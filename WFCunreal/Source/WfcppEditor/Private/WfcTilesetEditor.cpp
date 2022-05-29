#include "WfcTilesetEditor.h"

#include "Modules/ModuleManager.h"

#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "PropertyCustomizationHelpers.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Layout/SScrollBox.h"
#include "SAssetDropTarget.h"
#include "EditorStyleSet.h"

#include "WfcppEditorModule.h"
#include "WfcTilesetEditorScene.h"
#include "WfcTilesetEditorSceneViewTab.h"
#include "WfcTilesetEditorViewport.h"
#include "WfcTilesetEditorViewportClient.h"
#include "WfcTilesetTabBody.h"


#define LOCTEXT_NAMESPACE "WfcTilesetEditor"

const FName ToolkitName(TEXT("WfcTilesetEditor"));

const FName WfcTileset_TabID_Properties   (TEXT("WfcTilesetEditor_Properties"));
const FName WfcTileset_TabID_TileSelector (TEXT("WfcTilesetEditor_TileSelector"));

namespace
{
    FString EventToString(FPropertyChangedEvent& ev)
    {
        return FString::Printf(TEXT("'%s' from '%s', object %i of %i, array idx %i"),
                               *ev.GetPropertyName().ToString(),
                               *ev.MemberProperty->GetName(),
                               ev.ObjectIteratorIndex, ev.GetNumObjectsBeingEdited(),
                               ev.GetArrayIndex(ev.GetPropertyName().ToString()));
    }
    FString EventToString(FPropertyChangedChainEvent& ev)
    {
        FString baseStr = EventToString(static_cast<FPropertyChangedEvent&>(ev));
        baseStr += " [";

        for (auto* chain : ev.PropertyChain)
            baseStr += FString::Printf(TEXT("\n\tFrom %s"), *chain->GetFullName());

        baseStr += "\n]";

        return baseStr;
    }
}

void FWfcTilesetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	WorkspaceMenuCategory = tabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_WfcTilesetEditor", "WFC Tileset Editor"));

	//Call a helper function which initializes the UI layout for our new tab spawners.
	FAssetEditorToolkit::RegisterTabSpawners(tabManager);

	//Register the tabs/panes of this editor.
	tabManager->RegisterTabSpawner(WfcTileset_TabID_Properties, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GeneratePropertiesTab))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
	tabManager->RegisterTabSpawner(WfcTileset_TabID_TileSelector, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GenerateTileSelectorTab))
		.SetDisplayName(LOCTEXT("TileSelectorTab", "Tile Selector"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	    .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
    
    //The scene view tab has its own special factory.
    //Probably wouldn't hurt to refactor the other tabs similarly (apart from Properties).
    if (!tileSceneTabFactory.IsValid())
        tileSceneTabFactory = MakeShareable(new FWfcTilesetEditorSceneViewTab(SharedThis(this)));
    tileSceneTabFactory->RegisterTabSpawner(tabManager, nullptr)
        .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}
void FWfcTilesetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	//Invoke base-class logic.
	FAssetEditorToolkit::UnregisterTabSpawners(tabManager);
	
	//Also remove the button to spawn our custom editor.
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_Properties);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_TileSelector);
	tabManager->UnregisterTabSpawner(tileSceneTabFactory->GetIdentifier());
}

TSharedRef<SDockTab> FWfcTilesetEditor::GeneratePropertiesTab(const FSpawnTabArgs& args)
{
	check(args.GetTabId() == WfcTileset_TabID_Properties);

	return SAssignNew(propertiesTab, SDockTab)
		 .Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		 .Label(LOCTEXT("GenericDetailsTitle", "Details"))
		 .TabColorScale(GetTabColorScale())
		 [
			 SNew(SVerticalBox)
			 +SVerticalBox::Slot() .HAlign(HAlign_Left) [
		         detailsView.ToSharedRef()
		     ]
		 ];
}
TSharedRef<SDockTab> FWfcTilesetEditor::GenerateTileSelectorTab(const FSpawnTabArgs& args)
{
	check(args.GetTabId() == WfcTileset_TabID_TileSelector);
    
	return SAssignNew(tileSelectorTab, SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("TileSelectorTabLabel", "Tile Selector"))
		.TabColorScale(GetTabColorScale()) [
		    SNew(STextComboBox)
		        .OptionsSource(&tilesetTileSelectorChoices)
		        .OnSelectionChanged(this, &FWfcTilesetEditor::OnTileSelected)
		];
}

void FWfcTilesetEditor::RefreshTileChoices()
{
    //Update the two "tile selector" arrays.
    tilesetTileSelectorChoices.Empty();
    tilesetTileSelectorChoiceIDs.Empty();
    for (const auto& tileByID : tileset->Tiles)
    {
        auto displayName = tileByID.Value.GetDisplayName(tileByID.Key);
        tilesetTileSelectorChoices.Add(MakeShareable<FString>(new FString(displayName)));
        tilesetTileSelectorChoiceIDs.Add(tileByID.Key);
    }
}
void FWfcTilesetEditor::OnTileSelected(TSharedPtr<FString> name, ESelectInfo::Type)
{
    //For some reason, "null" gets selected sometimes when using the widget.
    if (!name.IsValid())
        return;
    
    //Get which tile this is.
    //Unfortunately, the iterator returned by std::find() doesn't provide the index, so we have to do it manually...
    int foundI;
    for (foundI = 0; foundI < tilesetTileSelectorChoices.Num(); ++foundI)
        if (*tilesetTileSelectorChoices[foundI] == *name)
            break;
    check(foundI < tilesetTileSelectorChoices.Num());
    int tileID = tilesetTileSelectorChoiceIDs[foundI];

    //Update the tile 3D visualization tab.
    tileToVisualize = tileID;    
    tileSceneTabBody->GetViewportClient()->Invalidate();
}

FWfcTilesetEditor::FWfcTilesetEditor() : tileset(nullptr)
{

}
void FWfcTilesetEditor::InitWfcTilesetEditorEditor(const EToolkitMode::Type mode,
                                                   const TSharedPtr<IToolkitHost>& initToolkitHost,
                                                   UWfcTileset* newTileset)
{
    if (!tileSceneTabFactory.IsValid())
        tileSceneTabFactory = MakeShareable(new FWfcTilesetEditorSceneViewTab(SharedThis(this)));
    
	SetAsset(newTileset);
	
	//Retrieve the property editor module and assign properties to the DetailsView.
	auto& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	detailsView = propertyEditorModule.CreateDetailView(FDetailsViewArgs(
		false, true, true,
		FDetailsViewArgs::ObjectsUseNameArea,
		false
	));
    detailsView->OnFinishedChangingProperties().AddRaw(this, &FWfcTilesetEditor::OnTilesetEdited);

	//Create our editor's layout.
	auto standaloneDefaultLayout = FTabManager::NewLayout("Standalone_WfcTilesetEditor_Layout_v1")
    ->AddArea(
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
                    ->SetOrientation(Orient_Horizontal)
                    ->Split(
                        FTabManager::NewStack()
                        ->AddTab(WfcTileset_TabID_SceneView, ETabState::OpenedTab)
                    )
                    ->Split(
                        FTabManager::NewSplitter()
                            ->SetOrientation(Orient_Vertical)
                            ->Split(
                                FTabManager::NewStack()
                                ->AddTab(WfcTileset_TabID_TileSelector, ETabState::OpenedTab)
                            )
                            ->Split(
                                FTabManager::NewStack()
                                ->AddTab(WfcTileset_TabID_Properties, ETabState::OpenedTab)
                            )
                    )
            )
    );

    auto* tilesetBasePtr = static_cast<UObject*>(tileset);
    
	//Start up the editor.
	FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, WfcTilesetEditorAppIdentifier,
									     standaloneDefaultLayout, true, true,
									     tilesetBasePtr);
	if (detailsView.IsValid())
		detailsView->SetObject(tilesetBasePtr);
}

FWfcTilesetEditor::~FWfcTilesetEditor()
{

}

void FWfcTilesetEditor::SetAsset(UWfcTileset* asset)
{
	tileset = asset;
    RefreshTileChoices();
}

TSharedRef<SWidget> FWfcTilesetEditor::SpawnSceneView()
{
    auto ref = SAssignNew(tileSceneTabBody, SWfcTilesetTabBody);
    tileSceneTabBody->GetViewportClient()->OnTick.AddRaw(this, &FWfcTilesetEditor::OnSceneTick);
    return ref;
}

void FWfcTilesetEditor::OnTilesetEdited(const FPropertyChangedEvent& data)
{
    RefreshTileChoices();
    tileSceneTabBody->GetViewportClient()->Invalidate();
}
// ReSharper disable once CppMemberFunctionMayBeConst
void FWfcTilesetEditor::OnSceneTick(float deltaSeconds)
{
    const auto& camPos = tileSceneTabBody->GetViewportClient()->GetViewLocation();
    tileSceneTabBody->GetScene()->Refresh(tileset, tileToVisualize, camPos);
}


FName FWfcTilesetEditor::GetToolkitFName() const { return ToolkitName; }
FText FWfcTilesetEditor::GetBaseToolkitName() const { return LOCTEXT("AppLabel", "WFC Tileset Editor"); }
FText FWfcTilesetEditor::GetToolkitToolTipText() const { return LOCTEXT("ToolTip", "WFC Tileset Editor"); }
FString FWfcTilesetEditor::GetWorldCentricTabPrefix() const { return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString(); }
FLinearColor FWfcTilesetEditor::GetWorldCentricTabColorScale() const { return FColor::Red; }

#undef LOCTEXT_NAMESPACE
