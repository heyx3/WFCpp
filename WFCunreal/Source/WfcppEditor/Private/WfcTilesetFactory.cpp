#include "WfcTilesetFactory.h"

UObject* UWfcTilesetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent,
											  FName InName, EObjectFlags Flags,
											  UObject* Context, FFeedbackContext* Warn)
{
	auto* tileset = NewObject<UWfcTileset>(InParent, InClass, InName, Flags);
	return tileset;
}
