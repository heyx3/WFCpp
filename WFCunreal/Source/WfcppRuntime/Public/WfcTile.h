#pragma once

#include "WfcFacePrototype.h"
#include "WfcDataReflection.h"
#include "WfcTile.generated.h"


//A specific tile's face.
USTRUCT(BlueprintType)
struct WFCPPRUNTIME_API FWfcTileFace
{
	GENERATED_BODY()
	
	//The ID of the 'WfcFace' this face takes after.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PrototypeID;
	//Maps the symmetry points of the prototype onto this specific face.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	WFC_Transforms2D PrototypeOrientation;
};
template<>
struct TStructOpsTypeTraits<FWfcTileFace> : public TStructOpsTypeTraitsBase2<FWfcTileFace>
{
	enum
	{
		WithZeroConstructor = true,
		WithNoDestructor = true
	};
};


//A specific tile.
USTRUCT(BlueprintType)
struct WFCPPRUNTIME_API FWfcTile
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWfcTileFace MinX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWfcTileFace MinY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWfcTileFace MinZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWfcTileFace MaxX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWfcTileFace MaxY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWfcTileFace MaxZ;

	//Higher values make this tile more popular during generation.
	//Should fit into a uint32.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int WeightU32;

	//User data associated with this tile (e.x. an Actor to spawn in its place).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject* Data;
};
template<>
struct TStructOpsTypeTraits<FWfcTile> : public TStructOpsTypeTraitsBase2<FWfcTile>
{
	enum
	{
		WithZeroConstructor = true,
		WithNoDestructor = true
	};
};