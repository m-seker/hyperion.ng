#pragma once

// Hyperion-Qt includes
#include <grabber/ObsGrabber.h>

//Utils includes
#include <utils/VideoMode.h>

class ObsWrapper : public QObject
{
	Q_OBJECT
public:
	ObsWrapper(int grabInterval, int cropLeft, int cropRight, int cropTop, int cropBottom, int pixelDecimation);

signals:
	void sig_screenshot(const Image<ColorRgb> & screenshot);

public slots:
	///
	/// Set the video mode (2D/3D)
	/// @param[in] mode The new video mode
	///
	void setVideoMode(VideoMode videoMode);

private:
	/// The grabber for creating screenshots
	ObsGrabber _grabber;
};
