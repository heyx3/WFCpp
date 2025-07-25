#pragma once

#include <assert.h>
#include <iterator>
#include <math.h>

#include "../Platform.h"
#include "EnumFlags.h"
#include "Vector2i.h"


namespace WFC
{
	//A 3D integer coordinate.
	struct WFC_API Vector3i
	{
	public:

        static inline Vector3i Zero() { return Vector3i(); }


        int x, y, z;


		Vector3i() : Vector3i(0, 0, 0) { }
		Vector3i(int X, int Y, int Z) : x(X), y(Y), z(Z) { }
		Vector3i(const Vector2i& xy, int Z) : x(xy.x), y(xy.y), z(Z) { }
		Vector3i(int X, const Vector2i& yz) : x(X), y(yz.x), z(yz.y) { }


        //Access each component with 0, 1, or 2.
		const int& operator[](uint_fast8_t index) const { return (&x)[index]; }
        //Access each component with 0, 1, or 2.
		int& operator[](uint_fast8_t index) { return (&x)[index]; }


        Vector3i& operator+=(const Vector3i& other) { x += other.x; y += other.y; z += other.z; return *this; }
        Vector3i& operator-=(const Vector3i& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
        Vector3i& operator*=(int i) { x *= i; y *= i; z *= i; return *this; }
        Vector3i& operator/=(int i) { x /= i; y /= i; z /= i; return *this; }

		Vector3i operator+(const Vector3i& other) const { return Vector3i(x + other.x, y + other.y, z + other.z); }
		Vector3i operator-(const Vector3i& other) const { return Vector3i(x - other.x, y - other.y, z - other.z); }
		Vector3i operator*(const Vector3i& other) const { return Vector3i(x * other.x, y * other.y, z * other.z); }
		Vector3i operator/(const Vector3i& other) const { return Vector3i(x / other.x, y / other.y, z / other.z); }

		Vector3i operator+(int i) const { return Vector3i(x + i, y + i, z + i); }
		Vector3i operator-(int i) const { return Vector3i(x - i, y - i, z - i); }
		Vector3i operator*(int i) const { return Vector3i(x * i, y * i, z * i); }
		Vector3i operator/(int i) const { return Vector3i(x / i, y / i, z / i); }

		Vector3i operator-() const { return Vector3i(-x, -y, -z); }

		bool operator==(const Vector3i& v) const
		{
			static_assert(sizeof(Vector3i) == 3 * sizeof(int),
						  "Vector3i has some byte padding");
			return memcmp(this, &v, sizeof(Vector3i)) == 0;
		}
		bool operator!=(const Vector3i& v) const { return !operator==(v); }


		Vector3i LessX() const { return Vector3i(x - 1, y, z); }
		Vector3i MoreX() const { return Vector3i(x + 1, y, z); }
        Vector3i LessY() const { return Vector3i(x, y - 1, z); }
        Vector3i MoreY() const { return Vector3i(x, y + 1, z); }
        Vector3i LessZ() const { return Vector3i(x, y, z - 1); }
        Vector3i MoreZ() const { return Vector3i(x, y, z + 1); }

		Vector3i Min(const Vector3i& b) const { return { std::min(x, b.x), std::min(y, b.y), std::min(z, b.z) }; }
		Vector3i Max(const Vector3i& b) const { return { std::max(x, b.x), std::max(y, b.y), std::max(z, b.z) }; }

		uint32_t GetHashcode() const
		{
			return Vector2i(Vector2i(x, y).GetHashcode(), z).GetHashcode();
		}
		uint64_t GetHashcodeLarge() const
		{
			return Vector2i(x, y).GetHashcodeLarge() ^
				   Vector2i(y, z).GetHashcodeLarge();
		}
		template<typename _ = size_t> //Templating is needed to static_assert(false) in the fallthrough case
		size_t GetSTLHashcode() const
		{
			if constexpr (sizeof(size_t) == 4)
				return static_cast<size_t>(GetHashcode());
			else if constexpr (sizeof(size_t) == 8)
				return static_cast<size_t>(GetHashcodeLarge());
			else
				static_assert(std::is_same_v<_, void>, "Unexpected hashcode type size");
		}
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
		int GetNumbElements() const
		{
			auto size = GetSize();
			return (size.x * size.y * size.z);
		}

        #pragma region const_iterator
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
        #pragma endregion

		Region3i::const_iterator begin() const
		{
			return const_iterator(*this, MinInclusive);
		}
		Region3i::const_iterator end() const
		{
			return const_iterator(*this, Vector3i(MinInclusive.x, MinInclusive.y, MaxExclusive.z));
		}
	};
}

template<>
inline WFC::Vector3i WFC::Math::Max<WFC::Vector3i>(Vector3i a, Vector3i b)
{
	Vector3i x;
	for (int i = 0; i < 3; ++i)
		x[i] = Max(a[i], b[i]);
	return x;
}
template<>
inline WFC::Vector3i WFC::Math::Min<WFC::Vector3i>(Vector3i a, Vector3i b)
{
	Vector3i x;
	for (int i = 0; i < 3; ++i)
		x[i] = Min(a[i], b[i]);
	return x;
}


template<> struct std::hash<WFC::Vector3i>
{
	size_t operator()(const WFC::Vector3i& v) const { return v.GetSTLHashcode(); }
};
template<> struct std::hash<WFC::Region3i>
{
	size_t operator()(const WFC::Region3i& r) const {
		size_t x = r.MinInclusive.GetSTLHashcode(),
			   y = r.MaxExclusive.GetSTLHashcode();
		return x ^ y; //TODO: Probably a better way to mix bits.
	}
};