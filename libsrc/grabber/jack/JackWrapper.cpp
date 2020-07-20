#include <grabber/JackWrapper.h>

JackWrapper::JackWrapper(const unsigned updateRate_Hz)
	: GrabberWrapper("Jack", &_grabber, 0, 0, updateRate_Hz)
	, _grabber()
	, _init(false)
{
	connect(&_grabber, &JackGrabber::systemAudio, this, &GrabberWrapper::systemAudio);
}

JackWrapper::~JackWrapper()
{
}

void JackWrapper::action()
{
}
