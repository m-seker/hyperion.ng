#include "ObsWrapper.h"

ObsWrapper::ObsWrapper(int grabInterval, int cropLeft, int cropRight, int cropTop, int cropBottom, int pixelDecimation) :
	_grabber(cropLeft, cropRight, cropTop, cropBottom, pixelDecimation)
{
	connect(&_grabber, &ObsGrabber::onNewFrame, this, &ObsWrapper::sig_screenshot);
}

void ObsWrapper::setVideoMode(VideoMode mode)
{
	_grabber.setVideoMode(mode);
}
