#include <grabber/AlsaWrapper.h>

AlsaWrapper::AlsaWrapper(const unsigned updateRate_Hz)
	: GrabberWrapper("ALSA", &_grabber, 0, 0, updateRate_Hz)
	, _grabber()
	, _init(false)
{
	connect(&_grabber, &AlsaGrabber::systemAudio, this, &GrabberWrapper::systemAudio);
}

AlsaWrapper::~AlsaWrapper()
{
}

void AlsaWrapper::action()
{
}
