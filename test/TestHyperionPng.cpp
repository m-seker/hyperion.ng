
// STL includes
#include <iostream>

// HyperionPNG includes
#include <hyperionpng/HyperionPng.h>

#include <gtest/gtest.h>

using namespace testing;

TEST(TestHyperionPNG, TestPNG)
{
	HyperionPng hyperion;
	hyperion.setInputSize(64, 64);

	// Obtain reference to buffer
	RgbImage& image = hyperion.image();

	// Write some data to the image
	for (unsigned y = 0; y < image.height(); ++y)
	{
		for (unsigned x = 0; x < image.width(); ++x)
		{
			image(x,y) = RgbColor{255, 0, 0};
		}
	}

	for (unsigned i = 0; i < 40; ++i)
	{
		hyperion.commit();
	}
}

