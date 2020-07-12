// STL includes
#include <chrono>
#include <iostream>

// Qt includes
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

// Hyperion includes
#include <utils/Image.h>

#include <gtest/gtest.h>

using namespace testing;

void createScreenshot(const int cropHorizontal, const int cropVertical, const int decimation, Image<ColorRgb> & image)
{
	int argc      = {};
	char * argv[] = {};
	QApplication app(argc, argv);

	// Create the full size screenshot
	QScreen *screen = QApplication::primaryScreen();
	const QRect screenSize = screen->availableGeometry();
	const int croppedWidth  = screenSize.width()  - 2 * cropVertical;
	const int croppedHeight = screenSize.height() - 2 * cropHorizontal;
	const QPixmap fullSizeScreenshot = screen->grabWindow(QApplication::desktop()->winId(), cropVertical, cropHorizontal, croppedWidth, croppedHeight);

	// Scale the screenshot to the required size
	const int width  = fullSizeScreenshot.width()/decimation;
	const int height = fullSizeScreenshot.height()/decimation;
	const QPixmap scaledScreenshot = fullSizeScreenshot.scaled(width, height, Qt::IgnoreAspectRatio, Qt::FastTransformation);

	// Convert the QPixmap to QImage in order to get out RGB values
	const QImage qImage = scaledScreenshot.toImage();

	// Make sure that the output image has the right size
	image.resize(width, height);

	// Copy the data into the output image
	for (int y = 0; y < qImage.height(); ++y)
	{
		for (int x = 0; x < qImage.width(); ++x)
		{
			// Get the pixel at [x;y] (format int32 #AARRGGBB)
			const QRgb inPixel = qImage.pixel(x,y);

			// Copy the color channels into the output pixel
			ColorRgb & outPixel = image(x,y);
			outPixel.red   = (inPixel & 0x00ff0000) >> 16;
			outPixel.green = (inPixel & 0x0000ff00) >>  8;
			outPixel.blue  = (inPixel & 0x000000ff);
		}
	}

}

TEST(TestQtScreenshot, TestScreenshotTimeIsLessThanMaximumAllowedTime)
{
	const int EVALUATION_COUNT		 = 100;
	const int MAX_ALLOWED_SCREENSHOT_TIME    = 100;
	const int DECIMATION			 = 10;

	Image<ColorRgb> screenshot(64,64);

	const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for (int i = 0; i < EVALUATION_COUNT; ++i)
	{
		createScreenshot(0,0, DECIMATION, screenshot);
	}

	const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	const int duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

	ASSERT_LE(duration/EVALUATION_COUNT, MAX_ALLOWED_SCREENSHOT_TIME)
		<< "Taking screenshot takes more time than maximum allowed value";

	std::cout << "Time required for single screenshot: " << duration/EVALUATION_COUNT << "ms" << std::endl;
}

