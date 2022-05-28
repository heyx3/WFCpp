#pragma once

#include "WfcDataReflection.h"

#include "WfcFacePrototype.generated.h"


//ID's for a tile face, to identify symmetries and match with other faces.
UENUM(BlueprintType)
enum class PointID : uint8
{
	_0 = 0,
	_1 = 1,
	_2 = 2,
	_3 = 3
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
	PointID pAA = PointID::_0;
	//The ID of the corner on the 'min' side of the first axis (X or Y),
	//    and the 'max' side of the second axis (Y or Z).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointID pAB = PointID::_1;
	
	//The ID of the corner on the 'max' side of the first axis (X or Y),
	//    and the 'min' side of the second axis (Y or Z).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointID pBA = PointID::_2;
	//The ID of the corner on the 'max' side of both face axes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PointID pBB = PointID::_3;

	//Whether this face can match with itself.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool FriendWithSelf = true;

	//A display name for GUI purposes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Nickname = "New Face";
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