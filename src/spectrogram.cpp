#include "spectrogram.hpp"
#include <cmath>
#include <cstdio>

void Spectrogram::display(
	const std::vector<double>& spectrum,
	int startIndex,
	int spectroSize,
	int displaySize
) const
{
	printf("\r");
	for (int i = 0; i < displaySize; ++i)
	{
		// double proportion = std::pow(static_cast<double>(i) / displaySize, 4);
		double proportion = static_cast<double>(i) / displaySize;
		size_t index = static_cast<size_t>(startIndex + proportion * spectroSize);
		double freq = spectrum[index];
		if (freq < 0.125)
		{
			printf(" ");
		}
		else if (freq < 0.25)
		{
			printf("▂");
		}
		else if (freq < 0.375)
		{
			printf("▃");
		}
		else if (freq < 0.5)
		{
			printf("▄");
		}
		else if (freq < 0.625)
		{
			printf("▅");
		}
		else if (freq < 0.75)
		{
			printf("▆");
		}
		else if (freq < 0.875)
		{
			printf("▇");
		}
		else
		{
			printf("█");
		}
	}
	fflush(stdout);
}