#include <grabber/PulseWrapper.h>

PulseWrapper::PulseWrapper(const unsigned updateRate_Hz)
	: GrabberWrapper("PULSE", &_grabber, 0, 0, updateRate_Hz)
	, _grabber()
	, _init(false)
{
	connect(&_grabber, &PulseGrabber::systemAudio, this, &GrabberWrapper::systemAudio);
}

PulseWrapper::~PulseWrapper()
{
}

void PulseWrapper::action()
{
}
