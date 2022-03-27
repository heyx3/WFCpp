#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "WfcTileset.h"

#include "WfcTilesetFactory.generated.h"


//TODO: Custom thumbnail for the asset. Follow this tutorial: https://www.orfeasel.com/creating-custom-editor-assets/#comment-22758

UCLASS()
class WFCPPEDITOR_API UWfcTilesetFactory : public UFactory
{
    GENERATED_BODY()
public:

    //Define the features of this factory:
    UWfcTilesetFactory()
    {
        bCreateNew = true;
        bEditAfterNew = true;
        SupportedClass = UWfcTileset::StaticClass();
    }
    virtual FString GetDefaultNewAssetName() const override { return "Tileset"; }

    //By default, this type of asset will show up in the "Miscellaneous" category.
    //If you want it to show up in a different category, do this:
    // [in MyProject.Build.cs] PublicDependencyModules.Add("AssetTools");
    // [inside UWfcTilesetFactory class header] virtual uint32 GetMenuCategories() const override;
    // [top of WfcTileset.cpp] #include "AssetTypeCategories.h"
    // [inside WfcTileset.cpp] UWfcTilesetFactory::GetMenuCategories() const { return EAssetTypeCategories::Blueprint; }

    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                      EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};