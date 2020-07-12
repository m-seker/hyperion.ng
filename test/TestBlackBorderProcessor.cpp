
// STL includes
#include <cassert>
#include <random>
#include <iostream>

// Utils includes
#include <utils/Image.h>
#include <utils/ColorRgb.h>

// Blackborder includes
#include "blackborder/BlackBorderProcessor.h"

#include <gtest/gtest.h>

using namespace hyperion;
using namespace testing;

ColorRgb randomColor()
{
	const uint8_t randomRedValue   = uint8_t(rand() % (std::numeric_limits<uint8_t>::max() + 1));
	const uint8_t randomGreenValue = uint8_t(rand() % (std::numeric_limits<uint8_t>::max() + 1));
	const uint8_t randomBlueValue  = uint8_t(rand() % (std::numeric_limits<uint8_t>::max() + 1));

	return {randomRedValue, randomGreenValue, randomBlueValue};
}

Image<ColorRgb> createImage(unsigned width, unsigned height, unsigned topBorder, unsigned leftBorder)
{
	Image<ColorRgb> image(width, height);
	for (unsigned x=0; x<image.width(); ++x)
	{
		for (unsigned y=0; y<image.height(); ++y)
		{
			if (y < topBorder || y > ( height - topBorder ) || x < leftBorder || x > (width - leftBorder) )
			{
				image(x,y) = ColorRgb::BLACK;
			}
			else
			{
				image(x,y) = randomColor();
			}
		}
	}
	return image;
}

TEST(TestBlackBorderProcessor, main)
{
	unsigned borderCnt  = 50;
	QJsonObject config;

	BlackBorderProcessor processor(config);

	// Start with 'no border' detection
	Image<ColorRgb> noBorderImage = createImage(64, 64, 0, 0);
	for (unsigned i=0; i < 10; ++i)
	{
		bool newBorder = processor.process(noBorderImage);
		if (i == 0)
		{
			// Switch to 'no border' should immediate
			ASSERT_TRUE(newBorder) << "Failed to detect 'no border' when required";
		}
		else
		{
			ASSERT_FALSE(newBorder) << "Incorrectly detected new border, when there in none";
		}
	}

	// Verify that the border is indeed
	ASSERT_FALSE(processor.getCurrentBorder().unknown) 	  << "Incorrectlty identified 'no border'";
	ASSERT_EQ(0, processor.getCurrentBorder().horizontalSize) << "Incorrectlty identified 'no border'";
	ASSERT_EQ(0, processor.getCurrentBorder().verticalSize)   << "Incorrectlty identified 'no border'";

	int borderSize = 12;
	Image<ColorRgb> horzImage = createImage(64, 64, borderSize, 0);
	for (unsigned i=0; i<borderCnt*2; ++i)
	{
		bool newBorder = processor.process(horzImage);
                if (i == borderCnt+10)// 10 frames till new border gets a chance to proof consistency
                {
                        ASSERT_TRUE(newBorder) << "Failed to detect 'no border' when required after " << borderCnt << " images";
                }
                else
                {
                        ASSERT_FALSE(newBorder) << "Incorrectly detected no border, when there in none";
                }
	}

	if (processor.getCurrentBorder().unknown != false || processor.getCurrentBorder().horizontalSize != borderSize || processor.getCurrentBorder().verticalSize != 0)
	{

		std::cerr << "Incorrectlty found 'horizontal border' (" << processor.getCurrentBorder().unknown << "," << processor.getCurrentBorder().horizontalSize << "," << processor.getCurrentBorder().verticalSize << ")" << std::endl;
		exit(EXIT_FAILURE);
	}

	for (unsigned i=0; i<borderCnt*2; ++i)
	{

		bool newBorder = processor.process(noBorderImage);
		if (i == borderCnt+10)// 10 frames till new border gets a chance to proof consistency
		{
			ASSERT_TRUE(newBorder) << "Failed to detect 'no border' when required after " << borderCnt << " images";
		}
		else
		{
			ASSERT_FALSE(newBorder) << "Incorrectly detected no border, when there in none";
		}
	}

	// Check switch back to no border

	ASSERT_FALSE(processor.getCurrentBorder().unknown)	  << "Failed to switch back to 'no border'";
	ASSERT_EQ(0, processor.getCurrentBorder().horizontalSize) << "Failed to switch back to 'no border'";
	ASSERT_EQ(0, processor.getCurrentBorder().verticalSize)   << "Failed to switch back to 'no border'";

	Image<ColorRgb> vertImage = createImage(64, 64, 0, borderSize);
	for (unsigned i=0; i<borderCnt*2; ++i)
	{
		bool newBorder = processor.process(vertImage);
		if (i == borderCnt+10)// 10 frames till new border gets a chance to proof consistency
		{
			ASSERT_TRUE(newBorder) << "Failed to detect 'vertical border' when required after " << borderCnt << " images";
		}
		else
		{
			ASSERT_FALSE(newBorder) << "Incorrectly detected new border, when there in none";
		}
	}

	ASSERT_FALSE(processor.getCurrentBorder().unknown) 		 << "Incorrectlty found 'vertical border'";
	ASSERT_EQ(0, processor.getCurrentBorder().horizontalSize)	 << "Incorrectlty found 'vertical border'";
	ASSERT_EQ(borderSize, processor.getCurrentBorder().verticalSize) << "Incorrectlty found 'vertical border'";
}

