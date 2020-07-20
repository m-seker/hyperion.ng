#include <grabber/SoundIOWrapper.h>

SoundIOWrapper::SoundIOWrapper(const unsigned updateRate_Hz)
	: GrabberWrapper("SoundIO", &_grabber, 0, 0, updateRate_Hz)
	, _grabber()
	, _init(false)
{
	connect(&_grabber, &SoundIOGrabber::systemAudio, this, &GrabberWrapper::systemAudio);
}

SoundIOWrapper::~SoundIOWrapper()
{
}

void SoundIOWrapper::action()
{
}
