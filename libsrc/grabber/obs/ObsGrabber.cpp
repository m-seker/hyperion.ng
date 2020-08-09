#include <grabber/ObsGrabber.h>

#include <iostream>

ObsGrabber *instance = nullptr;

extern "C"
{
#include <obs-module.h>

Image<ColorRgb> image;

void hyperion_receive_video(void *data, struct video_data *frame)
{
	//struct hyperion_output *context = data;

	memcpy(image.memptr(), frame->data[0], frame->linesize[0] * image.height());

	if (instance)
		instance->onNewFrame(image);
}

void set_image_size(uint32_t width, uint32_t height)
{
	image.resize(width, height);
}
}

ObsGrabber::ObsGrabber(int cropLeft, int cropRight, int cropTop, int cropBottom, int pixelDecimation)
	: Grabber("ObsGrabber", 0, 0, cropLeft, cropRight, cropTop, cropBottom)
	, _pixelDecimation(pixelDecimation)
	, _screenWidth(0)
	, _screenHeight(0)
	, _src_x(0)
	, _src_y(0)
	, _src_x_max(0)
	, _src_y_max(0)
{
	_useImageResampler = false;
}

ObsGrabber::~ObsGrabber()
{
	freeResources();
}

void ObsGrabber::freeResources()
{
}

int ObsGrabber::updateScreenDimensions(bool force)
{
	return 1;
}

void ObsGrabber::setVideoMode(VideoMode mode)
{
	Grabber::setVideoMode(mode);
	updateScreenDimensions(true);
}

void ObsGrabber::setPixelDecimation(int pixelDecimation)
{
	_pixelDecimation = pixelDecimation;
}

void ObsGrabber::setCropping(unsigned cropLeft, unsigned cropRight, unsigned cropTop, unsigned cropBottom)
{
	Grabber::setCropping(cropLeft, cropRight, cropTop, cropBottom);
	updateScreenDimensions(true);
}
