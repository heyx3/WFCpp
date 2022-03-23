#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "WFC_DataReflection.h"

#include "BP_Utils.generated.h"

UCLASS()
class MYPROJECT_API UBP_Utils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="=="))
	static bool FaceEquals(const FWFC_Face& a, const FWFC_Face& b) { return a == b; }
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="!="))
	static bool FaceNotEquals(const FWFC_Face& a, const FWFC_Face& b) { return !FaceEquals(a, b); }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="=="))
	static bool CubeEquals(const FWFC_Cube& a, const FWFC_Cube& b) { return a == b; }
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="!="))
	static bool CubeNotEquals(const FWFC_Cube& a, const FWFC_Cube& b) { return !CubeEquals(a, b); }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="=="))
	static bool TransfEquals(const FWFC_Transform3D& a, const FWFC_Transform3D& b) { return a == b; }
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(CompactNodeTitle="!="))
	static bool TransfNotEquals(const FWFC_Transform3D& a, const FWFC_Transform3D& b) { return !TransfEquals(a, b); }
};
