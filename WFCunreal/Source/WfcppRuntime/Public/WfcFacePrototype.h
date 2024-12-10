#pragma once

#include "WfcDataReflection.h"

#include "WfcFacePrototype.generated.h"


//ID's for a tile face, to identify symmetries and match with other faces.
UENUM(BlueprintType)
enum class PointSymmetry : uint8
{
	a = 0,
	b = 1,
	c = 2,
	d = 3
};


//A type of face that tiles can have.
//Defines symmetry and the ability to match with similar faces,
//    by assigning point
USTRUCT(BlueprintType)
struct WFCPPRUNTIME_API FWfcFacePrototype
{
	GENERATED_BODY()
	
	//The ID of the corner on the 'min' side of both face axes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointSymmetry pAA = PointSymmetry::a;
	//The ID of the corner on the 'min' side of the first axis (X or Y),
	//    and the 'max' side of the second axis (Y or Z).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointSymmetry pAB = PointSymmetry::b;
	
	//The ID of the corner on the 'max' side of the first axis (X or Y),
	//    and the 'min' side of the second axis (Y or Z).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointSymmetry pBA = PointSymmetry::c;
	//The ID of the corner on the 'max' side of both face axes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointSymmetry pBB = PointSymmetry::d;

	//Whether this face can match with itself.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FriendWithSelf = true;

	//A display name for human readability.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Nickname = "New Face";

    //TOOD: Add nicknames for each point as well


    //Finds the symmetry of the given corner.
    PointSymmetry GetPointSymmetry(WFC::Tiled3D::FacePoints corner) const { return const_cast<FWfcFacePrototype*>(this)->GetPointSymmetry(corner); }
    //Gets a mutable reference to the symmetry at the given corner.
    PointSymmetry& GetPointSymmetry(WFC::Tiled3D::FacePoints corner)
    {
        switch (corner)
        {
            case WFC::Tiled3D::FacePoints::AA: return pAA;
            case WFC::Tiled3D::FacePoints::AB: return pAB;
            case WFC::Tiled3D::FacePoints::BA: return pBA;
            case WFC::Tiled3D::FacePoints::BB: return pBB;
            default: check(false); return pAA;
        }
    }
};
template<>
struct TStructOpsTypeTraits<FWfcFacePrototype> : public TStructOpsTypeTraitsBase2<FWfcFacePrototype> {
	enum {
		
	};
};

//Face prototypes have a unique ID in a tile-set.
using WfcFacePrototypeID = int32;
constexpr WfcFacePrototypeID INVALID_FACE_ID = 0,
			                 FIRST_VALID_FACE_ID = 1;