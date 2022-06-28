#pragma once

#include "WFCMath.h"
#include "Vector2i.h"

#pragma warning(disable: 4018)


namespace WFC
{
	template<class ArrayType>
	//Wraps a contiguous heap-allocated one-dimensional array
	//    so it can be treated like a two-dimensional array.
	//The most cache-efficient way to iterate through this array is through
	//    the Y in the outer loop and then the X in the inner loop.
	class Array2D
	{
	public:

        //Creates an invalid Array2D with 0 elements.
        Array2D() : width(0), height(0), arrayVals(nullptr) { }

		//Creates a new Array2D without initializing any of the values.
		Array2D(int aWidth, int aHeight)
		{
			width = aWidth;
			height = aHeight;

			arrayVals = new ArrayType[(size_t)GetNumbElements()];
		}
		//Creates a new Array2D without initializing any of the values.
		Array2D(Vector2i size) : Array2D(size.x, size.y) { }

		Array2D(int aWidth, int aHeight, const ArrayType& defaultValue)
		{
			width = aWidth;
			height = aHeight;

			arrayVals = new ArrayType[GetNumbElements()];
			for (int i = 0; i < GetNumbElements(); ++i)
				arrayVals[i] = defaultValue; //MSVC warns this is a buffer overflow, but it's not?
		}
		Array2D(Vector2i size, const ArrayType& defaultValue) : Array2D(size.x, size.y, defaultValue) { }

		//Move operator.
		Array2D(Array2D&& toMove) : arrayVals(nullptr) { *this = std::move(toMove); }
		Array2D& operator=(Array2D&& toMove)
		{
			if (arrayVals != nullptr)
				delete[] arrayVals;

			width = toMove.width;
			height = toMove.height;
			arrayVals = toMove.arrayVals;

			toMove.width = 0;
			toMove.height = 0;
			toMove.arrayVals = nullptr;

			return *this;
		}

		//Copy operator.
		Array2D(const Array2D<ArrayType>& copy) { *this = copy; }
        Array2D& operator=(const Array2D<ArrayType>& other)
        {
            if (arrayVals != nullptr)
                delete[] arrayVals;

            width = other.width;
            height = other.height;
            arrayVals = new ArrayType[GetNumbElements()];

            size_t size = (size_t)(GetNumbElements());
            for (size_t i = 0; i < size; ++i)
                arrayVals[i] = other.arrayVals[i];

            return *this;
        }

		~Array2D()
		{
			if (arrayVals != nullptr)
			{
				delete[] arrayVals;
			}
		}


		ArrayType& operator[](Vector2i l)
        {
            assert(Region2i(GetDimensions()).Contains(l));
            return arrayVals[GetIndex(l.x, l.y)];
        }
		const ArrayType& operator[](Vector2i l) const
        {
            assert(Region2i(GetDimensions()).Contains(l));
            return arrayVals[GetIndex(l.x, l.y)];
        }


		//Gets the X size of this array.
		inline int GetWidth() const { return width; }
		//Gets the Y size of this array.
        inline int GetHeight() const { return height; }
		//Gets the size of this array along each axis.
		Vector2i GetDimensions() const { return Vector2i(width, height); }


		//Resets this array to the given size and leaves its elements uninitialized.
		//If the total number of elements doesn't change, then nothing is allocated or un-allocated
		//    and the elements keep their values.
		void Reset(int _width, int _height)
		{
			//Only resize if the current array does not have the same number of elements.
			if ((GetNumbElements()) != (_width * _height))
			{
				if (arrayVals != nullptr)
				    delete[] arrayVals;

				arrayVals = new ArrayType[_width * _height];
			}

			width = _width;
			height = _height;
		}
		//Resets this array to the given size and initializes all elements to the given value.
		void Reset(int _width, int _height, const ArrayType& newValues)
		{
			Reset(_width, _height);
			Fill(newValues);
		}


		//Gets the array index for the given position.
		int GetIndex(int x, int y) const
		{
			return x + (y * width);
		}
		//Gets the location in this array that corresponds to the given array index.
		Vector2i GetLocation(int index) const
		{
			return Vector2i(index % width, index / width);
		}


		//Wraps the given index around the range of allowable indices for this array.
		Vector2i Wrap(Vector2i in) const
		{
            in.x = Math::PositiveModulo(in.x, width);
            in.y = Math::PositiveModulo(in.y, height);
			return in;
		}


		//Gets the total number of elements contained by this array.
		int GetNumbElements() const { return width * height; }


		//Fills every element with the given value.
		void Fill(const ArrayType& value)
		{
			int size = GetNumbElements();
			for (int i = 0; i < size; ++i)
				arrayVals[i] = value;
		}
		//Copies the given elements to this array.
		//Assumes that the size of this array matches with the given one.
		//If "useMemcpy" is true, this array will have its exact binary data copied quickly using memcpy.
		//Otherwise, each element will be set using its assignment operator.
		void Fill(const ArrayType* values, bool useMemcpy)
		{
			if (useMemcpy)
				memcpy(arrayVals, values, GetNumbElements() * sizeof(ArrayType));

			else for (int i = 0; i < (GetNumbElements()); ++i)
				arrayVals[i] = values[i];
		}

		//A function with signature "void GetValue(Vector2i index, ArrayType* outNewValue)".
		template<typename Func>
		//Fills every element using the given function.
		void FillFunc(Func getValue)
		{
			Vector2i loc;
			for (loc.y = 0; loc.y < height; ++loc.y)
				for (loc.x = 0; loc.x < width; ++loc.x)
					getValue(loc, &arrayVals[GetIndex(loc.x, loc.y)]);
		}

		//Sets the given array to be a rotated version of this array.
		//The number of 90-degree rotations is specified by "clockwiseRots".
		//"useFastCopy" determines whether to use memcpy on the whole thing (faster)
		//    or the assignment operator on each element (slower).
		//The given array will be automatically resized to match this one's size if needed.
		void RotateInto(int clockwiseRots, Array2D<ArrayType>& outArray, bool useFastCopy) const
		{
			//Wrap the value to the range [0, 3].
			//Annoyingly, % operator doesn't work as expected for negative integers.
			while (clockwiseRots < 0) clockwiseRots += 1024;
			clockwiseRots %= 4;

			const Array2D<ArrayType> * thisA = this;

			switch (clockwiseRots)
			{
				case 0:
					outArray.Reset(width, height);
					outArray.Fill(arrayVals, useFastCopy);
					break;

				case 1:
					outArray.Reset(height, width);
					outArray.FillFunc([thisA](Vector2i loc, ArrayType * outValue)
					{
						Vector2i locF(thisA->GetHeight() - 1 - loc.y, loc.x);
						const ArrayType * value = &thisA->operator[](locF);
						*outValue = *value;
					});
					break;

				case 2:
					outArray.Reset(height, width);
					outArray.FillFunc([thisA](Vector2i loc, ArrayType * outValue)
					{
						*outValue = thisA->operator[](Vector2i(thisA->GetWidth() - 1 - loc.x,
															   thisA->GetHeight() - 1 - loc.y));
					});
					break;

				case 3:
					outArray.Reset(height, width);
					outArray.FillFunc([thisA](Vector2i loc, ArrayType * outValue)
					{
						*outValue = thisA->operator[](Vector2i(loc.y, thisA->GetWidth() - 1 - loc.x));
					});
					break;

				default: assert(false);
			}
		}


		//Gets a pointer to the first element in this array.
		const ArrayType* GetArray() const { return arrayVals; }
		//Gets a pointer to the first element in this array.
		ArrayType* GetArray() { return arrayVals; }
    
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


	private:

		int width, height;
		ArrayType* arrayVals;
	};
}

#pragma warning(default: 4018)