#include "Pattern.h"
#include "InputData.h"

using namespace WFC;


const Pixel& Pattern::operator[](const Vector2i& patternPos) const
{
	Vector2i inputPos = InputDataRegion.MinInclusive + patternPos;
	return input->GetPixel(inputPos, InputDataTransform);
}

size_t Pattern::GetHashCode() const
{
	const unsigned int prime = 59;
	unsigned int hash = 1;
	for (Vector2i sourcePixelPos : InputDataRegion)
	{
		Pixel pixel = input->GetPixel(sourcePixelPos, InputDataTransform);
		hash = (hash * prime) + pixel;
	}

    return hash;
}

bool Pattern::HasSameData(const Pattern& otherPattern) const
{
	if (InputDataRegion.GetSize() != otherPattern.InputDataRegion.GetSize())
		return false;

	for (Vector2i patternPos : Region2i(InputDataRegion.GetSize()))
	{
		Pixel myPixel = (*this)[patternPos],
			  theirPixel = otherPattern[patternPos];
		if (myPixel != theirPixel)
			return false;
	}

	return true;
}

bool Pattern::DoesFit(Vector2i outputMinCorner,
					  std::function<Nullable<Pixel>(Vector2i)> getOutputPixel) const
{
	for (Vector2i patternPos : Region2i(InputDataRegion.GetSize()))
	{
		Vector2i inputPos = patternPos + InputDataRegion.MinInclusive,
				 outputPos = patternPos + outputMinCorner;

		Pixel inputPixel = input->GetPixel(inputPos, InputDataTransform);
		Nullable<Pixel> outputPixel = getOutputPixel(outputPos);

		if (outputPixel.HasValue && outputPixel.Value != inputPixel)
			return false;
	}

	return true;
}