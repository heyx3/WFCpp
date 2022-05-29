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


    //Transforms a point on this face into its corresponding point in the face prototype,
    //    based on "PrototypeOrientation".
    WFC::Tiled3D::FacePoints GetPrototypeCorner(WFC::Tiled3D::FacePoints thisCorner) const;
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


    FString GetDisplayName() const { return (Data == nullptr) ? "null" : Data->GetName(); }
    FString GetDisplayName(int myID) const { return FString::FromInt(myID) + ": " +
                                                    ((Data == nullptr) ? "null" : Data->GetName()); }

    const FWfcTileFace& GetFace(WFC::Tiled3D::Directions3D dir) const
    {
        switch (dir)
        {
            case WFC::Tiled3D::Directions3D::MinX: return MinX;
            case WFC::Tiled3D::Directions3D::MaxX: return MaxX;
            case WFC::Tiled3D::Directions3D::MinY: return MinY;
            case WFC::Tiled3D::Directions3D::MaxY: return MaxY;
            case WFC::Tiled3D::Directions3D::MinZ: return MinZ;
            case WFC::Tiled3D::Directions3D::MaxZ: return MaxZ;
            default: check(false); return MinX;
        }
    }
    FWfcTileFace& GetFace(WFC::Tiled3D::Directions3D dir) { return const_cast<FWfcTileFace&>(const_cast<const FWfcTile*>(this)->GetFace(dir)); }
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