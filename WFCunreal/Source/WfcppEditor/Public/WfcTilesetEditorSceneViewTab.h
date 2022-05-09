#pragma once

#include "WorkflowOrientedApp/WorkflowTabFactory.h"

//Based on this wonderful tutorial:
//  https://easycomplex-tech.com/blog/Unreal/AssetEditor/UEAssetEditorDev-AssetEditorPreview/

class FWfcTilesetEditor;

extern const FName WfcTileset_TabID_SceneView;

//Encapsulates the header, and body generation, for the "3D tile visualization" editor tab.
struct FWfcTilesetEditorSceneViewTab : public FWorkflowTabFactory
{
    FWfcTilesetEditorSceneViewTab(TSharedPtr<FWfcTilesetEditor> editor);

    virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& info) const override;
    virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& info) const override;
    virtual FTabSpawnerEntry& RegisterTabSpawner(TSharedRef<FTabManager> tabManager, const FApplicationMode* currentApplicationMode) const override;

protected:
    TWeakPtr<FWfcTilesetEditor> editor;
};