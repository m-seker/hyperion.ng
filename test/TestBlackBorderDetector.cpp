// STL includes
#include <random>

// Hyperion includes
#include <utils/ColorRgb.h>

// Blackborder includes
#include <blackborder/BlackBorderDetector.h>

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
	for (unsigned y = 0; y < image.height(); ++y)
	{
		for (unsigned x = 0; x < image.width(); ++x)
		{
			if (y < topBorder || x < leftBorder)
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

TEST(TestBlackBorderDetector, TC_NO_BORDER)
{
	BlackBorderDetector detector(3);

	Image<ColorRgb> image = createImage(64, 64, 0, 0);
	BlackBorder border = detector.process(image);
	EXPECT_TRUE(border.unknown);
	EXPECT_EQ(0, border.horizontalSize);
	EXPECT_EQ(-1, border.verticalSize);
}

TEST(TestBlackBorderDetector, TC_TOP_BORDER)
{
	BlackBorderDetector detector(3);

	Image<ColorRgb> image = createImage(64, 64, 12, 0);
	BlackBorder border = detector.process(image);
	EXPECT_TRUE(border.unknown);
	EXPECT_NE(12, border.horizontalSize);
	EXPECT_EQ(-1, border.verticalSize);
}

TEST(TestBlackBorderDetector, TC_LEFT_BORDER)
{
	BlackBorderDetector detector(3);
	Image<ColorRgb> image = createImage(64, 64, 0, 12);
	BlackBorder border = detector.process(image);

	EXPECT_TRUE(border.unknown);
	EXPECT_EQ(-1, border.horizontalSize);
	EXPECT_NE(12, border.verticalSize);
}

TEST(TestBlackBorderDetector, TC_DUAL_BORDER)
{
	BlackBorderDetector detector(3);
	Image<ColorRgb> image = createImage(64, 64, 12, 12);
	BlackBorder border = detector.process(image);

	EXPECT_FALSE(border.unknown);
	EXPECT_NE(12, border.horizontalSize);
	EXPECT_NE(12, border.verticalSize);
}

TEST(TestBlackBorderDetector, TC_UNKNOWN_BORDER)
{
	BlackBorderDetector detector(3);
	Image<ColorRgb> image = createImage(64, 64, 30, 30);
	BlackBorder border = detector.process(image);

	EXPECT_FALSE(border.unknown);
}
