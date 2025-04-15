#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <portaudio.h>

constexpr double SAMPLE_RATE = 44100;
constexpr unsigned long FRAMES_PER_BUFFER = 512;


static void checkErr(PaError err)
{
	if (err != paNoError)
	{
		std::cout << "PortAudio error: " << Pa_GetErrorText(err) << '\n';
		exit(EXIT_FAILURE);
	}
}

static int paTestCallBack(const void* inputBuffer, void* outputBuffer,
						  unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags, void* userData)
{
	float* in = (float*) inputBuffer;
	(void) outputBuffer;

	int displaySize = 100;
	std::cout << "\r";

	float vol_l = 0;
	float vol_r = 0;

	for (unsigned long i=0; i < framesPerBuffer * 2; i += 2){
		vol_l = std::max(vol_l, abs(in[i]));
		vol_r = std::max(vol_r, abs(in[i+1]));
	}

	for (int i=0 ;i < displaySize; i++){
		float barProportion = i / float(displaySize);
		if (barProportion <= vol_l && barProportion <= vol_r){
			std::cout << "█";
		} else if (barProportion <= vol_l) {
			std::cout << "▀";
		} else if (barProportion <= vol_r) {
			std::cout << "▄";
		} else {
			std::cout << " ";
		}
	}

	fflush(stdout);

	return 0;
}

int main()
{
	PaError err;
	err = Pa_Initialize();
	checkErr(err);

	int devCount = Pa_GetDeviceCount();
	if (devCount < 0)
	{
		std::cerr << "Error getting device count.\n";
		exit(EXIT_FAILURE);
	}
	else if (devCount == 0)
	{
		std::cout << "No devices found!\n";
		exit(EXIT_SUCCESS);
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

	int inputDevice =  1;
	int outputDevice = 2;

	PaStreamParameters inputParams;
	PaStreamParameters outputParams;

	memset(&inputParams, 0, sizeof(inputParams));
	inputParams.channelCount = Pa_GetDeviceInfo(inputDevice)->maxInputChannels;
	inputParams.device = inputDevice;
	inputParams.hostApiSpecificStreamInfo = NULL;
	inputParams.sampleFormat = paFloat32;
	inputParams.suggestedLatency = Pa_GetDeviceInfo(inputDevice)->defaultLowInputLatency;

	memset(&outputParams, 0, sizeof(outputParams));
	outputParams.channelCount = Pa_GetDeviceInfo(outputDevice)->maxOutputChannels;
	outputParams.device = outputDevice;
	outputParams.hostApiSpecificStreamInfo = NULL;
	outputParams.sampleFormat = paFloat32;
	outputParams.suggestedLatency = Pa_GetDeviceInfo(outputDevice)->defaultLowOutputLatency;

	PaStream* stream;
	err = Pa_OpenStream(&stream, &inputParams, &outputParams, SAMPLE_RATE, FRAMES_PER_BUFFER,
						paNoFlag, paTestCallBack, nullptr);
	checkErr(err);


	err = Pa_StartStream(stream);
	checkErr(err);

	Pa_Sleep(10*1000);

	err = Pa_StopStream(stream);
	checkErr(err);

	err = Pa_CloseStream(stream);
	checkErr(err);

	err = Pa_Terminate();
	checkErr(err);
	return EXIT_SUCCESS;
}