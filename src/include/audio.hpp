
#pragma once

#include <portaudio.h>

class AudioIO
{
  public:
	AudioIO();
	~AudioIO();

	void initialize();
	void openStream(PaStreamCallback* callback, void* userData);
	void startStream();
	void stopStream();
	void closeStream();
	void terminate();
    void displayDeviceInfo();

	int getInputDeviceIndex() const { return inputDeviceIndex_; }
	int getOutputDeviceIndex() const { return outputDeviceIndex_; }

  private:
	PaStream* stream_;
	int inputDeviceIndex_;
	int outputDeviceIndex_;

	static void checkPaError(PaError err);
};
