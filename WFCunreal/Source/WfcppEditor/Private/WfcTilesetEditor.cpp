#include "WfcTilesetEditor.h"

#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Docking/SDockTabStack.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"

#include "WfcppEditorModule.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Views/STileView.h"


#define LOCTEXT_NAMESPACE "WfcTilesetEditor"

const FName ToolkitName(TEXT("WfcTilesetEditor"));

const FName WfcTileset_TabID_Properties   (TEXT("WfcTilesetEditor_Properties"));
const FName WfcTileset_TabID_FaceSelector (TEXT("WfcTilesetEditor_FaceSelector"));
const FName WfcTileset_TabID_Experiments  (TEXT("WfcTilesetEditor_Experiments"));
const FName WfcTileset_TabID_TileSelector (TEXT("WfcTilesetEditor_TileSelector"));
const FName WfcTileset_TabID_TileViewer   (TEXT("WfcTilesetEditor_TileViewer"));


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
	tabManager->RegisterTabSpawner(WfcTileset_TabID_Experiments, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GenerateExperimentsTab))
		.SetDisplayName(LOCTEXT("Experiments1Tab", "Experiments2"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
	tabManager->RegisterTabSpawner(WfcTileset_TabID_FaceSelector, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GenerateFaceSelectorTab))
		.SetDisplayName(LOCTEXT("FaceSelectorTab", "Face Prototypes"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	    .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
	tabManager->RegisterTabSpawner(WfcTileset_TabID_TileSelector, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GenerateTileSelectorTab))
		.SetDisplayName(LOCTEXT("TileSelectorTab", "Tile Selector"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	    .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
    tabManager->RegisterTabSpawner(WfcTileset_TabID_TileViewer, FOnSpawnTab::CreateSP(this, &FWfcTilesetEditor::GenerateTileViewerTab))
        .SetDisplayName(LOCTEXT("TileViewerTab", "Tile Viewer"))
        .SetGroup(WorkspaceMenuCategory.ToSharedRef())
        .SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}
void FWfcTilesetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& tabManager)
{
	//Invoke base-class logic.
	FAssetEditorToolkit::UnregisterTabSpawners(tabManager);
	
	//Also remove the button to spawn our custom editor.
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_Properties);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_FaceSelector);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_Experiments);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_TileSelector);
	tabManager->UnregisterTabSpawner(WfcTileset_TabID_TileViewer);
}

TSharedRef<SDockTab> FWfcTilesetEditor::GeneratePropertiesTab(const FSpawnTabArgs& args)
{
	check(args.GetTabId() == WfcTileset_TabID_Properties);

	auto output = SAssignNew(propertiesTab, SDockTab)
			 .Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
			 .Label(LOCTEXT("GenericDetailsTitle", "Details"))
			 .TabColorScale(GetTabColorScale())
			 [
			 	 SNew(SVerticalBox)
			 	 +SVerticalBox::Slot() .HAlign(HAlign_Left) [
			         detailsView.ToSharedRef()
			     ]
			     +SVerticalBox::Slot() .HAlign(HAlign_Right) [
				     SNew(SButton)
				         .Text(LOCTEXT("Experiments7", "Experiments8"))
			  		     .OnClicked(FOnClicked::CreateLambda([]() {
					         UE_LOG(LogWfcppEditor, Warning, TEXT("Clicked the button! Experiments9"))
					   	     return FReply::Handled();
					     }))
				 ]
			 ];
			 
	// auto& box = static_cast<const SVerticalBox&>(*output->GetContent());
	// UE_LOG(LogWfcppEditor, Warning, TEXT("Found %i slots"), box.NumSlots());
	
	return output;
}
// ReSharper disable once CppMemberFunctionMayBeConst
TSharedRef<SDockTab> FWfcTilesetEditor::GenerateExperimentsTab(const FSpawnTabArgs& args)
{
	check(args.GetTabId() == WfcTileset_TabID_Experiments);

	return SNew(SDockTab)
	    .Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
	    .Label(LOCTEXT("Experiments3Title", "Experiments4"))
	    .TabColorScale(GetTabColorScale())
	    [
		    SNew(SVerticalBox)
		    +SVerticalBox::Slot() .HAlign(HAlign_Left) [
			    SNew(SButton)
				    .Text(LOCTEXT("Experiments5", "Experiments6"))
				    .OnClicked(FOnClicked::CreateLambda([]() {
					    UE_LOG(LogWfcppEditor, Warning, TEXT("Clicked the button! Experiments6"))
					    return FReply::Handled();
				    }))
	        ]
	    ];
}
TSharedRef<SDockTab> FWfcTilesetEditor::GenerateFaceSelectorTab(const FSpawnTabArgs& args)
{
	check(args.GetTabId() == WfcTileset_TabID_FaceSelector);

	return SAssignNew(faceSelectorTab, SDockTab)
		.Icon(FEditorStyle::GetBrush("GenericEditor.Tabs.Properties"))
		.Label(LOCTEXT("FaceSelectorTabLabel", "Face Selector"))
		.TabColorScale(GetTabColorScale()) [
			SNew(SVerticalBox)
				+SVerticalBox::Slot()
					.MaxHeight(30.0f)
					.Padding(FMargin(10.0f, 10.0f))
				[
					SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
							.MaxWidth(30.0f)
						[
							SNew(SButton)
								.Text(FText::FromString("+"))
								.OnClicked(FOnClicked::CreateSP(this, &FWfcTilesetEditor::GenerateFacePrototypeData))
						]
				]
				+SVerticalBox::Slot() [
					SAssignNew(faceSelector, SListView<TSharedPtr<int>>)
						.Orientation(Orient_Vertical)
						.ItemHeight(24)
						.ListItemsSource(&tilesetBuf_PrototypeIDs)
						.OnGenerateRow(SListView<TSharedPtr<int>>
									       ::FOnGenerateRow
										   ::CreateSP(this, &FWfcTilesetEditor::GenerateFacePrototypeUI))
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
            SNew(SVerticalBox)
                +SVerticalBox::Slot()
                    .MaxHeight(30.0f)
                    .Padding(FMargin(10.0f, 10.0f))
                [
                    SNew(SHorizontalBox)
                        +SHorizontalBox::Slot()
                            .MaxWidth(30.0f)
                        [
                            SNew(SButton)
                                .Text(FText::FromString("+"))
                                .OnClicked(FOnClicked::CreateSP(this, &FWfcTilesetEditor::GenerateTileData))
                        ]
                ]
                +SVerticalBox::Slot() [
                    SAssignNew(tileSelector, SListView<TSharedPtr<int>>)
                        .Orientation(Orient_Vertical)
                        .ItemHeight(24)
                        .ListItemsSource(&tilesetBuf_TileIDs)
                        .OnGenerateRow(SListView<TSharedPtr<int>>
                                           ::FOnGenerateRow
                                           ::CreateSP(this, &FWfcTilesetEditor::GenerateFacePrototypeUI))
                ]
        ];
}

FReply FWfcTilesetEditor::GenerateFacePrototypeData()
{
	//Pick a new unique ID.
	WfcFaceID maxExistingID = 0;
	for (const auto& kvp : tileset->FacePrototypes)
		maxExistingID = std::max(kvp.Get<0>(), maxExistingID);
	auto newID = maxExistingID + 1;
	check(newID > 0); //Watch for overflow
	
	tileset->FacePrototypes.Add(newID, FWfcFacePrototype());
	tilesetBuf_PrototypeIDs.Add(MakeShareable(new int(newID)));
	this->faceSelector->RequestListRefresh();
	
	return FReply::Handled();
}
FReply FWfcTilesetEditor::RemoveFacePrototypeData(int ID)
{
	tileset->FacePrototypes.Remove(ID);
	tilesetBuf_PrototypeIDs.RemoveAll([ID](TSharedPtr<int> ID2) { return ID == *ID2; });
	this->faceSelector->RequestListRefresh();
	
	return FReply::Handled();
}
TSharedRef<ITableRow> FWfcTilesetEditor::GenerateFacePrototypeUI(TSharedPtr<int> item,
																 const TSharedRef<STableViewBase>& owner)
{
	const auto* ownerPtr = &owner;
	auto ID = *item;
	auto* me = this;

    //Apparently can't make this a lambda, because it invokes the copy constructor.
    #define MAKE_POINT_EDITOR(pointField, fieldStr) \
        SHorizontalBox::Slot() \
            .FillWidth(0.25f) \
        [ \
            SNew(SNumericEntryBox<int>) \
                .Label() [ \
                    SNew(STextBlock) \
                        .Text(FText::FromString(fieldStr)) \
                ] \
                .AllowSpin(true) \
                .MinValue(0) \
                .MinSliderValue(0) \
                .MaxValue(3) \
                .MaxSliderValue(3) \
                .Value_Lambda([me, ID]() -> TOptional<int> { \
                    if (me->tileset->FacePrototypes.Contains(ID)) { \
                        const auto& data = me->tileset->FacePrototypes[ID]; \
                        return static_cast<int>(data.p##pointField); \
                    } \
                    else \
                    { \
                        return TOptional<int>(); \
                    } \
                }) \
                .OnValueCommitted_Lambda([me, ID](int value, ETextCommit::Type type) { \
                    if (type != ETextCommit::OnCleared && me->tileset->FacePrototypes.Contains(ID)) { \
                        auto& data = me->tileset->FacePrototypes[ID]; \
                        data.p##pointField = static_cast<PointID>(value); \
                    } \
                }) \
        ]
    
	return SNew(STableRow<TSharedPtr<int>>, owner)
	   .Padding(2.0f) [
			SNew(SHorizontalBox)
				// ID label:
			   	+SHorizontalBox::Slot()
			   		.FillWidth(0.1f)
			   	[
			   		SNew(STextBlock)
			   			.Text(FText::FromString(FString::Printf(TEXT("%i"), ID)))
			   	]
			   	// Nickname textbox:
			   	+SHorizontalBox::Slot()
			   		.FillWidth(0.2f)
			   	[
			   		SNew(SEditableText)
			   			.Text_Lambda([me, ID]() {
			   				if (me->tileset->FacePrototypes.Contains(ID))
			   					return FText::FromString(me->tileset->FacePrototypes[ID].Nickname);
			   				else
			   					return FText::FromString("[Invalid ID]");
			   			})
			   			.OnTextCommitted_Lambda([me, ID](const FText& text, auto type) {
			   				if (type != ETextCommit::Type::OnCleared) {
			   					me->tileset->FacePrototypes[ID].Nickname = text.ToString();
			   				}
			   			})
			   	]
			   	//Spacer:
			   	+SHorizontalBox::Slot().FillWidth(0.25)
			   	+MAKE_POINT_EDITOR(AA, "MinU/MinV")
			   	+SHorizontalBox::Slot().FillWidth(0.05)
			   	+MAKE_POINT_EDITOR(AB, "MinU/MaxV")
			   	+SHorizontalBox::Slot().FillWidth(0.05)
			   	+MAKE_POINT_EDITOR(BA, "MaxU/MinV")
			   	+SHorizontalBox::Slot().FillWidth(0.05)
			   	+MAKE_POINT_EDITOR(BB, "MaxU/MaxV")
			   	//Spacer:
			   	+SHorizontalBox::Slot().FillWidth(0.25)
			   	//Delete button:
			   	+SHorizontalBox::Slot()
			   		.MaxWidth(30.0f)
			   	[
			   		SNew(SButton)
			   			.Text(FText::FromString("X"))
			   			.OnClicked_Lambda([me, ID]() { return me->RemoveFacePrototypeData(ID); })
			   	]
	   ];

    #undef MAKE_POINT_EDITOR
}

FReply FWfcTilesetEditor::GenerateTileData()
{
	//Pick a new unique ID.
	WfcFaceID maxExistingID = 0;
	for (const auto& kvp : tileset->Tiles)
		maxExistingID = std::max(kvp.Get<0>(), maxExistingID);
	auto newID = maxExistingID + 1;
	check(newID > 0); //Watch for overflow

    //Pick an arbitrary initial face-prototype ID to reference.
    auto facePrototypeID = (tileset->FacePrototypes.Num() > 0) ?
                               tileset->FacePrototypes.begin()->Key :
                               0;
	
    auto faceData = FWfcTileFace{facePrototypeID, WFC_Transforms2D::None};
    auto tileData = FWfcTile{faceData, faceData, faceData,
                             faceData, faceData, faceData,
                             100, nullptr};
    
	tileset->Tiles.Add(newID, tileData);
	tilesetBuf_PrototypeIDs.Add(MakeShareable(new int(newID)));
	this->tileSelector->RequestListRefresh();
	
	return FReply::Handled();
}
FReply FWfcTilesetEditor::RemoveTileData(int ID)
{
	tileset->Tiles.Remove(ID);
	tilesetBuf_TileIDs.RemoveAll([ID](TSharedPtr<int> ID2) { return ID == *ID2; });
	this->tileSelector->RequestListRefresh();
	
	return FReply::Handled();
}
TSharedRef<ITableRow> FWfcTilesetEditor::GenerateTileUI(TSharedPtr<int> item,
														const TSharedRef<STableViewBase>& owner)
{
	const auto* ownerPtr = &owner;
	auto ID = *item;
	auto* me = this;
    
	return SNew(STableRow<TSharedPtr<int>>, owner)
	   .Padding(2.0f) [
			SNew(SHorizontalBox)
				// ID label:
			   	+SHorizontalBox::Slot()
			   		.FillWidth(0.1f)
			   	[
			   		SNew(STextBlock)
			   			.Text(FText::FromString(FString::Printf(TEXT("%i"), ID)))
			   	]
			  
			   	//Spacer:
			   	+SHorizontalBox::Slot().FillWidth(0.25)
			   	//Delete button:
			   	+SHorizontalBox::Slot()
			   		.MaxWidth(30.0f)
			   	[
			   		SNew(SButton)
			   			.Text(FText::FromString("X"))
			   			.OnClicked_Lambda([me, ID]() { return me->RemoveTileData(ID); })
			   	]
	   ];

    #undef MAKE_POINT_EDITOR
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
				->SetOrientation(Orient_Horizontal)
				->Split(
					FTabManager::NewStack()
					->AddTab(WfcTileset_TabID_Properties, ETabState::OpenedTab)
					->AddTab(WfcTileset_TabID_Experiments, ETabState::ClosedTab)
				)
			)
			->Split(
			    FTabManager::NewSplitter()
			    ->SetOrientation(Orient_Horizontal)
			    ->Split(
			        FTabManager::NewStack()
			        ->AddTab(WfcTileset_TabID_TileViewer, ETabState::OpenedTab)
			        ->AddTab(WfcTileset_TabID_TileSelector, ETabState::OpenedTab)
			    )
			)
	);

	//Start up the editor.
	FAssetEditorToolkit::InitAssetEditor(mode, initToolkitHost, WfcTilesetEditorAppIdentifier,
									     standaloneDefaultLayout, true, true,
									     (UObject*)tileset);
	if (detailsView.IsValid())
		detailsView->SetObject((UObject*)tileset);

	TabManager->TryInvokeTab(WfcTileset_TabID_Experiments);
}

FWfcTilesetEditor::~FWfcTilesetEditor()
{
	//TODO: why do we manually release shared pointers in the destructor? Can't we let them clean themselves up?
	detailsView.Reset();
}

void FWfcTilesetEditor::SetAsset(UWfcTileset* asset)
{
	tileset = asset;

    //Generate UI data for face prototypes.
	tilesetBuf_PrototypeIDs.Empty();
	for (const auto& facePrototype : tileset->FacePrototypes)
		tilesetBuf_PrototypeIDs.Add(MakeShareable(new int(facePrototype.Get<0>())));
	if (faceSelector.IsValid())
		faceSelector->RebuildList();
    
    //Generate UI data for tiles.
    tilesetBuf_TileIDs.Empty();
    for (const auto& tiles : tileset->Tiles)
        tilesetBuf_TileIDs.Add(MakeShareable(new int(tiles.Get<0>())));
    if (tileSelector.IsValid())
        tileSelector->RebuildList();
}


FName FWfcTilesetEditor::GetToolkitFName() const { return ToolkitName; }
FText FWfcTilesetEditor::GetBaseToolkitName() const { return LOCTEXT("AppLabel", "WFC Tileset Editor"); }
FText FWfcTilesetEditor::GetToolkitToolTipText() const { return LOCTEXT("ToolTip", "WFC Tileset Editor"); }
FString FWfcTilesetEditor::GetWorldCentricTabPrefix() const { return LOCTEXT("WorldCentricTabPrefix", "AnimationDatabase ").ToString(); }
FLinearColor FWfcTilesetEditor::GetWorldCentricTabColorScale() const { return FColor::Red; }


#undef LOCTEXT_NAMESPACE