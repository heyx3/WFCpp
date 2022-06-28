#pragma once

#include "WFCMath.h"
#include "Vector4i.h"

#pragma warning(disable: 4018)


namespace WFC
{
	//TODO: Check out this library: https://www.cs.cornell.edu/~saeed/lite/html/group__array.html
	//TODO: Use vector<> for backing, or else things like default-construction won't be handled right.

	template<class ArrayType>
	//Wraps a contiguous heap-allocated one-dimensional array
	//    so it can be treated like a four-dimensional array.
	//The most cache-efficient way to iterate through this array is through
	//    the W in the outer loop, and the X in the inner loop.
	class Array4D
	{
	public:

        //Creates an invalid Array4D with 0 elements.
        Array4D() : size(0, 0, 0, 0), arrayVals(nullptr) { }

		//Creates a new Array4D without initializing any of the values.
		Array4D(const Vector4i& size) : size(size), arrayVals(new ArrayType[(size_t)size.x * size.y * size.z * size.w]) { }
		//Creates a new Array4D and fills it with copies of the given element.
		Array4D(const Vector4i& size, const ArrayType& defaultValue)
			: Array4D(size)
		{
			for (int i = 0; i < GetNumbElements(); ++i)
				arrayVals[i] = defaultValue;
		}

		//Move operator.
		Array4D(Array4D&& toMove) : arrayVals(nullptr) { *this = std::move(toMove); }
		Array4D& operator=(Array4D&& toMove)
		{
			if (arrayVals != nullptr)
				delete[] arrayVals;

			size = toMove.size;
			arrayVals = toMove.arrayVals;

			toMove.size = Vector4i{};
			toMove.arrayVals = nullptr;

			return *this;
		}

		//Copy operator.
		Array4D(const Array4D<ArrayType>& copy) { *this = copy; }
        Array4D& operator=(const Array4D<ArrayType>& other)
        {
            if (arrayVals != nullptr)
                delete[] arrayVals;

			size = other.size;
            arrayVals = new ArrayType[GetNumbElements()];

            for (size_t i = 0; i < (size_t)GetNumbElements(); ++i)
                arrayVals[i] = other.arrayVals[i];

            return *this;
        }

		~Array4D()
		{
			if (arrayVals != nullptr)
			{
				delete[] arrayVals;
			}
		}


		ArrayType& operator[](const Vector4i& l)
        {
            assert(Region4i(GetDimensions()).Contains(l));
            return arrayVals[GetIndex(l.x, l.y, l.z, l.w)];
        }
		const ArrayType& operator[](const Vector4i& l) const
        {
            assert(Region4i(GetDimensions()).Contains(l));
            return arrayVals[GetIndex(l.x, l.y, l.z, l.w)];
        }


		//Gets the size of this array along each axis.
		Vector4i GetDimensions() const { return size; }


		//Resets this array to the given size and leaves its elements uninitialized.
		//If the total number of elements doesn't change, then nothing is allocated or un-allocated
		//    and the elements keep their values.
		void Reset(const Vector4i& newSize)
		{
			//Only resize if the current array does not have the same number of elements.
			if (GetNumbElements() != (newSize.x * newSize.y * newSize.z * newSize.w))
			{
				if (arrayVals != nullptr)
				    delete[] arrayVals;

				arrayVals = new ArrayType[newSize.x * newSize.y * newSize.z * newSize.w];
			}

			size = newSize;
		}
		//Resets this array to the given size and initializes all elements to the given value.
		void Reset(const Vector4i& size, const ArrayType& newValues)
		{
			Reset(size);
			Fill(newValues);
		}


		//Gets the array index for the given position.
		int GetIndex(int x, int y, int z, int w) const
		{
			return x + (y * size.x) + (z * size.x * size.y) + (w * size.x * size.y * size.z);
		}
		//Gets the location in this array that corresponds to the given array index.
		Vector4i GetLocation(int index) const
		{
			return Vector4i(index % size.x,
                            (index / size.x) % size.y,
                            (index / (size.x * size.y)) % size.z,
							index / (size.x * size.y * size.z));
		}


		//Wraps the given index around the range of allowable indices for this array.
		Vector4i Wrap(Vector4i in) const
		{
            in.x = Math::PositiveModulo(in.x, size.x);
            in.y = Math::PositiveModulo(in.y, size.y);
			in.z = Math::PositiveModulo(in.z, size.z);
			in.w = Math::PositiveModulo(in.w, size.w);
			return in;
		}


		//Gets the total number of elements contained by this array.
		int GetNumbElements() const { return size.x * size.y * size.z * size.w; }


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

		//A function with signature "void GetValue(Vector4i index, ArrayType* outNewValue)".
		template<typename Func>
		//Fills every element using the given function.
		void FillFunc(Func getValue)
		{
			Vector4i loc;

			for (Vector4i p : Region4i(GetDimensions()))
				getValue(loc, &(*this)[p]);
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

		Vector4i size;
		ArrayType* arrayVals;
	};
}

#pragma warning(default: 4018)