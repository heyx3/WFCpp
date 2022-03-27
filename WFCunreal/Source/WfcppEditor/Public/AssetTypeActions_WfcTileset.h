#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"


class FAssetTypeActions_WfcTileset : public FAssetTypeActions_Base
{
public:

    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual void OpenAssetEditor(const TArray<UObject*>& objects, TSharedPtr<IToolkitHost> editWithinLevelEditor) override;
    virtual uint32 GetCategories() override;
};