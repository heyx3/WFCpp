#pragma once

#include "Platform.h"
#include "EnumFlags.h"

#include <assert.h>
#include <iterator>
#include <math.h>


namespace WFC
{
    //NOTE: By convention, the positive Y axis points downward.

	//Things that can be done to 2D integer vectors.
    //Note that this set is completely closed -- any combination of these transformations
    //    is equivalent to another one of these transformations.
	//TODO: Make enum-class for better scoping.
	enum WFC_API Transformations
	{
        None,

		Rotate90CW,
		Rotate180,
		Rotate270CW,

		FlipX,
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

		//Gets the hash value for a vector instance.
		//Enables this class to be used for std collections that use hashes.
		inline unsigned int operator()(const Vector2i& v) const
		{
			int i = v.GetHashcode();
			return *((unsigned int*)(&i));
		}


		int x;
		int y;


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

		bool operator==(const Vector2i& v) const { return (x == v.x) && (y == v.y); }
		bool operator!=(const Vector2i& v) const { return (x != v.x) || (y != v.y); }


		Vector2i LessX() const { return Vector2i(x - 1, y); }
		Vector2i MoreX() const { return Vector2i(x + 1, y); }
		Vector2i LessY() const { return Vector2i(x, y - 1); }
		Vector2i MoreY() const { return Vector2i(x, y + 1); }

	    //Transforms this vector with the given transformation, pivoting around the origin.
	    Vector2i Transform(Transformations transf) const { return Transform(transf, Vector2i(1, 1)); }
		//Transforms this vector with the given transformation,
		//    assuming the "pivot" is halfway between the origin and "size".
		Vector2i Transform(Transformations trnsf, Vector2i size) const;

		int GetHashcode() const { return (x * 73856093) ^ (y * 19349663); }
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