#include "audio.hpp"
#include "config.hpp"
#include "fft.hpp"
#include "spectrogram.hpp"
#include <iostream>
#include <vector>

struct StreamCallBackData
{
	StreamCallBackData()
		: fft(FRAMES_PER_BUFFER)
		, spectrogram()
	{
	}

	std::vector<double> inputBuffer;
	std::vector<double> spectrumBuffer;
	FFT fft;
	Spectrogram spectrogram;
	unsigned long startIndex;
	unsigned long spectroSize;
};

static int streamCallback(
	const void* inputBuffer,
	void* outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData
)
{
	auto* callbackData = static_cast<StreamCallBackData*>(userData);
	const auto* input = static_cast<const float*>(inputBuffer);
	(void)outputBuffer; // Suppress unused variable warning

	if (callbackData->inputBuffer.size() != framesPerBuffer)
	{
		callbackData->inputBuffer.resize(framesPerBuffer);
		callbackData->spectrumBuffer.resize(framesPerBuffer);
	}

	for (unsigned long i = 0; i < framesPerBuffer; ++i)
	{
		callbackData->inputBuffer[i] = static_cast<double>(input[i * NUM_CHANNELS]);
	}

	callbackData->fft.execute(callbackData->inputBuffer, callbackData->spectrumBuffer);
	callbackData->spectrogram.display(
		callbackData->spectrumBuffer,
		callbackData->startIndex,
		callbackData->spectroSize,
		DISPLAY_SIZE
	);

	return paContinue;
}

int main()
{
	AudioIO audioIO;
	StreamCallBackData callbackData;

	try
	{
		audioIO.initialize();
		audioIO.displayDeviceInfo();
		
		callbackData.startIndex = callbackData.fft.calculateStartIndex(SPECTRO_FREQ_START);
		callbackData.spectroSize = callbackData.fft.calculateSpectroSize(SPECTRO_FREQ_END);

		audioIO.openStream(streamCallback, &callbackData);
		audioIO.startStream();

		std::cout << "Spectrogram running for 30 seconds..." << std::endl;
		Pa_Sleep(30 * 1000);

		audioIO.stopStream();
		audioIO.closeStream();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception caught: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << std::endl;
	return EXIT_SUCCESS;
}