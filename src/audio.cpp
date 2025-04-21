#include <cstring>
#include <iostream>

#include "audio.hpp"
#include "config.hpp" // Include if you use constants here

AudioIO::AudioIO()
	: stream_(nullptr)
	, inputDeviceIndex_(INPUT_DEVICE_INDEX)
	, outputDeviceIndex_(OUTPUT_DEVICE_INDEX)
{
}

AudioIO::~AudioIO()
{
	if (stream_)
	{
		stopStream();
		closeStream();
	}
	terminate();
}

void AudioIO::initialize()
{
	PaError err = Pa_Initialize();
	checkPaError(err);
}

void AudioIO::openStream(PaStreamCallback* callback, void* userData)
{
	PaStreamParameters inputParams;
	PaStreamParameters outputParams;

	const PaDeviceInfo* inputDeviceInfo = Pa_GetDeviceInfo(inputDeviceIndex_);
	if (!inputDeviceInfo)
	{
		throw std::runtime_error("Invalid input device index.");
	}

	const PaDeviceInfo* outputDeviceInfo = Pa_GetDeviceInfo(outputDeviceIndex_);
	if (!outputDeviceInfo)
	{
		throw std::runtime_error("Invalid output device index.");
	}

	std::cout << "Using Input Device: " << inputDeviceInfo->name << std::endl;
	std::cout << "Using Output Device: " << outputDeviceInfo->name << std::endl;

	std::memset(&inputParams, 0, sizeof(inputParams));
	inputParams.channelCount = inputDeviceInfo->maxInputChannels;
	inputParams.device = inputDeviceIndex_;
	inputParams.hostApiSpecificStreamInfo = nullptr;
	inputParams.sampleFormat = paFloat32;
	inputParams.suggestedLatency = inputDeviceInfo->defaultLowInputLatency;

	std::memset(&outputParams, 0, sizeof(outputParams));
	outputParams.channelCount = outputDeviceInfo->maxOutputChannels;
	outputParams.device = outputDeviceIndex_;
	outputParams.hostApiSpecificStreamInfo = nullptr;
	outputParams.sampleFormat = paFloat32;
	outputParams.suggestedLatency = outputDeviceInfo->defaultLowOutputLatency;

	PaError err = Pa_OpenStream(
		&stream_,
		&inputParams,
		&outputParams,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paNoFlag,
		callback,
		userData
	);
	checkPaError(err);
}

void AudioIO::startStream()
{
	PaError err = Pa_StartStream(stream_);
	checkPaError(err);
}

void AudioIO::stopStream()
{
	if (stream_)
	{
		PaError err = Pa_StopStream(stream_);
		checkPaError(err);
	}
}

void AudioIO::closeStream()
{
	if (stream_)
	{
		PaError err = Pa_CloseStream(stream_);
		checkPaError(err);
		stream_ = nullptr;
	}
}

void AudioIO::terminate()
{
	PaError err = Pa_Terminate();
	checkPaError(err);
}


void AudioIO::displayDeviceInfo(){
    int devCount = Pa_GetDeviceCount();

	if (devCount < 0)
	{
		std::cerr << "Error getting device count.\n";
        throw std::runtime_error("PortAudio error");
	}
	else if (devCount == 0)
	{
		std::cout << "No devices found!\n";
		throw std::runtime_error("PortAudio error");
	}

	const PaDeviceInfo* deviceInfo;
	for (int i = 0; i < devCount; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(i);
		std::cout << "Device: " << i << '\n';
		std::cout << "  name: " << deviceInfo->name << '\n';
		std::cout << "  maxInputChannels: " << deviceInfo->maxInputChannels << '\n';
		std::cout << "  maxOutputChannels: " << deviceInfo->maxOutputChannels << '\n';
		std::cout << "  defaultSampleRate: " << deviceInfo->defaultSampleRate << '\n';
	}
}

void AudioIO::checkPaError(PaError err)
{
	if (err != paNoError)
	{
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << '\n';
		throw std::runtime_error("PortAudio error");
	}
}