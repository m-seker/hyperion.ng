
// STL includes
#include <iostream>

// Utils includes
#include <utils/Image.h>
#include <utils/ColorRgba.h>
#include <utils/ColorRgb.h>
#include <utils/ColorBgr.h>
#include <hyperion/ImageProcessor.h>

#include <gtest/gtest.h>

using namespace testing;

TEST(TestRgbImage, execute)
{
	int width = 64;
	int height = 64;
	Image<ColorRgb> image_rgb(width, height, ColorRgb::BLACK);
	Image<ColorBgr> image_bgr(image_rgb.width(), image_rgb.height(), ColorBgr::BLACK);

	unsigned size = width * height;

	// BGR
 	for (unsigned i = 0; i < size; ++i)
 		image_bgr.memptr()[i] = ColorBgr{0,128,255};

	// to RGB
	image_bgr.toRgb(image_rgb);

	// test
	for (unsigned i = 0; i < size; ++i)
	{
		const ColorRgb rgb = image_rgb.memptr()[i];
		EXPECT_EQ(255, rgb.red);
		EXPECT_EQ(128, rgb.green);
		EXPECT_EQ(0, rgb.blue);
	}
}

