#pragma once

#include "Vector2i.h"
#include "Tiled3D/Transform3D.h"

#include "WfcDataReflection.generated.h"


//Unfortunately, for structs and enums to show up properly in Unreal,
//    they need the reflection macros attached to them.
//So I have to redefine some WFC stuff here, using Unreal's reflection macros.

UENUM(Category=WFC, BlueprintType)
enum class WFC_Directions3D : uint8
{
	MinX = 0, // WFC::Tiled3D::Directions3D::MinX
	MaxX = WFC::Tiled3D::Directions3D::MaxX,
	MinY = WFC::Tiled3D::Directions3D::MinY,
	MaxY = WFC::Tiled3D::Directions3D::MaxY,
	MinZ = WFC::Tiled3D::Directions3D::MinZ,
	MaxZ = WFC::Tiled3D::Directions3D::MaxZ
};
static_assert(static_cast<int>(WFC_Directions3D::MinX) == WFC::Tiled3D::Directions3D::MinX,
			  "Needed to satisfy the compiler");

UENUM(Category=WFC, BlueprintType)
enum class WFC_Transforms2D : uint8
{
	None = 0, //WFC::Transformations::None

	Rotate90CW = WFC::Transformations::Rotate90CW,
	Rotate180 = WFC::Transformations::Rotate180,
	Rotate270CW = WFC::Transformations::Rotate270CW,

	FlipX = WFC::Transformations::FlipX,
	FlipY = WFC::Transformations::FlipY,

	//Mirror along the primary diagonal, going from the min corner to the max corner.
	FlipDiag1 = WFC::Transformations::FlipDiag1,
	//Mirror along the secondary diagonal.
	FlipDiag2 = WFC::Transformations::FlipDiag2
};
static_assert(static_cast<int>(WFC_Transforms2D::None) == WFC::Transformations::None,
			  "Needed to satisfy the compiler");


UENUM(Category=WFC, BlueprintType)
enum class WFC_Rotations3D : uint8
{
	//0- or 360-degree rotation.
	None = 0, //WFC::Tiled3D::Rotations3D::None

	//Rotation along an axis, clockwise when looking at the positive face.
	AxisX_90 = WFC::Tiled3D::Rotations3D::AxisX_90,
	AxisX_180 = WFC::Tiled3D::Rotations3D::AxisX_180,
	AxisX_270 = WFC::Tiled3D::Rotations3D::AxisX_270,
	AxisY_90 = WFC::Tiled3D::Rotations3D::AxisY_90,
	AxisY_180 = WFC::Tiled3D::Rotations3D::AxisY_180,
	AxisY_270 = WFC::Tiled3D::Rotations3D::AxisY_270,
	AxisZ_90 = WFC::Tiled3D::Rotations3D::AxisZ_90,
	AxisZ_180 = WFC::Tiled3D::Rotations3D::AxisZ_180,
	AxisZ_270 = WFC::Tiled3D::Rotations3D::AxisZ_270,

	//Rotation by grabbing two opposite edges and rotating 180 degrees.
	//Notated with the face that the edges are parallel to, and "a" or "b"
	//    for "major diagonal" (i.e. one of the edges is an axis)
	//    or "minor diagonal" respectively.
	EdgesXa = WFC::Tiled3D::Rotations3D::EdgesXa,
	EdgesXb = WFC::Tiled3D::Rotations3D::EdgesXb,
	EdgesYa = WFC::Tiled3D::Rotations3D::EdgesYa,
	EdgesYb = WFC::Tiled3D::Rotations3D::EdgesYb,
	EdgesZa = WFC::Tiled3D::Rotations3D::EdgesZa,
	EdgesZb = WFC::Tiled3D::Rotations3D::EdgesZb,

	//Rotation by grabbing opposite corners and rotating 120 or 240 degrees.
	//Notated with one corner, and rotation amount
	//    (clockwise while staring at the notated corner).
	//The corner is notated as "CornerXYZ", where X, Y, and Z are either
	//    "A" for the min side, or "B" for the max side.
	CornerAAA_120 = WFC::Tiled3D::Rotations3D::CornerAAA_120,
	CornerAAA_240 = WFC::Tiled3D::Rotations3D::CornerAAA_240,
	CornerABA_120 = WFC::Tiled3D::Rotations3D::CornerABA_120,
	CornerABA_240 = WFC::Tiled3D::Rotations3D::CornerABA_240,
	CornerBAA_120 = WFC::Tiled3D::Rotations3D::CornerBAA_120,
	CornerBAA_240 = WFC::Tiled3D::Rotations3D::CornerBAA_240,
	CornerBBA_120 = WFC::Tiled3D::Rotations3D::CornerBBA_120,
	CornerBBA_240 = WFC::Tiled3D::Rotations3D::CornerBBA_240
};
static_assert(static_cast<int>(WFC_Rotations3D::None) == static_cast<int>(WFC::Tiled3D::Rotations3D::None),
			  "Needed to satisfy the compiler");


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

	bool operator==(const FWFC_Face& f) const
	{
		static_assert(sizeof(FWFC_Face) == (4 * sizeof(int)),
					  "Assumption used for fast equality is wrong");
		return memcmp(this, &f, sizeof(FWFC_Face)) == 0;
	}
};
template<>
struct TStructOpsTypeTraits<FWFC_Face> : public TStructOpsTypeTraitsBase2<FWFC_Face>
{
	enum
	{
		WithZeroConstructor = true,
		WithNoDestructor = true,
		WithIdenticalViaEquality = true
	};
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
	
	bool operator==(const FWFC_Cube& c) const
	{
		static_assert(sizeof(FWFC_Cube) == (4 * sizeof(int)) * 6,
		              "Assumption used for fast equality is incorrect");
		return memcmp(this, &c, sizeof(FWFC_Cube)) == 0;
	}
};
template<>
struct TStructOpsTypeTraits<FWFC_Cube> : public TStructOpsTypeTraitsBase2<FWFC_Cube>
{
	enum
	{
		WithZeroConstructor = true,
		WithNoDestructor = true,
		WithIdenticalViaEquality = true
	};
};


//A transformation that can be done to a cube (while keeping it axis-aligned).
USTRUCT(BlueprintType)
struct FWFC_Transform3D
{
	GENERATED_BODY()
	bool Invert;
	WFC_Rotations3D Rot;

	bool operator==(const FWFC_Transform3D& t) const
	{
		static_assert(sizeof(FWFC_Transform3D) == sizeof(bool) + sizeof(WFC_Rotations3D),
					  "Assumption used for fast equality is incorrect");
		return memcmp(this, &t, sizeof(FWFC_Transform3D)) == 0;
	}
};
template<>
struct TStructOpsTypeTraits<FWFC_Transform3D> : public TStructOpsTypeTraitsBase2<FWFC_Transform3D>
{
	enum
	{
		WithZeroConstructor = true,
		WithNoDestructor = true,
		WithIdenticalViaEquality = true
	};
};