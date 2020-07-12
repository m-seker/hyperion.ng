#include <QElapsedTimer>

#include <utils/Image.h>
#include <utils/ColorRgb.h>

// X11 includes
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <gtest/gtest.h>

using namespace testing;

const int PIXEL_DECIMATION = 10;

TEST(TestX11Performance, foo_1)
{
	int cropWidth  = 0;
	int cropHeight = 0;

	const char * display_name = nullptr;
	Display * x11Display = XOpenDisplay(display_name);

	std::cout << "Opened display: " << x11Display << std::endl;

	XWindowAttributes window_attributes_return;
	XGetWindowAttributes(x11Display, DefaultRootWindow(x11Display), &window_attributes_return);

	int screenWidth  = window_attributes_return.width;
	int screenHeight = window_attributes_return.height;
	std::cout << "[" << screenWidth << "x" << screenHeight <<"]" << std::endl;

	// Update the size of the buffer used to transfer the screenshot
	int width  = (screenWidth  - 2 * cropWidth  + PIXEL_DECIMATION/2) / PIXEL_DECIMATION;
	int height = (screenHeight - 2 * cropHeight + PIXEL_DECIMATION/2) / PIXEL_DECIMATION;

	const int croppedWidth  = screenWidth  - 2*cropWidth;
	const int croppedHeight = screenHeight - 2*cropHeight;

	QElapsedTimer timer;
	timer.start();

	XImage * xImage = XGetImage(x11Display, DefaultRootWindow(x11Display), cropWidth, cropHeight, croppedWidth, croppedHeight, AllPlanes, ZPixmap);

	std::cout << "Captured image: " << xImage << std::endl;

	// Copy the capture XImage to the local image (and apply required decimation)
	Image<ColorRgb> image(width, height);
	ColorRgb * outputPtr = image.memptr();
	for (int iY=(PIXEL_DECIMATION/2); iY<croppedHeight; iY+=PIXEL_DECIMATION)
	{
		for (int iX=(PIXEL_DECIMATION/2); iX<croppedWidth; iX+=PIXEL_DECIMATION)
		{
			// Extract the pixel from the X11-image
			const uint32_t pixel = uint32_t(XGetPixel(xImage, iX, iY));

			// Assign the color value
			outputPtr->red   = uint8_t((pixel >> 16) & 0xff);
			outputPtr->green = uint8_t((pixel >> 8)  & 0xff);
			outputPtr->blue  = uint8_t((pixel >> 0)  & 0xff);

			// Move to the next output pixel
			++outputPtr;
		}
	}

	// Cleanup allocated resources of the X11 grab
	XDestroyImage(xImage);

	std::cout << "Time required: " << timer.elapsed() << " ms" << std::endl;

	XCloseDisplay(x11Display);
}

TEST(TestX11Performance, foo2)
{
	int cropWidth  = 0;
	int cropHeight = 0;

	const char * display_name = nullptr;
	Display * x11Display = XOpenDisplay(display_name);

	XWindowAttributes window_attributes_return;
	XGetWindowAttributes(x11Display, DefaultRootWindow(x11Display), &window_attributes_return);

	int screenWidth  = window_attributes_return.width;
	int screenHeight = window_attributes_return.height;
	std::cout << "[" << screenWidth << "x" << screenHeight <<"]" << std::endl;

	// Update the size of the buffer used to transfer the screenshot
	int width  = (screenWidth  - 2 * cropWidth  + PIXEL_DECIMATION/2) / PIXEL_DECIMATION;
	int height = (screenHeight - 2 * cropHeight + PIXEL_DECIMATION/2) / PIXEL_DECIMATION;

	const int croppedWidth  = screenWidth  - 2*cropWidth;
	const int croppedHeight = screenHeight - 2*cropHeight;

	QElapsedTimer timer;
	timer.start();

	// Copy the capture XImage to the local image (and apply required decimation)
	Image<ColorRgb> image(width, height);
	ColorRgb * outputPtr = image.memptr();
	for (int iY=(PIXEL_DECIMATION/2); iY<croppedHeight; iY+=PIXEL_DECIMATION)
	{
		for (int iX=(PIXEL_DECIMATION/2); iX<croppedWidth; iX+=PIXEL_DECIMATION)
		{
			XImage * xImage = XGetImage(x11Display, DefaultRootWindow(x11Display), iX, iY, 1, 1, AllPlanes, ZPixmap);
			// Extract the pixel from the X11-image
			const uint32_t pixel = uint32_t(XGetPixel(xImage, 0, 0));

			// Assign the color value
			outputPtr->red   = uint8_t((pixel >> 16) & 0xff);
			outputPtr->green = uint8_t((pixel >> 8)  & 0xff);
			outputPtr->blue  = uint8_t((pixel >> 0)  & 0xff);

			// Move to the next output pixel
			++outputPtr;

			// Cleanup allocated resources of the X11 grab
			XDestroyImage(xImage);
		}
	}
	std::cout << "Time required: " << timer.elapsed() << " ms" << std::endl;

	XCloseDisplay(x11Display);
}

