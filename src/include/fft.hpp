#pragma once

#include <fftw3.h>

#include <vector>

class FFT
{
  public:
	FFT(unsigned long frameSize);
	~FFT();

	void execute(const std::vector<double>& input, std::vector<double>& output);

	unsigned long calculateStartIndex(double freqStart) const;
	unsigned long calculateSpectroSize(double freqEnd) const;

  private:
	unsigned long frameSize_;
	std::vector<double> inputBuffer_;
	std::vector<double> outputBuffer_;
	fftw_plan plan_;
};
