#pragma once

#include "CoreMinimal.h"
#include "Containers/StaticArray.h"

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

    //Maps the symmetry points of this face to the original prototype.
    //For example, "Rot90Clockwise" means to rotate this tile face's corners 90 degrees clockwise
    //    to get the corresponding corners on the prototype.
    //The rotation is in the space defined by 'FacePoints', "AA, AB, BA, BB" --
    //    imagine "BA" is {1, 0} and "AB" is {0, 1}.
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
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Faces")
	FWfcTileFace MinX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Faces")
	FWfcTileFace MinY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Faces")
	FWfcTileFace MinZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Faces")
	FWfcTileFace MaxX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Faces")
	FWfcTileFace MaxY;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Faces")
	FWfcTileFace MaxZ;

	//Higher values make this tile more popular during generation.
    //The value will be casted into uint32.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=0, ClampMax=4294967295))
	int WeightU32 = 100;

	//User data associated with this tile (e.x. an Actor to spawn in its place).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObject* Data;

    
    //Whether to include inverted versions of all supported rotations,
    //    NOT including the explicit ones in "SpecificSupportedTransforms".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseInvertedTransforms = false;
    
    //Whether to include all rotations of this tile.
    //If both this and "UseInvertedTransforms" are true, then
    //    *all* possible transformations of this tile will be used.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseAllRotations = false;
    
    //Whether to include all rotations of this tile around the X axis.
    //Superceded by "UseAllRotations".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseXAxisRotations = false;
    //Whether to include all rotations of this tile around the Y axis.
    //Superceded by "UseAllRotations".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseYAxisRotations = false;
    //Whether to include all rotations of this tile around the Z axis.
    //Superceded by "UseAllRotations".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseZAxisRotations = false;

    //Whether to include all rotations of this tile around a pair of opposite edges.
    //Superceded by "UseAllRotations".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseEdgeRotations = false;
    //Whether to include all rotations of this tile around a pair of opposite corners.
    //Superceded by "UseAllRotations".
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms")
    bool UseCornerRotations = false;
    
    //Specific transformations to support on this tile.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transforms", meta=(TitleProperty="Rot"))
    TArray<FWFC_Transform3D> SpecificSupportedTransforms;

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

    //Writes into the given Set all the transforms that should be done on this tile when using the tileset.
    //Includes the "identity" transform.
    //Not thread-safe!
    void GetSupportedTransforms(TSet<FWFC_Transform3D>& output) const;

private:
    //Used in GetSupportedTransforms().
    mutable TArray<WFC_Rotations3D> buffer_usedRotations;
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