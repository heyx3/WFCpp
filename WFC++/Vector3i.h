#pragma once

#include "Platform.h"
#include "EnumFlags.h"

#include <assert.h>
#include <iterator>
#include <math.h>


namespace WFC
{
    //NOTE: By convention, the positive Y axis points UPWARD (unlike Vector2i).

	//A 3D integer coordinate.
	struct WFC_API Vector3i
	{
	public:

		//Gets the hash value for a vector instance.
		//Enables this class to be used for std collections that use hashes.
		inline unsigned int operator()(const Vector3i& v) const
		{
			int i = v.GetHashCode();
			return *((unsigned int*)(&i));
		}


        int x, y, z;


		Vector3i() : Vector3i(0, 0, 0) { }
		Vector3i(int X, int Y, int Z) : x(X), y(Y), z(Z) { }


        //Access each component with 0, 1, or 2.
		const int& operator[](int index) const { return (&x)[index]; }
        //Access each component with 0, 1, or 2.
		int& operator[](int index) { return (&x)[index]; }


        Vector3i& operator+=(const Vector3i& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vector3i& operator-=(const Vector3i& other) { x -= other.x; y -= other.y; z += other.z; return *this; }
        Vector3i& operator*=(int i) { x *= i; y *= i; z *= i; return *this; }
        Vector3i& operator/=(int i) { x /= i; y /= i; z *= i; return *this; }

		Vector3i operator+(const Vector3i& other) const { return Vector3i(x + other.x, y + other.y, z + other.z); }
		Vector3i operator-(const Vector3i& other) const { return Vector3i(x - other.x, y - other.y, z - other.z); }
		Vector3i operator*(const Vector3i& other) const { return Vector3i(x * other.x, y * other.y, z * other.z); }
		Vector3i operator/(const Vector3i& other) const { return Vector3i(x / other.x, y / other.y, z / other.z); }

		Vector3i operator+(int i) const { return Vector3i(x + i, y + i, z + i); }
		Vector3i operator-(int i) const { return Vector3i(x - i, y - i, z - i); }
		Vector3i operator*(int i) const { return Vector3i(x * i, y * i, z * i); }
		Vector3i operator/(int i) const { return Vector3i(x / i, y / i, z / i); }

		Vector3i operator-() const { return Vector3i(-x, -y, -z); }

		bool operator==(const Vector3i& v) const { return (x == v.x) & (y == v.y) & (z == v.z); }
		bool operator!=(const Vector3i& v) const { return (x != v.x) | (y != v.y) | (z != v.z); }


		Vector3i LessX() const { return Vector3i(x - 1, y, z); }
		Vector3i MoreX() const { return Vector3i(x + 1, y, z); }
        Vector3i LessY() const { return Vector3i(x, y - 1, z); }
        Vector3i MoreY() const { return Vector3i(x, y + 1, z); }
        Vector3i LessZ() const { return Vector3i(x, y, z - 1); }
        Vector3i MoreZ() const { return Vector3i(x, y, z + 1); }

		int GetHashCode() const { return (x * 73856093) ^ (y * 19349663) ^ (z * 6869627); }
	};


	//A rectangular region of 3D integer coordinates.
	//This structure can be iterated over in a ranged-based "for" loop.
	struct WFC_API Region3i
	{
	public:

		Vector3i MinInclusive, MaxExclusive;

		explicit Region3i(const Vector3i& maxExclusive)
			: MinInclusive(0, 0, 0), MaxExclusive(maxExclusive) { }
		Region3i(const Vector3i& minInclusive, const Vector3i& maxExclusive)
			: MinInclusive(minInclusive), MaxExclusive(maxExclusive) { }

		bool Contains(const Vector3i& pos) const
		{
			return pos.x >= MinInclusive.x && pos.x < MaxExclusive.x &&
				   pos.y >= MinInclusive.y && pos.y < MaxExclusive.y &&
                   pos.z >= MinInclusive.z && pos.z < MaxExclusive.z;
		}
		Vector3i GetSize() const { return MaxExclusive - MinInclusive; }

		auto begin() const
		{
			return const_iterator(*this, MinInclusive);
		}
		auto end() const
		{
			return const_iterator(*this, Vector3i(MinInclusive.x, MinInclusive.y, MaxExclusive.z));
		}


		//An std-style iterator over the region.
		struct WFC_API const_iterator
		{
		public:
			typedef const_iterator self_type;
			typedef Vector3i value_type;
			typedef Vector3i& reference;
			typedef Vector3i* pointer;
			typedef std::forward_iterator_tag iterator_category;
			typedef int difference_type;

			const Region3i& Owner;

			const_iterator(const Region3i& owner, Vector3i _pos = Vector3i())
				: Owner(owner), pos(_pos) { }

			self_type operator++() { auto _this = *this; AdvancePos(); return _this; }
			self_type operator++(int dummy) { AdvancePos(); return *this; }
			const reference operator*() { return pos; }
			const pointer operator->() { return &pos; }
			bool operator==(const self_type& rhs) { return pos == rhs.pos; }
			bool operator!=(const self_type& rhs) { return pos != rhs.pos; }

		private:
			Vector3i pos;

			inline void AdvancePos()
			{
				pos.x += 1;
				if (pos.x >= Owner.MaxExclusive.x)
				{
					pos.x = Owner.MinInclusive.x;
                    pos.y += 1;

                    if (pos.y >= Owner.MaxExclusive.y)
                    {
                        pos.y = Owner.MinInclusive.y;
                        pos.z += 1;
                    }
				}
			}
		};
	};
}