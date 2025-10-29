#pragma once

#include "WFCMath.h"
#include "Vector3i.h"

#pragma warning(disable: 4018)


namespace WFC
{
    //TODO: Convert to unsigned ints? Will require Vector2i and Vector3i to be templated.
	//TODO: Use vector<> for backing, or else things like default-construction won't be handled right.

	template<class ArrayType>
	//Wraps a contiguous heap-allocated one-dimensional array
	//    so it can be treated like a three-dimensional array.
	//The most cache-efficient way to iterate through this array is through
	//    the Z in the outer loop, Y in the middle loop, and X in the inner loop.
	class Array3D
	{
	public:

        //Creates an invalid Array3D with 0 elements.
        Array3D() : width(0), height(0), depth(0), arrayVals(nullptr) { }

		//Creates a new Array3D without initializing any of the values.
		Array3D(int aWidth, int aHeight, int aDepth)
		{
			width = aWidth;
			height = aHeight;
            depth = aDepth;

			arrayVals = new ArrayType[GetNumbElements()];
		}
		//Creates a new Array3D without initializing any of the values.
		Array3D(Vector3i size) : Array3D(size.x, size.y, size.z) { }

		Array3D(int aWidth, int aHeight, int aDepth, const ArrayType& defaultValue)
		{
			width = aWidth;
			height = aHeight;
            depth = aDepth;

			arrayVals = new ArrayType[GetNumbElements()];

			for (int i = 0; i < GetNumbElements(); ++i)
				arrayVals[i] = defaultValue;
		}
		Array3D(Vector3i size, const ArrayType& defaultValue) : Array3D(size.x, size.y, size.z, defaultValue) { }

		//Move operator.
		Array3D(Array3D&& toMove) noexcept : arrayVals(nullptr) { *this = std::move(toMove); }
		Array3D& operator=(Array3D&& toMove) noexcept
		{
			if (&toMove == this)
				return *this;

			if (arrayVals != nullptr)
				delete[] arrayVals;

			width = toMove.width;
			height = toMove.height;
            depth = toMove.depth;
			arrayVals = toMove.arrayVals;

			toMove.width = 0;
			toMove.height = 0;
            toMove.depth = 0;
			toMove.arrayVals = nullptr;

			return *this;
		}

		//Copy operator.
		Array3D(const Array3D<ArrayType>& copy) noexcept : arrayVals(nullptr) { *this = copy; }
        Array3D& operator=(const Array3D<ArrayType>& other)
        {
			if (&other == this)
				return *this;

            if (arrayVals != nullptr)
                delete[] arrayVals;

            width = other.width;
            height = other.height;
            depth = other.depth;
            arrayVals = new ArrayType[GetNumbElements()];

            for (size_t i = 0; i < (size_t)GetNumbElements(); ++i)
			{
				WFCPP_ASSERT(i < other.GetNumbElements());
				arrayVals[i] = other.arrayVals[i];
			}

            return *this;
        }

		~Array3D()
		{
			if (arrayVals != nullptr)
			{
				delete[] arrayVals;
			}
		}


		ArrayType& operator[](const Vector3i& l)
        {
			WFCPP_ASSERT(IsIndexValid(l));
            return arrayVals[GetIndex(l.x, l.y, l.z)];
        }
		const ArrayType& operator[](const Vector3i& l) const
        {
			WFCPP_ASSERT(IsIndexValid(l));
            return arrayVals[GetIndex(l.x, l.y, l.z)];
        }


		//Gets the X size of this array.
		inline int GetWidth() const { return width; }
		//Gets the Y size of this array.
        inline int GetHeight() const { return height; }
        //Gets the Z size of this array.
        inline int GetDepth() const { return depth; }
		//Gets the size of this array along each axis.
		Vector3i GetDimensions() const { return Vector3i(width, height, depth); }
		bool IsIndexValid(const Vector3i& idx) const { return Region3i(GetDimensions()).Contains(idx); }


		//Resets this array to the given size and leaves its elements uninitialized.
		//If the total number of elements doesn't change, then nothing is allocated or un-allocated
		//    and the elements keep their values.
		void Reset(int _width, int _height, int _depth)
		{
			//Only resize if the current array does not have the same number of elements.
			if (GetNumbElements() != (_width * _height * _depth))
			{
				if (arrayVals != nullptr)
				    delete[] arrayVals;

				arrayVals = new ArrayType[_width * _height * _depth];
			}

			width = _width;
			height = _height;
            depth = _depth;
		}
		//Resets this array to the given size and initializes all elements to the given value.
		void Reset(int _width, int _height, int _depth, const ArrayType& newValues)
		{
			Reset(_width, _height, _depth);
			Fill(newValues);
		}


		//Gets the array index for the given position.
		int GetIndex(int x, int y, int z) const
		{
			return x + (y * width) + (z * width * height);
		}
		//Gets the location in this array that corresponds to the given array index.
		Vector3i GetLocation(int index) const
		{
			return Vector3i(index % width,
                            (index / width) % height,
                            index / (width * height));
		}


		//Wraps the given index around the range of allowable indices for this array.
		Vector3i Wrap(Vector3i in) const
		{
            in.x = Math::PositiveModulo(in.x, width);
            in.y = Math::PositiveModulo(in.y, height);
            in.z = Math::PositiveModulo(in.z, depth);
			return in;
		}


		//Gets the total number of elements contained by this array.
		int GetNumbElements() const { return width * height * depth; }


		//Fills every element with the given value.
		void Fill(const ArrayType& value)
		{
			for (int i = 0; i < GetNumbElements(); ++i)
				arrayVals[i] = value;
		}
		//Copies the given elements to this array.
		//Assumes that the size of this array matches with the given one.
		//If "useMemcpy" is true, the whole thing will be copied at once with memcpy().
		//Otherwise, each element will be set using its assignment operator.
		void Fill(const ArrayType* values, bool useMemcpy)
		{
			if (useMemcpy)
				memcpy(arrayVals, values, GetNumbElements() * sizeof(ArrayType));
			else for (int i = 0; i < GetNumbElements(); ++i)
				arrayVals[i] = values[i];
		}

		//A function with signature "void GetValue(Vector3i index, ArrayType* outNewValue)".
		template<typename Func>
		//Fills every element using the given function.
		void FillFunc(Func getValue)
		{
			Vector3i loc;
            for (loc.z = 0; loc.z < depth; ++loc.z)
                for (loc.y = 0; loc.y < height; ++loc.y)
				    for (loc.x = 0; loc.x < width; ++loc.x)
					    getValue(loc, &arrayVals[GetIndex(loc.x, loc.y, loc.z)]);
		}

		//Copies this array into the given one using "memcpy", which is as fast as possible.
		//Assumes the given array is the same size as this one.
		void MemCopyInto(ArrayType* outValues) const
		{
			memcpy(outValues, arrayVals, GetNumbElements() * sizeof(ArrayType));
		}
		//Copies this array into the given one using the assignment operator for each value.
		//Assumes it is the same size as this array.
		//Use this instead of "MemCopyInto" if the items are too complex to just copy their byte-data over.
		void CopyInto(ArrayType* outValues) const
		{
			for (int i = 0; i < GetNumbElements(); ++i)
				outValues[i] = arrayVals[i];
		}

		//Gets a pointer to the first element in this array.
		const ArrayType* GetArray() const { return arrayVals; }
		//Gets a pointer to the first element in this array.
		ArrayType* GetArray() { return arrayVals; }
    

	private:

		int width, height, depth;
		ArrayType* arrayVals;
	};
}

#pragma warning(default: 4018)