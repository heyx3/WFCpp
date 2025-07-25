#pragma once

#include <stdint.h> //TODO: Why? remove this
#include <utility>
#include <array>

#include "../Platform.h"

#include "../Helpers/Vector2i.h"
#include "../Helpers/Vector3i.h"
#include "../Helpers/Vector4i.h"
#include "../Helpers/WFCMath.h"

namespace WFC
{
    namespace Tiled3D
    {
        //Note that we're assuming a left-handed coordinate system.
        //For visualization purposes we treat X as rightward, Z as forward, and Y as upwards.
        //This matches the coordinate system used by the Unity3D game engine.
        //The Unreal engine's coordinate system is also left-handed but with Z-up, X-forward.

        //NOTE: most/all the enums below have a specific ordering
        //    that a number of functions and lookup tables rely on.
        //Don't change them!

    	
    	//Points on a tile face are uniquely identified with an integer ID.
    	//The symmetries of a tile face come from assigning the same ID's to different points,
    	//    to make those points functionally interchangeable.
    	using PointID = std::uint_fast32_t;

        //Unique identifiers for the four corners or edges of a face,
        //     ordered in world-space to achieve trivial comparison of opposite faces.
        //Axes are always ordered X->Y->Z, for example on the Y face axis 1 is X and axis 2 is Z.
        //
        //For the four corners, each letter puts you on the Min ('A') or Max ('B') of that face axis.
        //For example on the X face, 'BA' means the 'MaxY-MinZ' corner.
        //
        //For the four edges, the first letter represents
        //    the axis parallel to that edge, 1 ('A') or 2 ('B'),
        //    while the second letter represents Min or Max side.
        //E.g. on the Y face 'AB' means the {0, 0, 1}=>{1, 0, 1} edge
        //    and 'BB' means the {1, 0, 0}=>{1, 0, 1} edge.
    	//
    	/* Refer to the diagram below:
		    2
		    AB ----- AB ----- BB
		    |                  |
		    |                  |
		   BA                 BB
		    |                  |
		    |                  |
		    AA ----- AA ----- BA 1
		Origin
		*/
        enum WFC_API FacePoints
        {
            AA, AB,
            BA, BB
        };
        constexpr int N_FACE_POINTS = 4;
        template<typename T>
        using PerFacePoint = std::array<T, N_FACE_POINTS>;

        inline bool WFC_API IsCornerFirstAxisMin(FacePoints p) { return ((uint_fast8_t)p / 2) == 0; }
        inline bool WFC_API IsCornerSecondAxisMin(FacePoints p) { return ((uint_fast8_t)p % 2) == 0; }
        inline FacePoints WFC_API MakeCornerFacePoint(bool axis1IsMin, bool axis2IsMin)
        {
            return axis1IsMin ?
                       (axis2IsMin ? FacePoints::AA : FacePoints::AB) :
                       (axis2IsMin ? FacePoints::BA : FacePoints::BB);
        }
        //Generates a vector whose components are -1 or +1, representing a face corner.
        inline Vector2i WFC_API MakeCornerFaceVector(FacePoints point) { return { (IsCornerFirstAxisMin(point) ? -1 : 1), (IsCornerSecondAxisMin(point) ? -1 : 1) }; }
    	//The inverse of 'MakeCornerFaceVector'.
    	inline FacePoints WFC_API MakeCornerFacePoint(Vector2i faceVector)
        {
	        return MakeCornerFacePoint(faceVector.x < 0, faceVector.y < 0);
        }

        inline bool WFC_API IsEdgeParallelToFirstAxis(FacePoints p) { return ((uint_fast8_t)p / 2) == 0; }
        inline bool WFC_API IsEdgeOnMinSide(FacePoints p) { return ((uint_fast8_t)p % 2) == 0; }
        inline FacePoints WFC_API MakeEdgeFacePoint(bool isParallelToAxis1, bool onMinSide)
        {
            return isParallelToAxis1 ?
                       (onMinSide ? FacePoints::AA : FacePoints::AB) :
                       (onMinSide ? FacePoints::BA : FacePoints::BB);
        }
        //Generates a vector with one component 0 and one component +/- 1,
        //    pointing towards the center of a given face edge.
        inline Vector2i WFC_API MakeEdgeFaceVector(FacePoints point)
        {
            Vector2i v;
            v[IsEdgeParallelToFirstAxis(point) ? 1 : 0] = IsEdgeOnMinSide(point) ? -1 : 1;
            return v;
        }
    	//The inverse of 'MakeEdgeFaceVector'. 
    	inline FacePoints WFC_API MakeEdgeFacePoint(Vector2i vector)
        {
	        return MakeEdgeFacePoint(
	        	vector.x == 0,
	        	std::min(vector.x, vector.y) < 0
	        );
        }


        //The different axis directions/faces of a cube.
        enum WFC_API Directions3D : uint8_t
        {
            MinX, MaxX,
            MinY, MaxY,
            MinZ, MaxZ,
        };
        const uint_fast8_t N_DIRECTIONS_3D = 6;

        inline bool WFC_API IsMin(Directions3D dir) { return (uint_fast8_t)dir % 2 == 0; }
        inline bool WFC_API IsMax(Directions3D dir) { return (uint_fast8_t)dir % 2 == 1; }
        inline uint_fast8_t GetAxisIndex(Directions3D dir) { return (uint_fast8_t)dir / 2; }
        inline Directions3D WFC_API GetOpposite(Directions3D dir) { return (Directions3D)(IsMin(dir) ? (dir + 1) : (dir - 1)); }
        inline Directions3D WFC_API MakeDirection3D(bool isMin, int axis) { return (Directions3D)((isMin ? 0 : 1) + (axis * 2)); }
        inline Vector3i GetFaceDirection(Directions3D dir)
        {
            switch (dir)
            {
                case Directions3D::MinX: return Vector3i(-1, 0, 0);
                case Directions3D::MaxX: return Vector3i(1, 0, 0);

                case Directions3D::MinY: return Vector3i(0, -1, 0);
                case Directions3D::MaxY: return Vector3i(0, 1, 0);

                case Directions3D::MinZ: return Vector3i(0, 0, -1);
                case Directions3D::MaxZ: return Vector3i(0, 0, 1);

                default: WFCPP_ASSERT(false); return Vector3i(1, 0, 0);
            }
        }
        //Gets the axis the given direction is pointing in,
        //    as well as the other two axes in world-space order (i.e. X before Y, Y before Z).
        inline void GetAxes(Directions3D dir, uint_fast8_t& outMain,
                            uint_fast8_t& outPlane1, uint_fast8_t& outPlane2)
        {
            outMain = GetAxisIndex(dir);
            
            outPlane1 = (outMain + uint_fast8_t{ 1 }) % uint_fast8_t{ 3 };
            outPlane2 = (outPlane1 + uint_fast8_t{ 1 }) % uint_fast8_t{ 3 };

            if (outPlane2 < outPlane1)
                std::swap(outPlane1, outPlane2);
        }
    	//Gets whether the given face has left-handed coordinates
    	//    using the two face axes in our conventional ordering (see 'GetAxes()').
    	inline bool IsFaceLeftHanded(Directions3D dir)
        {
	        switch (dir)
			{
				case Directions3D::MinX:
				case Directions3D::MaxY:
				case Directions3D::MinZ:
					return true;
				case Directions3D::MaxX:
				case Directions3D::MinY:
				case Directions3D::MaxZ:
					return false;
				
				default: WFCPP_ASSERT(false); return true;
			}
        }
    	
    	//Applies a 2D transformation to the given corner of the given face.
    	//This transformation is from the point of view looking at this face on the rotation axis.
    	WFC_API FacePoints TransformFaceCorner(FacePoints p, Directions3D dir, Transformations tr2D);
    	//Applies a 2D transformation to the given edge of the given face.
    	//This transformation is from the point of view looking at this face on the rotation axis.
    	WFC_API FacePoints TransformFaceEdge(FacePoints p, Directions3D dir, Transformations tr2D);

        //Unique identifier for a face, including any symmetries implicit in the arrangement of corners and faces.
        struct WFC_API FaceIdentifiers
        {
            WFCPP_MEMORY_CHECK_HEADER(8, "FaceIdentifiers struct");

            //Unique identifiers for each corner, indexed with the 'FacePoints' enum.
            PerFacePoint<PointID> Corners;
            //Unique identifiers for each edge, indexed with the 'FacePoints' enum.
            PerFacePoint<PointID> Edges;

            FaceIdentifiers()
            {
                for (int i = 0; i < N_FACE_POINTS; ++i)
                {
                    Corners[i] = 0;
                    Edges[i] = 0;
                }
            }
            FaceIdentifiers(const PerFacePoint<PointID>& corners, const PerFacePoint<PointID>& edges)
                : Corners(corners), Edges(edges) { }

            inline uint32_t GetHashCode() const
            {
                uint64_t h = GetHashcodeLarge();

                return Vector2i(
                    static_cast<int32_t>(h >> 32),
                    static_cast<int32_t>(h & 0xffffffff)
                ).GetHashcode();
            }
            inline uint64_t GetHashcodeLarge() const
            {
                uint64_t a = Vector4i(
                    static_cast<int32_t>(Corners[0]),
                    static_cast<int32_t>(Corners[1]),
                    static_cast<int32_t>(Corners[2]),
                    static_cast<int32_t>(Corners[3])
                ).GetHashcodeLarge();
                uint64_t b = Vector4i(
                    static_cast<int32_t>(Edges[0]),
                    static_cast<int32_t>(Edges[1]),
                    static_cast<int32_t>(Edges[2]),
                    static_cast<int32_t>(Edges[3])
                ).GetHashcodeLarge();

                return a ^ b;
            }

            inline bool operator==(const FaceIdentifiers& f2) const
            {
                return std::equal(Corners.begin(), Corners.end(), f2.Corners.begin()) &&
                       std::equal(Edges.begin(), Edges.end(), f2.Edges.begin());
            }
            inline bool operator!=(const FaceIdentifiers& f2) const { return !(operator==(f2)); }

            
            WFCPP_MEMORY_CHECK_FOOTER(8);
        };

        //A specific permutation of a face, on a specific side of the cube.
        //Two faces line up against each other if they have opposite Side and their Points are identical.
        struct WFC_API FacePermutation
        {
            WFCPP_MEMORY_CHECK_HEADER(16, "FacePermutation struct");

            //The side of the cube this face is on.
            Directions3D Side;
            //The identifying edges/corners of this face,
            //    implicitly carrying info about how this face got flipped/rotated.
            FaceIdentifiers Points;

            FacePermutation(Directions3D side = Directions3D::MinX) : Side(side) { }
            FacePermutation(Directions3D side, const FaceIdentifiers& points) : Side(side), Points(points) { }


            //Gets the matching face on the opposite side.
            FacePermutation Flipped() const
            {
                auto copy = *this;

                copy.Side = GetOpposite(Side);
                //The points, by design, are ordered the same
                //    regardless of whether you're on the min or max side.

                return copy;
            }


            inline uint32_t GetHashCode() const
            {
                return Vector2i(Points.GetHashCode(), Side).GetHashcode();
            }
            inline uint64_t GetHashcodeLarge() const
            {
                uint64_t a = Points.GetHashcodeLarge();
                uint64_t b = Vector2i(
                    static_cast<int32_t>(Side),
                    ~static_cast<int32_t>(Side)
                ).GetHashcodeLarge();

                return a ^ b;
            }

            inline bool operator==(const FacePermutation& f2) const
            {
                return Side == f2.Side && Points == f2.Points;
            }
            inline bool operator!=(const FacePermutation& f2) const { return !(operator==(f2)); }


            WFCPP_MEMORY_CHECK_FOOTER(16);
            inline void DEBUGMEM_ValidateAll()
            {
                DEBUGMEM_Validate();
                Points.DEBUGMEM_Validate();
            }
        };


        //Different ways to rotate a cube while keeping it axis-aligned.
        enum class WFC_API Rotations3D : uint8_t
        {
            //Note that some functions and lookup tables depend on the exact ordering here!

            //0- or 360-degree rotation.
            None,

            //Rotation along an axis, clockwise when looking at the positive face.
            AxisX_90, AxisX_180, AxisX_270,
            AxisY_90, AxisY_180, AxisY_270,
            AxisZ_90, AxisZ_180, AxisZ_270,

            //Rotation by grabbing two opposite edges and rotating 180 degrees.
            //Notated with the axis that the edges are parallel to, and "a" or "b"
            //    for "major diagonal" (i.e. one of the edges is an axis)
            //    or "minor diagonal" respectively.
            EdgesXa, EdgesXb,
            EdgesYa, EdgesYb,
            EdgesZa, EdgesZb,

            //Rotation by grabbing opposite corners and rotating 120 or 240 degrees.
            //Notated with one corner, and rotation amount
            //    (clockwise while staring at the notated corner).
            //The corner is notated as "CornerXYZ", where X, Y, and Z are either
            //    "A" for the min side, or "B" for the max side.
            CornerAAA_120, CornerAAA_240,
            CornerABA_120, CornerABA_240,
            CornerBAA_120, CornerBAA_240,
            CornerBBA_120, CornerBBA_240,
        };
        constexpr uint_fast16_t N_ROTATIONS_3D = 24;
        WFC_API Rotations3D CombineRotations(Rotations3D first, Rotations3D second);


        //The faces of a cube, with memory of how they have been transformed.
        struct WFC_API CubePermutation
        {
            WFCPP_MEMORY_CHECK_HEADER(16, "CubePermutation struct");

            //The faces of this cube.
            //Their positions in the array are based on the original un-transformed cube --
            //    e.x. Faces[Directions::MinX] is the face that STARTED as the MinX face.
            std::array<FacePermutation, N_DIRECTIONS_3D> Faces;

            CubePermutation()
                : CubePermutation(FacePermutation{ Directions3D::MinX }, FacePermutation{ Directions3D::MaxX },
                                  FacePermutation{ Directions3D::MinY }, FacePermutation{ Directions3D::MaxY },
                                  FacePermutation{ Directions3D::MinZ }, FacePermutation{ Directions3D::MaxZ })
            {

            }
            CubePermutation(const FacePermutation& minX, const FacePermutation& maxX,
                            const FacePermutation& minY, const FacePermutation& maxY,
                            const FacePermutation& minZ, const FacePermutation& maxZ)
                : Faces{ minX, maxX, minY, maxY, minZ, maxZ }
            {
            }

            //Gets the index in "Faces" for the face currently facing the given direction.
            uint_fast8_t GetFace(Directions3D dir) const;

            WFCPP_MEMORY_CHECK_FOOTER(16);
            inline void DEBUGMEM_ValidateAll()
            {
                DEBUGMEM_Validate();
                for (const auto& facePermutation : Faces)
                    facePermutation.DEBUGMEM_Validate();
            }
        };


        //Reprsents a transformation that can be done to a cube (while keeping it axis-aligned).
        //The default/zero-constructor produces the identity transform.
        struct WFC_API Transform3D
        {
            //If true, this transform flips the cube along every axis before rotating.
            bool Invert = false;
            //The rotation that is applied (AFTER the inversion, if applicable).
            Rotations3D Rot = Rotations3D::None;

            inline bool IsIdentity() const { return !Invert && Rot == Rotations3D::None; }

            //Applies this transformation to the given 3D position.
            //By default centered around the origin, but you can provide a "max" value
            //    so that the transformation is in the space from 0 to "max", inclusive.
            Vector3i ApplyToPos(Vector3i pos, Vector3i maxInclusive = Vector3i::Zero()) const;

            //Gets the position of the given face after this transformation.
            Directions3D ApplyToSide(Directions3D currentSide) const;
            //Gets the new permutated face after this transformation.
            FacePermutation ApplyToFace(FacePermutation currentFace) const;
            //Gets the new permutated cube after this transformation.
            CubePermutation ApplyToCube(CubePermutation currentCube) const;

            inline Transform3D Inverse() const
            {
                //This is actually quite simple because you can consider
                //    the inversion and rotation separately.

                static const std::array<Rotations3D, N_ROTATIONS_3D> rotLookup = {
                    Rotations3D::None, //None
                    Rotations3D::AxisX_270, //AxisX_90
                    Rotations3D::AxisX_180, //AxisX_180
                    Rotations3D::AxisX_90, //AxisX_270
                    Rotations3D::AxisY_270, //AxisY_90
                    Rotations3D::AxisY_180, //AxisY_180
                    Rotations3D::AxisY_90, //AxisY_270
                    Rotations3D::AxisZ_270, //AxisZ_90
                    Rotations3D::AxisZ_180, //AxisZ_180
                    Rotations3D::AxisZ_90, //AxisZ_270
                    Rotations3D::EdgesXa, //EdgesXa
                    Rotations3D::EdgesXb, //EdgesXb
                    Rotations3D::EdgesYa, //EdgesYa
                    Rotations3D::EdgesYb, //EdgesYb
                    Rotations3D::EdgesZa, //EdgesZa
                    Rotations3D::EdgesZb, //EdgesZb
                    Rotations3D::CornerAAA_240, //CornerAAA_120
                    Rotations3D::CornerAAA_120, //CornerAAA_240
                    Rotations3D::CornerABA_240, //CornerABA_120
                    Rotations3D::CornerABA_120, //CornerABA_240
                    Rotations3D::CornerBAA_240, //CornerBAA_120
                    Rotations3D::CornerBAA_120, //CornerBAA_240
                    Rotations3D::CornerBBA_240, //CornerBBA_120
                    Rotations3D::CornerBBA_120, //CornerBBA_240
                };

                return { Invert, rotLookup[(int)Rot] };
            }
            //Applies the given transform after this one, and returns the resulting transform.
            Transform3D Then(const Transform3D& tr2) const;

            //Generates a perfect, unique hash code for this instance.
            using HashType = uint_fast8_t;
            HashType GetID() const
            {
                static_assert(sizeof(Rotations3D) == 1, "Rotations3D changed size! Update this hash function");
                HashType field1 = static_cast<HashType>(Rot),
                         field2 = (Invert ? HashType{1} : HashType{0}) << HashType{5};
                return field1 | field2;
            }
            //Mixes the perfect, unique hash code in order to provide a more even bit distribution.
            uint32_t GetHashcode() const
            {
                auto h = static_cast<uint32_t>(GetID());

                //Source on mixing: https://stackoverflow.com/a/12996028
                h = ((h >> 16) ^ h) * 0x45d9f3b;
                h = ((h >> 16) ^ h) * 0x45d9f3b;
                h = (h >> 16) ^ h;

                return h;
            }
        };
        constexpr int N_TRANSFORMS = N_ROTATIONS_3D * 2;

        //Gets the face data for a transformed cube.
        WFC_API FacePermutation GetFace(CubePermutation cubeBeforeTransform,
		                                Transform3D transform,
		                                Directions3D sideAfterTransform);

        inline bool operator==(Transform3D t1, Transform3D t2)
        {
            return (t1.Invert == t2.Invert) && (t1.Rot == t2.Rot);
        }
        inline bool operator!=(Transform3D t1, Transform3D t2)
        {
            return !operator==(t1, t2);
        }


        //An optimized and sorted collection of all possible transforms.
        //
        //The first half of the bits are for non-inverted rotations;
        //    the second half are for inverted ones.
        //
        //The rotations are ordered by their enum values.
        struct TransformSet
            //NOTE: static constexpr ints fail to compile for some reason when this class is marked WFC_API,
            //     so everything is kept inlined in the header.
            
            //TODO: Move to its own file.
        {
        public:
            
            WFCPP_MEMORY_CHECK_HEADER(16, "TransformSet struct");
            
            static constexpr uint_fast8_t BIT_COUNT = N_TRANSFORMS;
            
            using BitsType = Math::SmallestUInt<BIT_COUNT>;
            static constexpr BitsType ZERO = 0,
                                      ONE = 1,
                                      ALL_BITS = ~ZERO,
                                      N_TYPE_BITS = 8 * sizeof(BitsType),
                                      FIRST_INVERT_BIT_IDX = N_ROTATIONS_3D,
                                      USED_BITS = ALL_BITS >> (N_TYPE_BITS - BIT_COUNT),
                                      UNINVERTED_BITS = USED_BITS >> FIRST_INVERT_BIT_IDX,
                                      INVERTED_BITS = USED_BITS & (~UNINVERTED_BITS);


            //Gets the index of the bit for this transform.
            //This doubles as a unique index/perfect hash!
            static uint_fast8_t ToBitIdx(Transform3D tr)
            {
                return (uint_fast8_t)tr.Rot +
                       (uint_fast8_t)(tr.Invert ? FIRST_INVERT_BIT_IDX : 0);
            }
            //Turns a transformation into a specific bit.
            static BitsType ToBits(Transform3D tr)
            {
                return ONE << ToBitIdx(tr);
            }
            //Turns a specific bit-pattern into the Transform it represents.
            static Transform3D FromBits(BitsType bits)
            {
                return FromBit(Math::FindBitIndex(bits));
            }
            //Finds the transform corresponding to a specific bit.
            static Transform3D FromBit(uint_fast8_t bitIdx)
            {
                return (bitIdx < FIRST_INVERT_BIT_IDX) ?
                           Transform3D{ false, (Rotations3D)bitIdx } :
                           Transform3D{ true, (Rotations3D)(bitIdx - FIRST_INVERT_BIT_IDX) };
            }

            static TransformSet All() { TransformSet s; s.bits = USED_BITS; s.nBits = BIT_COUNT; return s; }


            //Creates a set from any combination of iterators, subsets, and elements.
            template<typename T>
            static TransformSet Combine(const T& iterable)
            {
                TransformSet set;
                for (Transform3D tr : iterable)
                {
                    set.bits |= ToBits(tr);
                    set.nBits += 1;
                }
                return set;
            }
            #pragma warning( push )
            #pragma warning( disable: 4499 ) //Warning about use of 'static' below, but that's needed for Unreal 4/C++17
            template<> static TransformSet Combine<TransformSet>(const TransformSet& set) { return set; }
            template<> static TransformSet Combine<Transform3D>(const Transform3D& transf)
            {
                TransformSet s;
                s.Add(transf);
                return s;
            }
            template<typename T, typename... Arguments>
            static TransformSet Combine(T first, Arguments... rest)
            {
                TransformSet tFirst = Combine(first),
                             tRest = Combine(rest...);
                tRest.Add(tFirst);
                return tRest;
            }
            #pragma warning( pop )


            uint_fast8_t Size() const { return nBits; }
            BitsType Bits() const { return bits; }

            bool Contains(Transform3D tr) const { return (bits & ToBits(tr)) != ZERO; }
            bool Add(Transform3D tr)
            {
                auto newBits = ToBits(tr);
                bool contained = (newBits & bits) != ZERO;

                nBits += contained ? ZERO : ONE;
                bits |= newBits;

                return contained;
            }
            bool Remove(Transform3D tr)
            {
                auto newBits = ToBits(tr);
                bool contained = (newBits & bits) != ZERO;

                nBits -= contained ? ONE : ZERO;
                bits &= ~newBits;

                return contained;
            }

            bool Contains(TransformSet set) const
            {
                return (bits & set.bits) == set.bits;
            }
            //Adds the given set to this one.
            //Returns how many new elements were added.
            uint_fast8_t Add(TransformSet set)
            {
                auto prevNBits = nBits;

                bits |= set.bits;
                nBits = Math::CountBits(bits);

                WFCPP_ASSERT(nBits >= prevNBits);
                return nBits - prevNBits;
            }
            //Removes the given elements from this set.
            //Returns how many elements were removed.
            uint_fast8_t Remove(TransformSet set)
            {
                auto prevNBits = nBits;

                bits &= ~(set.bits);
                nBits = Math::CountBits(bits);

                WFCPP_ASSERT(nBits <= prevNBits);
                return prevNBits - nBits;
            }
            //Removes all elements of this set except for those in the given one.
            //Returns how many elements were removed.
            uint_fast8_t Intersect(TransformSet set)
            {
                auto prevNBits = nBits;

                bits &= set.bits;
                nBits = Math::CountBits(bits);

                WFCPP_ASSERT(nBits <= prevNBits);
                return prevNBits - nBits;
            }

            //Adds some iteration of tranforms to this set.
            //Returns the number of actual new elements.
            template<typename Iter>
            uint_fast8_t Add(const Iter& iterable)
            {
                uint_fast8_t newCount = 0;
                for (Transform3D tr : iterable)
                    newCount += Add(tr);
                return newCount;
            }
            //Adds all inverted versions of this set's un-inverted transforms.
            //Existing inverted transforms will stay in the set.
            void AddInvertedVersions()
            {
                bits |= (bits & UNINVERTED_BITS) << FIRST_INVERT_BIT_IDX;
                nBits = Math::CountBits(bits);
            }

            void Clear() { bits = ZERO; nBits = ZERO; }

            //Implement equality/hashing for WFC dictionaries.
            bool operator==(TransformSet t) const { return bits == t.bits; }
            auto operator()(TransformSet t)
            {
                if constexpr (sizeof(BitsType) <= sizeof(size_t))
                {
                    return bits;
                }
                else
                {
                    return std::hash<BitsType>()(bits);
                }
            }

            //Implement iteration.
            #pragma region Iterator

            struct ConstIterator
            {
                using iterator_category = std::forward_iterator_tag;
                using difference_type = int32_t;
                using value_type = Transform3D;
                using pointer = Transform3D;
                using reference = Transform3D;
                //TODO: make proxies for references to elements.

                const TransformSet* Set;
                uint_fast8_t CurrentBit;

                //Makes an iterator starting at the given bit.
                //Automatically moves forward to the first actual element.
                ConstIterator(const TransformSet& set, uint_fast8_t startBit)
                    : Set(&set), CurrentBit(startBit)
                {
                    //Jump forward to the first existing rotation.
                    while (CurrentBit < BIT_COUNT && !IsValidElement())
                        CurrentBit += 1;
                }
                //Makes an 'end()' iterator.
                ConstIterator(const TransformSet& set) : Set(&set), CurrentBit(BIT_COUNT) { }

                ConstIterator& operator=(const ConstIterator& iter) { Set = iter.Set; CurrentBit = iter.CurrentBit; return *this; }
                ConstIterator& operator++()
                {
                    while (CurrentBit < BIT_COUNT)
                    {
                        CurrentBit += 1;
                        if (IsValidElement())
                            break;
                    }
                    return *this;
                };

                bool operator==(const ConstIterator& iter) const { return (Set == iter.Set) && (CurrentBit == iter.CurrentBit); }
                bool operator!=(const ConstIterator& iter) const { return !operator==(iter); }

                Transform3D operator*() const { return FromBit(CurrentBit); }
                Transform3D operator->() const { return operator*(); }


            private:
                bool IsValidElement() const { return Set->Contains(FromBit(CurrentBit)); }
            };

            #pragma endregion

            auto begin() const { return ConstIterator(*this, 0); }
            auto end() const { return ConstIterator(*this); }

        private:
            BitsType bits = 0;
            uint_fast8_t nBits = 0;

        public:
            WFCPP_MEMORY_CHECK_FOOTER(16);
        };

        //A group of parameters to help the user easily specify a large group of transforms.
        //A small set of legal rotations/inversions are given
        //     and then all possible combinations of these become part of the set.
        //
        //The default constructor sets all rotation/inversion flags to false
        //     and leaves you with only the identity transform.
        struct WFC_API ImplicitTransformSet
        {
            WFCPP_MEMORY_CHECK_HEADER(16, "ImplicitTransformSet struct");

            TransformSet InitialTransforms = TransformSet::Combine(
                Transform3D{ false, Rotations3D::None }
            );

            bool AllowInversion = false;
            bool AllowAllRotations = false;
            bool AllowAxisRots = false,
                 AllowCornerRots = false,
                 AllowEdgeRots = false;
            bool AllowAxisXRots = false,
                 AllowAxisYRots = false,
                 AllowAxisZRots = false,
                 AllowEdgeXRots = false,
                 AllowEdgeYRots = false,
                 AllowEdgeZRots = false;
        	TransformSet SpecificAllowedTransforms;

            TransformSet GetExplicit() const;

        private:
            uint_fast8_t PackFlags() const;

        public:
            WFCPP_MEMORY_CHECK_FOOTER(16);
        };
    }
}

template<> struct std::hash<WFC::Tiled3D::Transform3D>
{
    size_t operator()(const WFC::Tiled3D::Transform3D& t) const
    {
        uint32_t h = t.GetHashcode();
        if constexpr (std::is_same_v<size_t, uint64_t>)
            return (static_cast<uint64_t>(h) << 32) | h;
        else
            return static_cast<size_t>(h);
    }
};
template<> struct std::hash<WFC::Tiled3D::FaceIdentifiers>
{
    size_t operator()(const WFC::Tiled3D::FaceIdentifiers& i) const
    {
        if constexpr (std::is_same_v<size_t, uint64_t>)
            return i.GetHashcodeLarge();
        else
            return static_cast<size_t>(i.GetHashCode());
    }
};
template<> struct std::hash<WFC::Tiled3D::FacePermutation>
{
    size_t operator()(const WFC::Tiled3D::FacePermutation& f) const
    {
        if constexpr (std::is_same_v<size_t, uint64_t>)
            return f.GetHashcodeLarge();
        else
            return static_cast<size_t>(f.GetHashCode());
    }
};
template<> struct std::hash<WFC::Tiled3D::TransformSet>
{
    size_t operator()(const WFC::Tiled3D::TransformSet& s)
    {
        //If the bits representation fits directly into the hash-code type, then our job is easy.
        if constexpr (WFC::Tiled3D::TransformSet::BIT_COUNT <= sizeof(size_t) * 8)
            return static_cast<size_t>(s.Bits());
        //If the bits are not more than twice as large as the hash-code type
        //    (e.x. 32-bit system/hashes while the set is 64-bit), then combine the two halves of bits.
        else if constexpr (sizeof(size_t) * 8 * 2 >= WFC::Tiled3D::TransformSet::BIT_COUNT)
        {
            auto bits = s.Bits();

            auto fitSizeBytes = static_cast<decltype(bits)>(sizeof(size_t));
            auto fitMask = static_cast<decltype(bits)>(~size_t{ 0 });

            auto bitsHalves = std::to_array({
                static_cast<size_t>(bits >> (fitSizeBytes * 8)),
                static_cast<size_t>(bits & fitMask)
            });
            return bitsHalves[1] ^ bitsHalves[2];
        }
        //Other cases should basically never happen; no need to be clever about hashing them.
        else
        {
            return static_cast<size_t>(s.Bits());
        }
    }
};
template<> struct std::hash<WFC::Tiled3D::ImplicitTransformSet>
{
    size_t operator()(const WFC::Tiled3D::ImplicitTransformSet& s)
    {
        return std::hash<WFC::Tiled3D::TransformSet>{}(s.GetExplicit());
    }
};