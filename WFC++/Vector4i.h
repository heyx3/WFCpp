#pragma once

#include <assert.h>
#include <iterator>
#include <math.h>

#include "Platform.h"
#include "EnumFlags.h"
#include "Vector3i.h"


namespace WFC
{
	//A 4D integer coordinate.
	struct WFC_API Vector4i
	{
	public:
        static inline Vector4i Zero() { return Vector4i(); }


		int x, y, z, w;


		Vector4i() : Vector4i(0, 0, 0, 0) { }
		Vector4i(int X, int Y, int Z, int W) : x(X), y(Y), z(Z), w(W) { }
		Vector4i(const Vector3i& xyz, int W) : x(xyz.x), y(xyz.y), z(xyz.z), w(W) { }
		Vector4i(int X, const Vector3i& yzw) : x(X), y(yzw.x), z(yzw.y), w(yzw.z) { }


        //Access each component with 0, 1, 2, or 3.
		const int& operator[](uint_fast8_t index) const { return (&x)[index]; }
        //Access each component with 0, 1, 2, or 3.
		int& operator[](uint_fast8_t index) { return (&x)[index]; }


		Vector4i& operator+=(const Vector4i& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
		Vector4i& operator-=(const Vector4i& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
        Vector4i& operator*=(int i) { x *= i; y *= i; z *= i; return *this; }
        Vector4i& operator/=(int i) { x /= i; y /= i; z *= i; return *this; }

		Vector4i operator+(const Vector4i& other) const { return Vector4i(x + other.x, y + other.y, z + other.z, w + other.w); }
		Vector4i operator-(const Vector4i& other) const { return Vector4i(x - other.x, y - other.y, z - other.z, w - other.w); }
		Vector4i operator*(const Vector4i& other) const { return Vector4i(x * other.x, y * other.y, z * other.z, w * other.w); }
		Vector4i operator/(const Vector4i& other) const { return Vector4i(x / other.x, y / other.y, z / other.z, w / other.w); }

		Vector4i operator+(int i) const { return Vector4i(x + i, y + i, z + i, w + i); }
		Vector4i operator-(int i) const { return Vector4i(x - i, y - i, z - i, w - i); }
		Vector4i operator*(int i) const { return Vector4i(x * i, y * i, z * i, w * i); }
		Vector4i operator/(int i) const { return Vector4i(x / i, y / i, z / i, w / i); }

		Vector4i operator-() const { return Vector4i(-x, -y, -z, -w); }

		bool operator==(const Vector4i& v) const
		{
			static_assert(sizeof(Vector4i) == 4 * sizeof(int),
						  "Vector4i has some byte padding");
			return memcmp(this, &v, sizeof(Vector4i)) == 0;
		}
		bool operator!=(const Vector4i& v) const { return !operator==(v); }


		Vector4i LessX() const { return Vector4i(x - 1, y, z, w); }
		Vector4i MoreX() const { return Vector4i(x + 1, y, z, w); }
        Vector4i LessY() const { return Vector4i(x, y - 1, z, w); }
        Vector4i MoreY() const { return Vector4i(x, y + 1, z, w); }
        Vector4i LessZ() const { return Vector4i(x, y, z - 1, w); }
        Vector4i MoreZ() const { return Vector4i(x, y, z + 1, w); }
		Vector4i LessW() const { return Vector4i(x, y, z, w - 1); }
		Vector4i MoreW() const { return Vector4i(x, y, z, w + 1); }

		uint32_t GetHashcode() const
		{
			auto a = Vector2i(x, y).GetHashcode(),
				 b = Vector2i(z, w).GetHashcode();
			return Vector2i(static_cast<int32_t>(a), static_cast<int32_t>(b)).GetHashcode();
		}
		size_t GetHashcodeLarge() const
		{
			size_t a = Vector2i{ x, y }.GetHashcodeLarge(),
				   b = Vector2i{ z, w }.GetHashcodeLarge();
			//Make sure the bits of 'a' and 'b' are mixed well by re-hashing them.
			size_t c = Vector2i{ static_cast<int32_t>(a >> 32), static_cast<int32_t>(b & 0xffffffff) }.GetHashcodeLarge(),
				   d = Vector2i{ static_cast<int32_t>(a & 0xffffffff), static_cast<int32_t>(b >> 32) }.GetHashcodeLarge();

			return c ^ d;
		}
	};


	//A rectangular region of 4D integer coordinates.
	//This structure can be iterated over in a ranged-based "for" loop.
	struct WFC_API Region4i
	{
	public:

		Vector4i MinInclusive, MaxExclusive;

		explicit Region4i(const Vector4i& maxExclusive)
			: MinInclusive(0, 0, 0, 0), MaxExclusive(maxExclusive) { }
		Region4i(const Vector4i& minInclusive, const Vector4i& maxExclusive)
			: MinInclusive(minInclusive), MaxExclusive(maxExclusive) { }

		bool Contains(const Vector4i& pos) const
		{
			return pos.x >= MinInclusive.x && pos.x < MaxExclusive.x &&
				   pos.y >= MinInclusive.y && pos.y < MaxExclusive.y &&
                   pos.z >= MinInclusive.z && pos.z < MaxExclusive.z &&
				   pos.w >= MinInclusive.w && pos.w < MaxExclusive.w;
		}
		Vector4i GetSize() const { return MaxExclusive - MinInclusive; }

        #pragma region const_iterator
		//An std-style iterator over the region.
		struct WFC_API const_iterator
		{
		public:
			typedef const_iterator self_type;
			typedef Vector4i value_type;
			typedef Vector4i& reference;
			typedef Vector4i* pointer;
			typedef std::forward_iterator_tag iterator_category;
			typedef int difference_type;

			const Region4i& Owner;

			const_iterator(const Region4i& owner, Vector4i _pos = Vector4i())
				: Owner(owner), pos(_pos) { }

			self_type operator++() { auto _this = *this; AdvancePos(); return _this; }
			self_type operator++(int dummy) { AdvancePos(); return *this; }
			const reference operator*() { return pos; }
			const pointer operator->() { return &pos; }
			bool operator==(const self_type& rhs) { return pos == rhs.pos; }
			bool operator!=(const self_type& rhs) { return pos != rhs.pos; }

		private:
			Vector4i pos;

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

						if (pos.z >= Owner.MaxExclusive.z)
						{
							pos.z = Owner.MinInclusive.z;
							pos.w += 1;
						}
                    }
				}
			}
		};
        #pragma endregion

		Region4i::const_iterator begin() const
		{
			return const_iterator(*this, MinInclusive);
		}
		Region4i::const_iterator end() const
		{
			return const_iterator(*this, Vector4i(MinInclusive.x, MinInclusive.y, MinInclusive.z, MaxExclusive.w));
		}
	};
}


template<>
inline WFC::Vector4i WFC::Math::Max<WFC::Vector4i>(Vector4i a, Vector4i b)
{
	Vector4i x;
	for (int i = 0; i < 4; ++i)
		x[i] = Max(a[i], b[i]);
	return x;
}
template<>
inline WFC::Vector4i WFC::Math::Min<WFC::Vector4i>(Vector4i a, Vector4i b)
{
	Vector4i x;
	for (int i = 0; i < 4; ++i)
		x[i] = Min(a[i], b[i]);
	return x;
}

template<> struct std::hash<WFC::Vector4i>
{
	size_t operator()(const WFC::Vector4i& v) const { return v.GetHashcodeLarge(); }
};
template<> struct std::hash<WFC::Region4i>
{
	size_t operator()(const WFC::Region4i& r) const {
		//Hash min and max into 128 bits,
		//    then map those to a Vector4i and hash that.
		size_t a = r.MinInclusive.GetHashcodeLarge(),
			   b = r.MaxExclusive.GetHashcodeLarge();
		return std::hash<WFC::Vector4i>{}({
			static_cast<int32_t>(a >> 32),
			static_cast<int32_t>(a & 0xffffffff),
			static_cast<int32_t>(b >> 32),
			static_cast<int32_t>(b & 0xffffffff)
		});
	}
};