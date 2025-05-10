#pragma once

#include <assert.h>
#include <iterator>
#include <math.h>

#include "../Platform.h"
#include "EnumFlags.h"
#include "WFCMath.h"

#pragma warning( push )
#pragma warning( disable: 4702 )

namespace WFC
{
    //NOTE: By convention, the positive Y axis points downward.

	//Things that can be done to 2D integer vectors.
    //This set is completely closed -- any combination of these transformations
    //    is equivalent to another one of these transformations.
	enum WFC_API Transformations
		//TODO: Make enum-class for better scoping.
	{
        None,

		Rotate90CW,
		Rotate180,
		Rotate270CW,

		//Flip the horizontal component; leave the vertical component alone.
		//In other words flip *across* the Y axis.
		FlipX,
		//Flip the vertical component; leave the horizontal component alone.
		//In other words flip *across* the X axis.
		FlipY,

        //Mirror along the primary diagonal, going from the min corner to the max corner.
        FlipDiag1,
        //Mirror along the secondary diagonal.
        FlipDiag2,

		Count, //The size of this enum list -- not an actual transformation
	    //TODO: Make the count a const or #define
	};

    //A Set of transformations.
    using TransformationFlags =
        EnumFlags<Transformations, uint_fast8_t, Transformations::Count>;

    WFC_API Transformations Invert(Transformations t);
    WFC_API bool WillSwapAxes(Transformations t);
    WFC_API const char* ToString(Transformations t);
    WFC_API bool IsReflection(Transformations t);


	//A 2D integer coordinate.
	struct WFC_API Vector2i
	{
	public:
		int32_t x;
		int32_t y;


		Vector2i() : Vector2i(0, 0) { }
		Vector2i(int X, int Y) : x(X), y(Y) { }


		//Access the X or Y component with 0 or 1, respectively.
		const int& operator[](int index) const { return (&x)[index]; }
		//Access the X or Y component with 0 or 1, respectively.
		int& operator[](int index) { return (&x)[index]; }


		Vector2i& operator+=(const Vector2i& other) { x += other.x; y += other.y; return *this; }
		Vector2i& operator-=(const Vector2i& other) { x -= other.x; y -= other.y; return *this; }
		Vector2i& operator*=(int i) { x *= i; y *= i; return *this; }
		Vector2i& operator/=(int i) { x /= i; y /= i; return *this; }

		Vector2i operator+(const Vector2i& other) const { return Vector2i(x + other.x, y + other.y); }
		Vector2i operator-(const Vector2i& other) const { return Vector2i(x - other.x, y - other.y); }
		Vector2i operator*(const Vector2i& other) const { return Vector2i(x * other.x, y * other.y); }
		Vector2i operator/(const Vector2i& other) const { return Vector2i(x / other.x, y / other.y); }

		Vector2i operator+(int i) const { return Vector2i(x + i, y + i); }
		Vector2i operator-(int i) const { return Vector2i(x - i, y - i); }
		Vector2i operator*(int i) const { return Vector2i(x * i, y * i); }
		Vector2i operator/(int i) const { return Vector2i(x / i, y / i); }

		Vector2i operator-() const { return Vector2i(-x, -y); }

		bool operator==(const Vector2i& v) const
		{
			static_assert(sizeof(Vector2i) == 2 * sizeof(int),
						  "Vector2i has some byte padding");
			return memcmp(this, &v, sizeof(Vector2i)) == 0;
		}
		bool operator!=(const Vector2i& v) const { return !operator==(v); }


		Vector2i LessX() const { return Vector2i(x - 1, y); }
		Vector2i MoreX() const { return Vector2i(x + 1, y); }
		Vector2i LessY() const { return Vector2i(x, y - 1); }
		Vector2i MoreY() const { return Vector2i(x, y + 1); }

	    //Transforms this vector with the given transformation, pivoting around the origin.
	    Vector2i Transform(Transformations transf) const { return Transform(transf, Vector2i(1, 1)); }
		//Transforms this vector with the given transformation,
		//    assuming the "pivot" is halfway between the origin and "size".
		Vector2i Transform(Transformations trnsf, Vector2i size) const;

		uint32_t GetHashcode() const
		{
			//This seems to be a significant bottleneck, so I spent some time thinking
			//    and searching for a fast way to hash integers.
			//UPDATE: I think the real bottleneck was elsewhere in the collections which perform this hash.
			//        Still a good idea to optimize hashing though; it's in the hot code loop.

			//Reference: https://stackoverflow.com/questions/30032950/creating-a-hash-seed-value-from-2-integers-fast

			//This hash works better with high bits than low ones; rotate the component bits before using them.
			int32_t a = (x << 16) | (x & 0xffff),
					b = (y << 16) | (y & 0xffff);

			a ^= b;
			a = (a << 24) + (a * 0x193);
			return a;
		}
		size_t GetHashcodeLarge() const
		{
			if constexpr (sizeof(size_t) == sizeof(uint64_t))
			{
				//We can get a perfect hash by zipping the bits together.
				size_t h = (static_cast<size_t>(x) << 32) | static_cast<size_t>(y);

				//However they might not have the best distribution, so we should still mix them.
				//Source on mixing: https://stackoverflow.com/a/12996028
				h = (h ^ (h >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
				h = (h ^ (h >> 27)) * UINT64_C(0x94d049bb133111eb);
				h = h ^ (h >> 31);

				return h;
			}

			//For other bit sizes (not common), fall back to the 32-bit behavior.
			return static_cast<size_t>(GetHashcode());
		}
	};


	//A rectangular region of 2D integer coordinates.
	//This structure can be iterated over in a ranged-based "for" loop.
	struct WFC_API Region2i
	{
	public:

		Vector2i MinInclusive, MaxExclusive;

		explicit Region2i(const Vector2i& maxExclusive)
			: MinInclusive(0, 0), MaxExclusive(maxExclusive) { }
		Region2i(const Vector2i& minInclusive, const Vector2i& maxExclusive)
			: MinInclusive(minInclusive), MaxExclusive(maxExclusive) { }

		bool Contains(const Vector2i& pos) const
		{
			return pos.x >= MinInclusive.x && pos.x < MaxExclusive.x &&
				   pos.y >= MinInclusive.y && pos.y < MaxExclusive.y;
		}
		Vector2i GetSize() const { return MaxExclusive - MinInclusive; }

        #pragma region const_iterator    
		//An std-style iterator over the region.
		struct WFC_API const_iterator
		{
		public:
			typedef const_iterator self_type;
			typedef Vector2i value_type;
			typedef Vector2i& reference;
			typedef Vector2i* pointer;
			typedef std::forward_iterator_tag iterator_category;
			typedef int difference_type;

			const Region2i& Owner;

			const_iterator(const Region2i& owner, Vector2i _pos = Vector2i())
				: Owner(owner), pos(_pos) { }

			self_type operator++() { auto _this = *this; AdvancePos(); return _this; }
			self_type operator++(int dummy) { AdvancePos(); return *this; }
			const reference operator*() { return pos; }
			const pointer operator->() { return &pos; }
			bool operator==(const self_type& rhs) { return pos == rhs.pos; }
			bool operator!=(const self_type& rhs) { return pos != rhs.pos; }

		private:
			Vector2i pos;

			inline void AdvancePos()
			{
				pos.x += 1;
				if (pos.x >= Owner.MaxExclusive.x)
				{
					pos.x = Owner.MinInclusive.x;
					pos.y += 1;
				}
			}
		};
        #pragma endregion

		Region2i::const_iterator begin() const
		{
			return const_iterator(*this, MinInclusive);
		}
		Region2i::const_iterator end() const
		{
			return const_iterator(*this, Vector2i(MinInclusive.x, MaxExclusive.y));
		}
	};
}

template<>
inline WFC::Vector2i WFC::Math::Max<WFC::Vector2i>(Vector2i a, Vector2i b)
{
	Vector2i x;
	for (int i = 0; i < 2; ++i)
		x[i] = Max(a[i], b[i]);
	return x;
}
template<>
inline WFC::Vector2i WFC::Math::Min<WFC::Vector2i>(Vector2i a, Vector2i b)
{
	Vector2i x;
	for (int i = 0; i < 2; ++i)
		x[i] = Min(a[i], b[i]);
	return x;
}

template<> struct std::hash<WFC::Vector2i>
{
	size_t operator()(const WFC::Vector2i& v) const { return v.GetHashcodeLarge(); }
};
template<> struct std::hash<WFC::Region2i>
{
	size_t operator()(const WFC::Region2i& r) const {
		return std::hash<WFC::Vector2i>{}({
			static_cast<int32_t>(r.MinInclusive.GetHashcode()),
			static_cast<int32_t>(r.MaxExclusive.GetHashcode())
		});
	}
};

#pragma warning( pop )