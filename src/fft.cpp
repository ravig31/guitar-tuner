#include <cmath>
#include <config.hpp>
#include <stdexcept>

#include "fft.hpp"

FFT::FFT(unsigned long frameSize)
	: frameSize_(frameSize)
	, inputBuffer_(frameSize)
	, outputBuffer_(frameSize)
	, plan_(fftw_plan_r2r_1d(
		  frameSize_,
		  inputBuffer_.data(),
		  outputBuffer_.data(),
		  FFTW_R2HC,
		  FFTW_ESTIMATE
	  ))
{
	if (!plan_)
	{
		throw std::runtime_error("FFTW plan creation failed.");
	}
}

FFT::~FFT() { fftw_destroy_plan(plan_); }

void FFT::execute(const std::vector<double>& input, std::vector<double>& output)
{
	if (input.size() != frameSize_ || output.size() != frameSize_)
	{
		throw std::invalid_argument("Input or output buffer size mismatch.");
	}
	std::copy(input.begin(), input.end(), inputBuffer_.begin());
	fftw_execute(plan_);
	std::copy(outputBuffer_.begin(), outputBuffer_.end(), output.begin());
}

unsigned long FFT::calculateStartIndex(double freqStart) const
{
	double sampleRatio = frameSize_ / SAMPLE_RATE;
	return static_cast<unsigned long>(std::ceil(sampleRatio * freqStart));
}

unsigned long FFT::calculateSpectroSize(double freqEnd) const
{
	double sampleRatio = frameSize_ / SAMPLE_RATE;
	return static_cast<unsigned long>(
			   std::min(std::ceil(sampleRatio * freqEnd), static_cast<double>(frameSize_ / 2.0))
		   ) -
		calculateStartIndex(SPECTRO_FREQ_START);
}