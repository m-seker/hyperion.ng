#pragma once

#include <utils/ColorRgb.h>
#include <hyperion/Grabber.h>

class AudioPacket;
class Logger;

class JackAudioException : public std::exception {
public:
	inline virtual const char* what() const throw() override {
		return "JackAudioException";
	}
};

class JackGrabber : public Grabber
{
	Q_OBJECT
public:
	JackGrabber();
	~JackGrabber() override;

//	bool Setup();
//	int grabFrame(Image<ColorRgb> & image, bool forceUpdate=false);

signals:
	void systemAudio(const QString& name, const AudioPacket& audioPacket);

private:
	static const int64_t START_DELAY;

//	bool setupResources();
//	void freeResources();

	Logger * _logger;
};

