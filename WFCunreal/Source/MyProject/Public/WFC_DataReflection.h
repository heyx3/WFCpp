#pragma once

#include "Transform3D.h"

#include "WFC_DataReflection.generated.h"


//Unfortunately, for structs and enums to show up properly in Unreal,
//    they need the reflection macros attached to them.
//So I have to redefine some WFC stuff here, using Unreal's reflection macros.

//The six cube faces
UENUM(Category=WFC, BlueprintType)
enum class WFC_Directions3D : uint8
{
	MinX, MaxX,
	MinY, MaxY,
	MinZ, MaxZ
};


//Provides a unique ID for each corner of a cube face.
//These ID's describe both the symmetry of the face,
//    and its ability to match up with another face.
//The faces are ordered based on the two world-space axes along that face.
// The first letter describes the first axis (X or Y),
//     and the second describes the second axis (Y or Z).
// An 'A' means the min side of that axis, while 'B' means the max side.
USTRUCT(BlueprintType)
struct FWFC_Face
{
	GENERATED_BODY()

public:
	//The corner on the 'min' side of both face axes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AA;
	//The corner on the 'min' side of the first axis (X or Y),
	//    and the 'max' side of the second axis (Y or Z).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AB;
	
	//The corner on the 'max' side of the first axis (X or Y),
	//    and the 'min' side of the second axis (Y or Z).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BA;
	//The corner on the 'max' side of both face axes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int BB;
};

//Symmetry/face-matching data for all 6 faces of a tile.
USTRUCT(BlueprintType)
struct FWFC_Cube
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFC_Face MinX = { 1, 2, 3, 4 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFC_Face MaxX = { 5, 6, 7, 8 };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFC_Face MinY = { 9, 10, 11, 12 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFC_Face MaxY = { 13, 14, 15, 16 };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFC_Face MinZ = { 17, 18, 19, 20 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWFC_Face MaxZ = { 21, 22, 23, 24 };

	
	//Converts this Unreal-friendly data into the internal WFC data.
	WFC::Tiled3D::CubePermutation ToInternal() const;
};