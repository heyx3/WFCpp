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
        tilesetTileSelectorChoiceIDs.Add(tileByID.Key);
    }
}
void FWfcTilesetEditor::OnTileSelected(TSharedPtr<FString> name, ESelectInfo::Type)
{
    //TODO: See the tail end of SMaterialEditor3DPreviewViewport::Construct(), which adds a callback for when an asset's properties change!
    
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
    auto tileSceneData = tileSceneTabBody->GetScene();
    if (tileSceneData->GetTileViz() != nullptr)
        TileViz_Destroy(tileID, tileSceneData->GetTileViz());
    tileSceneData->SetTile(tileID, TileViz_Create(tileID));
    tileSceneTabBody->GetViewportClient()->Invalidate();
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

    for (const auto& tileKVP : asset->Tiles)
    {
        auto label = tileKVP.Value.GetDisplayName(tileKVP.Key);
        tilesetTileSelectorChoices.Add(MakeShareable(new FString(label)));
        tilesetTileSelectorChoiceIDs.Add(tileKVP.Key);
    }

    //Note that if the tile scene tab is not created yet, "SetTileset()" will be called on tab creation.
    if (tileSceneTabBody.IsValid())
        tileSceneTabBody->GetScene()->SetTileset(tileset);
}

TSharedRef<SWidget> FWfcTilesetEditor::SpawnSceneView()
{
    auto ref = SAssignNew(tileSceneTabBody, SWfcTilesetTabBody);

    tileSceneTabBody->GetScene()->SetTileset(tileset);

    return ref;
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
    auto* tileData = GetAsset()->Tiles[tileID].Data;
    
    if (tileData->IsA<UClass>())
    {
        auto dataClass = Cast<UClass>(tileData);
        if (dataClass->IsChildOf<AActor>())
        {
            UE_LOG(LogWfcppEditor, Error,
                   TEXT("Unable to spawn an Actor in the preview scene, because the editor crashes when it's destroyed."));
            return nullptr;
        }
        else
        {
            UE_LOG(LogWfcppEditor, Fatal, TEXT("Unknown UObject type referenced: %s"), *dataClass->GetFullName());
            return nullptr;
        }
    }
    
    if (tileData->IsA<UBlueprint>())
    {
        UE_LOG(LogWfcppEditor, Error,
               TEXT("Unable to spawn an Actor in the preview scene, because the editor crashes when it's destroyed."));
        return nullptr;
    }
    
    if (tileData->IsA<UStaticMesh>())
    {
        auto* component = NewObject<UStaticMeshComponent>(GetTransientPackage(), NAME_None, RF_Transient);
        component->SetStaticMesh(Cast<UStaticMesh>(tileData));
        return component;
    }
    else
    {
        UE_LOG(LogWfcppEditor, Fatal, TEXT("Unknown UObject data referenced: %s"), *tileData->GetFullName());
        return nullptr;
    }
}

#undef LOCTEXT_NAMESPACE
