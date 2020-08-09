#pragma once

#include <QObject>

// Hyperion-utils includes
#include <utils/ColorRgb.h>
#include <hyperion/Grabber.h>

///
/// @brief The platform capture implementation based on QT API
///
class ObsGrabber : public Grabber
{
	Q_OBJECT

public:

	ObsGrabber(int cropLeft, int cropRight, int cropTop, int cropBottom, int pixelDecimation);

	~ObsGrabber() override;

	///
	/// @brief Set a new video mode
	///
	void setVideoMode(VideoMode mode) override;

	///
	/// @brief Apply new width/height values, overwrite Grabber.h implementation as qt doesn't use width/height, just pixelDecimation to calc dimensions
	///
	bool setWidthHeight(int width, int height) override { return true; };

	///
	/// @brief Apply new pixelDecimation
	///
	void setPixelDecimation(int pixelDecimation) override;

	///
	/// Set the crop values
	/// @param  cropLeft    Left pixel crop
	/// @param  cropRight   Right pixel crop
	/// @param  cropTop     Top pixel crop
	/// @param  cropBottom  Bottom pixel crop
	///
	void setCropping(unsigned cropLeft, unsigned cropRight, unsigned cropTop, unsigned cropBottom) override;

signals:
	void onNewFrame(const Image<ColorRgb>& image);

private:
	///
	/// @brief Is called whenever we need new screen dimension calculations based on window geometry
	///
	int updateScreenDimensions(bool force);

	///
	/// @brief free the _screen pointer
	///
	void freeResources();

private:

	int _pixelDecimation;
	unsigned _screenWidth;
	unsigned _screenHeight;
	unsigned _src_x;
	unsigned _src_y;
	unsigned _src_x_max;
	unsigned _src_y_max;
};
