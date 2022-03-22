#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/Interface.h"

#include "WFC_DataReflection.h"

#include "WFC_Ti.generated.h"


//Data about a tile for use in WFC.
UINTERFACE(BlueprintType)
class MYPROJECT_API UWFC_Ti : public UInterface
{
	GENERATED_BODY()
};

//Data about a tile for use in WFC.
class MYPROJECT_API IWFC_Ti
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="WFC", meta=(CompactNodeTitle="Data"))
	FWFC_Cube GetTileData() const;
};
