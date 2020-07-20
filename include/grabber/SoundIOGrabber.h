#pragma once

#include <utils/ColorRgb.h>
#include <hyperion/Grabber.h>

#include <thread>

class AudioPacket;
class Logger;

class SoundIOAudioException : public std::exception {
public:
	inline virtual const char* what() const throw() override {
		return "SoundIOAudioException";
	}
};

class SoundIOGrabber : public Grabber
{
	Q_OBJECT
public:
	SoundIOGrabber();
	~SoundIOGrabber() override;

//	bool Setup();
//	int grabFrame(Image<ColorRgb> & image, bool forceUpdate=false);

signals:
	void systemAudio(const QString& name, const AudioPacket& audioPacket);

private:
	static const int64_t START_DELAY;

	bool setupResources();
	void freeResources();

	Logger * _logger;
	struct SoundIo *_soundio;
        struct SoundIoInStream *_instream;
	struct SoundIoDevice *_in_device;
	std::thread *_listenThread;
};

