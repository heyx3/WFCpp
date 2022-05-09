#include "WfcTilesetEditorSceneViewTab.h"

#include "WfcTilesetEditor.h"

#define LOCTEXT_NAMESPACE "WfcTileset_Editor_SceneView"


const FName WfcTileset_TabID_SceneView(TEXT("WfcTileset_Editor_SceneView"));

FWfcTilesetEditorSceneViewTab::FWfcTilesetEditorSceneViewTab(TSharedPtr<FWfcTilesetEditor> editor)
    : FWorkflowTabFactory(WfcTileset_TabID_SceneView, editor),
      editor(editor)
{
    bIsSingleton = false;

    TabLabel = LOCTEXT("SceneViewLabel", "Visualizer");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

    ViewMenuDescription = LOCTEXT("SceneView", "View the Viz");
    ViewMenuTooltip = LOCTEXT("SceneViewTooltip", "Fancy a view menu?");
}

TSharedRef<SWidget> FWfcTilesetEditorSceneViewTab::CreateTabBody(const FWorkflowTabSpawnInfo& info) const
{
    return editor.Pin()->SpawnSceneView();
}
FText FWfcTilesetEditorSceneViewTab::GetTabToolTipText(const FWorkflowTabSpawnInfo& info) const
{
    return LOCTEXT("SceneViewTabTooltip", "Visualize the selected tile");
}
FTabSpawnerEntry& FWfcTilesetEditorSceneViewTab::RegisterTabSpawner(TSharedRef<FTabManager> tabManager,
                                                                    const FApplicationMode* currentApplicationMode) const
{
    return FWorkflowTabFactory::RegisterTabSpawner(tabManager, currentApplicationMode)
               .SetDisplayName(LOCTEXT("SceneTabDisplayName", "Tile Preview"));
}
