#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/AssetEditorToolkit.h"

class UWfcTileset;
class IWfcTilesetEditor;

extern const FName WfcTilesetEditorAppIdentifier;
DECLARE_LOG_CATEGORY_EXTERN(LogWfcppEditor, Log, All);


class IWfcTilesetEditorModule : public IModuleInterface, public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
	virtual TSharedRef<IWfcTilesetEditor> CreateCustomAssetEditor(EToolkitMode::Type mode,
																  const TSharedPtr<IToolkitHost>& initToolkitHost,
																  UWfcTileset* tileset) = 0;
};