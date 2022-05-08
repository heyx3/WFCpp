#include "WfcTilesetEditor.h"

#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "PropertyCustomizationHelpers.h"
#include "SAssetDropTarget.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Views/STileView.h"

#include "WfcppEditorModule.h"
#include "Widgets/Layout/SScrollBox.h"
#include "AdvancedPreviewScene.h"
#include "Widgets/Input/STextComboBox.h"


#define LOCTEXT_NAMESPACE "WfcTilesetEditor"

const FName ToolkitName(TEXT("WfcTilesetEditor"));

const FName WfcTileset_TabID_Properties   (TEXT("WfcTilesetEditor_Properties"));
const FName WfcTileset_TabID_TileSelector (TEXT("WfcTilesetEditor_TileSelector"));
const FName WfcTileset_TabID_TileScene    (TEXT("WfcTilesetEditor_TileScene"));


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
    tabManager->RegisterTabSpawner(WfcTileset_TabID_TileScene, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GenerateTileSceneTab))
        .SetDisplayName(LOCTEXT("TileSceneTab", "Tile Preview"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}
void FWfcTilesetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	//Invoke base-class logic.
	FAssetEditorToolkit::UnregisterTabSpawners(tabManager);
	
	//Also remove the button to spawn our custom editor.
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_Properties);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_TileSelector);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_TileScene);
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
		        .OnComboBoxOpening(this, &FWfcTilesetEditor::RefreshTileChoices)
		        .OnSelectionChanged(this, &FWfcTilesetEditor::OnTileSelected)
		    // SNew(SScrollBox)
      //           .Orientation(EOrientation::Orient_Vertical)
      //           +SScrollBox::Slot().VAlign(VAlign_Fill) [
      //               SAssignNew(tileSelector, STileView<TSharedPtr<int>>)
      //                   .Orientation(Orient_Vertical)
      //                   .ListItemsSource(&tilesetTileIDs)
      //                   .ItemWidth(200).ItemHeight(200)
      //                   .OnGenerateTile(STileView<TSharedPtr<int>>
      //                                   ::FOnGenerateRow
      //                                   ::CreateSP(this, &FWfcTilesetEditor::GenerateTileSelectorTile))
      //               ]
		];
}
TSharedRef<SDockTab> FWfcTilesetEditor::GenerateTileSceneTab(const FSpawnTabArgs& args)
{
	check(args.GetTabId() == WfcTileset_TabID_TileScene);

	return SAssignNew(tileSelectorTab, SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("TileViewerTabLabel", "Tile Viewer"))
		.TabColorScale(GetTabColorScale()) [
			SNew(SVerticalBox)
				+SVerticalBox::Slot() [
					SNew(STextBlock)
						.Text(FText::FromString("//TODO: 3D viewer"))
				]
				//+SVerticalBox::Slot() [ tileSceneController->StartFrame() ]
		];
}

/*
TSharedRef<SWidget> FWfcTilesetEditor::MakeTileDataPicker(int tileID)
{
    auto* me = this;
    return SNew(SObjectPropertyEntryBox)
        .DisplayThumbnail(true)
        .DisplayBrowse(true)
        .DisplayUseSelected(true)
        .ObjectPath_Lambda([tileID, me]() {
            return me->tileset->Tiles[tileID].Data->GetPathName();
        })
        .OnObjectChanged_Lambda([tileID, me](const FAssetData& data) {
            me->tileset->Tiles[tileID].Data = data.GetAsset();
        });
}
*/

void FWfcTilesetEditor::RefreshTileChoices()
{
    //Update the two "tile selector" arrays.
    tilesetTileSelectorChoices.Empty();
    tilesetTileSelectorChoiceIDs.Empty();
    for (const auto& tileByID : tileset->Tiles)
    {
        auto displayName = tileByID.Value.GetDisplayName(tileByID.Key);
        tilesetTileSelectorChoices.Add(MakeShareable<FString>(new FString(displayName)));
    }
}
void FWfcTilesetEditor::OnTileSelected(TSharedPtr<FString> name, ESelectInfo::Type)
{
    //Get which tile this is.
    //Unfortunately, the iterator returned by std::find() doesn't provide the index, so we have to do it manually...
    int foundI;
    for (foundI = 0; foundI < tilesetTileSelectorChoices.Num(); ++foundI)
        if (tilesetTileSelectorChoices[foundI] == name)
            break;
    check(foundI < tilesetTileSelectorChoices.Num());
    int tileID = tilesetTileSelectorChoiceIDs[foundI];

    //Clean up the previous tile visualization, if one exists.
    if (tileVizActor != nullptr)
    {
        TileViz_Destroy(tileID, tileVizActor);
        tileVizActor->DestroyComponent();
    }

    //Create a new visualization of the selected tile.
    tileVizActor = TileViz_Create(tileID);
    tileScene->AddComponent(tileVizActor, FTransform::Identity);
}


void FWfcTilesetEditor::InitWfcTilesetEditorEditor(const EToolkitMode::Type mode,
                                                   const TSharedPtr<IToolkitHost>& initToolkitHost,
                                                   UWfcTileset* newTileset)
{
    tileScene = MakeShared<FAdvancedPreviewScene>(FPreviewScene::ConstructionValues()
        .SetEditor(true) //TODO: What does this actually do?
    );
    tileSceneController = tileScene->GetScene();
    
	SetAsset(newTileset);
	
	//Retrieve the property editor module and assign properties to the DetailsView.
	auto& propertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	detailsView = propertyEditorModule.CreateDetailView(FDetailsViewArgs(
		false, true, true,
		FDetailsViewArgs::ObjectsUseNameArea,
		false
	));
    //TODO: Callback for when tile-collection or face-prototype-collection changes 

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
                        ->AddTab(WfcTileset_TabID_TileScene, ETabState::OpenedTab)
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
	//TODO: why do we manually release shared pointers in the destructor? Can't we let them clean themselves up?
	detailsView.Reset();
}

void FWfcTilesetEditor::SetAsset(UWfcTileset* asset)
{
	tileset = asset;

    for (const auto& tileKVP : asset->Tiles)
    {
        auto label = tileKVP.Value.GetDisplayName(tileKVP.Key);
        tilesetTileSelectorChoices.Add(MakeShareable(new FString(label)));
        tilesetTileSelectorChoiceIDs.Add(tileKVP.Key);
    }
}

TSharedRef<SWidget> FWfcTilesetEditor::TileViz_MakePreview(int tileID)
{
    auto displayName = tileset->Tiles[tileID].GetDisplayName(tileID);
    return SNew(STextBlock)
        .Text(FText::FromString(displayName));
}

FName FWfcTilesetEditor::GetToolkitFName() const { return ToolkitName; }
FText FWfcTilesetEditor::GetBaseToolkitName() const { return LOCTEXT("AppLabel", "WFC Tileset Editor"); }
FText FWfcTilesetEditor::GetToolkitToolTipText() const { return LOCTEXT("ToolTip", "WFC Tileset Editor"); }
FString FWfcTilesetEditor::GetWorldCentricTabPrefix() const { return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString(); }
FLinearColor FWfcTilesetEditor::GetWorldCentricTabColorScale() const { return FColor::Red; }

UActorComponent* FWfcTilesetEditor_Actors::TileViz_Create(int tileID)
{
    auto* component = NewObject<UChildActorComponent>();
    TSubclassOf<AActor> actorClass = Cast<UClass>(GetAsset()->Tiles[tileID].Data);

    component->SetChildActorClass(actorClass);

    return component;
}

#undef LOCTEXT_NAMESPACE
