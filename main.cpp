#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <portaudio.h>
#include <fftw3.h>

constexpr double SAMPLE_RATE = 44100;
constexpr unsigned long FRAMES_PER_BUFFER = 512;
constexpr unsigned long SPECTRO_FREQ_START = 20;
constexpr unsigned long SPECTRO_FREQ_END = 20000;
constexpr unsigned long NUM_CHANNELS = 1;
typedef struct
{
	double* in;
	double* out;
	fftw_plan p;
	int startIndex;
	int spectroSize;
} streamCallBackData;

static streamCallBackData* spectroData;

static void checkErr(PaError err)
{
	if (err != paNoError)
	{
		std::cout << "PortAudio error: " << Pa_GetErrorText(err) << '\n';
		exit(EXIT_FAILURE);
	}
}

static int streamCallBack(const void* inputBuffer, void* outputBuffer,
						  unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo,
						  PaStreamCallbackFlags statusFlags, void* userData)
{
	float* input = (float*)inputBuffer;
	(void)outputBuffer;
	streamCallBackData* callBackData = (streamCallBackData*)userData;

	int dispSize = 100;
	printf("\r");

	for (unsigned long i=0; i < framesPerBuffer; i++){
		callBackData->in[i] = input[i * NUM_CHANNELS];
	}


	fftw_execute(callBackData->p);

	for (int i=0; i < dispSize; i++){
		double proportion = std::pow(i / (double) dispSize, 4);
		double freq = callBackData->out[(int)(callBackData->startIndex + proportion * callBackData->spectroSize)];
		
		if (freq < 0.125) {
            printf("▁");
        } else if (freq < 0.25) {
            printf("▂");
        } else if (freq < 0.375) {
            printf("▃");
        } else if (freq < 0.5) {
            printf("▄");
        } else if (freq < 0.625) {
            printf("▅");
        } else if (freq < 0.75) {
            printf("▆");
        } else if (freq < 0.875) {
            printf("▇");
        } else {
            printf("█");
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

	spectroData = (streamCallBackData*)malloc(sizeof(streamCallBackData));
	spectroData->in = (double*)malloc(sizeof(double) * FRAMES_PER_BUFFER);
	spectroData->out = (double*)malloc(sizeof(double) * FRAMES_PER_BUFFER);

	if (spectroData->in == NULL || spectroData->out == NULL)
	{
		std::cerr << "Could not allocate spectro data\n";
		exit(EXIT_FAILURE);
	}

	spectroData->p = fftw_plan_r2r_1d(FRAMES_PER_BUFFER, spectroData->in, spectroData->out,
									  FFTW_R2HC, FFTW_ESTIMATE);

	double sampleRatio = FRAMES_PER_BUFFER / SAMPLE_RATE;
	spectroData->startIndex = std::ceil(sampleRatio * SPECTRO_FREQ_START);
	spectroData->spectroSize =
		std::min(std::ceil(sampleRatio * SPECTRO_FREQ_END), FRAMES_PER_BUFFER / 2.0) -
		spectroData->startIndex;

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

	int inputDevice = 6;
	int outputDevice = 7;

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
						paNoFlag, streamCallBack, spectroData);
	checkErr(err);

	err = Pa_StartStream(stream);
	checkErr(err);

	Pa_Sleep(30 * 1000);

	err = Pa_StopStream(stream);
	checkErr(err);

	err = Pa_CloseStream(stream);
	checkErr(err);

	err = Pa_Terminate();
	checkErr(err);

	fftw_destroy_plan(spectroData->p);
	fftw_free(spectroData->in);
	fftw_free(spectroData->out);
	fftw_free(spectroData);

	std::cout << std::endl;
	return EXIT_SUCCESS;
}