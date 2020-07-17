#include "utils/ImageResampler.h"
#include <utils/Logger.h>

<<<<<<< Updated upstream
#include "Simd/SimdConst.h"
#include "Simd/SimdConfig.h"
#include "Simd/SimdDefs.h"
#include "Simd/SimdStore.h"
#include "Simd/SimdMemory.h"

#include <QImage>
#include <chrono>

namespace Simd
{
#ifdef SIMD_AVX2_ENABLE
namespace Avx2
{
namespace Hyperion
{
const __m256i K32_PERMUTE_BGRA_TO_BGR = SIMD_MM256_SETR_EPI32(0x0, 0x1, 0x2, 0x4, 0x5, 0x6, -1, -1);
const __m256i K8_SUFFLE_BGRA_TO_RGB = SIMD_MM256_SETR_EPI8(
	0x2, 0x1, 0x0, 0x6, 0x5, 0x4, 0xA, 0x9, 0x8, 0xE, 0xD, 0xC, -1, -1, -1, -1,
	0x2, 0x1, 0x0, 0x6, 0x5, 0x4, 0xA, 0x9, 0x8, 0xE, 0xD, 0xC, -1, -1, -1, -1);

template <bool align>
SIMD_INLINE __m256i BgraToRgb(const uint8_t* bgra)
{
	__m256i _bgra = Load<align>((__m256i*)bgra);
	return _mm256_permutevar8x32_epi32(_mm256_shuffle_epi8(_bgra, K8_SUFFLE_BGRA_TO_RGB), K32_PERMUTE_BGRA_TO_BGR);
}

template <bool align>
void BgraToRgb(const uint8_t* bgra, size_t beginWidth, size_t endWidth, size_t beginHeight, size_t endHeight, size_t bgraStride, uint8_t* rgb, size_t rgbStride, int hDecimation, int vDecimation)
{
	size_t width = endWidth - beginWidth;

	assert(width >= F);
	if (align)
		assert(Aligned(bgra) && Aligned(bgraStride) && Aligned(rgb) && Aligned(rgbStride));

	size_t widthF = AlignLo(width, F);
	if (width == widthF)
		widthF -= F;

	for (size_t row = beginHeight; row < endHeight; ++row)
	{
		for (size_t col = beginWidth; col < widthF; col += F)
		{
			Store<false>((__m256i*)(rgb + 3 * col), BgraToRgb<align>(bgra + 4 * col));
		}

		if (width != widthF)
			Store24<false>(rgb + 3 * (width - F), BgraToRgb<false>(bgra + 4 * (width - F)));

		bgra += bgraStride;
		rgb += rgbStride;
	}
}

void BgraToRgb(const uint8_t* bgra, size_t beginWidth, size_t endWidth, size_t beginHeight, size_t endHeight, size_t bgraStride, uint8_t* rgb, size_t rgbStride, int hDecimation, int vDecimation)
{
	if (Aligned(bgra) && Aligned(bgraStride) && Aligned(rgb) && Aligned(rgbStride))
	{
		BgraToRgb<true>(bgra, beginWidth, endWidth, beginHeight, endHeight, bgraStride, rgb, rgbStride, hDecimation, vDecimation);
	}
	else
	{
		BgraToRgb<false>(bgra, beginWidth, endWidth, beginHeight, endHeight, bgraStride, rgb, rgbStride, hDecimation, vDecimation);
	}
}
}
}
#endif// SIMD_AVX2_ENABLE
}
=======
#include <sys/time.h>
>>>>>>> Stashed changes

ImageResampler::ImageResampler()
	: _horizontalDecimation(1)
	, _verticalDecimation(1)
	, _cropLeft(0)
	, _cropRight(0)
	, _cropTop(0)
	, _cropBottom(0)
	, _videoMode(VideoMode::VIDEO_2D)
{
}

ImageResampler::~ImageResampler()
{
}

void ImageResampler::setHorizontalPixelDecimation(int decimator)
{
	_horizontalDecimation = decimator;
}

void ImageResampler::setVerticalPixelDecimation(int decimator)
{
	_verticalDecimation = decimator;
}

void ImageResampler::setCropping(int cropLeft, int cropRight, int cropTop, int cropBottom)
{
	_cropLeft   = cropLeft;
	_cropRight  = cropRight;
	_cropTop    = cropTop;
	_cropBottom = cropBottom;
}

void ImageResampler::setVideoMode(VideoMode mode)
{
	_videoMode = mode;
}

void ImageResampler::processImage(const uint8_t * data, int width, int height, int lineLength, PixelFormat pixelFormat, Image<ColorRgb> &outputImage) const
{
	int cropRight  = _cropRight;
	int cropBottom = _cropBottom;

	// handle 3D mode
	switch (_videoMode)
	{
	case VideoMode::VIDEO_3DSBS:
		cropRight = width >> 1;
		break;
	case VideoMode::VIDEO_3DTAB:
		cropBottom = width >> 1;
		break;
	default:
		break;
	}

    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    printf("Resampling image : %lld\n\n\n\n", milliseconds);


	// calculate the output size
	int outputWidth = (width - _cropLeft - cropRight - (_horizontalDecimation >> 1) + _horizontalDecimation - 1) / _horizontalDecimation;
	int outputHeight = (height - _cropTop - cropBottom - (_verticalDecimation >> 1) + _verticalDecimation - 1) / _verticalDecimation;
	outputImage.resize(outputWidth, outputHeight);

	if (pixelFormat == PixelFormat::BGR32)
	{
		int beginWidth = _cropLeft + (_horizontalDecimation >> 1);
		int beginHeight = _cropTop + (_verticalDecimation >> 1);
		int endWidth = outputWidth;
		int endHeight = outputHeight;
auto start = std::chrono::high_resolution_clock::now();

		Simd::Avx2::Hyperion::BgraToRgb(data, beginWidth, endWidth, beginHeight, endHeight, 4 * width, (uint8_t*)outputImage.memptr(), outputWidth * sizeof(ColorRgb), _horizontalDecimation, _verticalDecimation);


#if 1
static int a = 0;
++a;
if (a == 130)
{
	std::cout << "Yaziyoruz" << std::endl;
	QImage input(data, width, height, QImage::Format_ARGB32);
	QImage output((uint8_t*)outputImage.memptr(), outputWidth, outputHeight, QImage::Format_RGB888);
	QImage yalan((uint8_t*)outputImage.memptr(), outputWidth, outputHeight, QImage::Format_ARGB32);
	input.save("/home/murse/input.jpg", 0, -1);
	output.save("/home/murse/output.jpg", 0, -1);
	yalan.save("/home/murse/yalan.jpg", 0, -1);
}
#endif

auto finish = std::chrono::high_resolution_clock::now();
std::chrono::duration<double, std::micro> elapsed = finish - start;
std::cout << "Elapsed Time: " << elapsed.count() << " micro" << std::endl;

		return;
	}

auto start = std::chrono::high_resolution_clock::now();

	for (int yDest = 0, ySource = _cropTop + (_verticalDecimation >> 1); yDest < outputHeight; ySource += _verticalDecimation, ++yDest)
	{
		for (int xDest = 0, xSource = _cropLeft + (_horizontalDecimation >> 1); xDest < outputWidth; xSource += _horizontalDecimation, ++xDest)
		{
			ColorRgb & rgb = outputImage(xDest, yDest);

			switch (pixelFormat)
			{
				case PixelFormat::UYVY:
				{
					int index = lineLength * ySource + (xSource << 1);
					uint8_t y = data[index+1];
					uint8_t u = ((xSource&1) == 0) ? data[index  ] : data[index-2];
					uint8_t v = ((xSource&1) == 0) ? data[index+2] : data[index  ];
					yuv2rgb(y, u, v, rgb.red, rgb.green, rgb.blue);
				}
				break;
				case PixelFormat::YUYV:
				{
					int index = lineLength * ySource + (xSource << 1);
					uint8_t y = data[index];
					uint8_t u = ((xSource&1) == 0) ? data[index+1] : data[index-1];
					uint8_t v = ((xSource&1) == 0) ? data[index+3] : data[index+1];
					yuv2rgb(y, u, v, rgb.red, rgb.green, rgb.blue);
				}
				break;
				case PixelFormat::BGR16:
				{
					int index = lineLength * ySource + (xSource << 1);
					rgb.blue  = (data[index] & 0x1f) << 3;
					rgb.green = (((data[index+1] & 0x7) << 3) | (data[index] & 0xE0) >> 5) << 2;
					rgb.red   = (data[index+1] & 0xF8);
				}
				break;
				case PixelFormat::BGR24:
				{
					int index = lineLength * ySource + (xSource << 1) + xSource;
					rgb.blue  = data[index  ];
					rgb.green = data[index+1];
					rgb.red   = data[index+2];
				}
				break;
				case PixelFormat::RGB32:
				{
					int index = lineLength * ySource + (xSource << 2);
					rgb.red   = data[index  ];
					rgb.green = data[index+1];
					rgb.blue  = data[index+2];
				}
				break;
				case PixelFormat::BGR32:
				{
					int index = lineLength * ySource + (xSource << 2);
					rgb.blue  = data[index  ];
					rgb.green = data[index+1];
					rgb.red   = data[index+2];
				}
				break;
#ifdef HAVE_JPEG_DECODER
				case PixelFormat::MJPEG:
				break;
#endif
				case PixelFormat::NO_CHANGE:
					Error(Logger::getInstance("ImageResampler"), "Invalid pixel format given");
				break;
			}
		}
	}
auto finish = std::chrono::high_resolution_clock::now();
std::chrono::duration<double, std::micro> elapsed = finish - start;
std::cout << "Elapsed Time: " << elapsed.count() << " microseconds" << std::endl;

}

uint8_t ImageResampler::clamp(int x)
{
	return (x<0) ? 0 : ((x>255) ? 255 : uint8_t(x));
}

void ImageResampler::yuv2rgb(uint8_t y, uint8_t u, uint8_t v, uint8_t &r, uint8_t &g, uint8_t &b)
{
	// see: http://en.wikipedia.org/wiki/YUV#Y.27UV444_to_RGB888_conversion
	int c = y - 16;
	int d = u - 128;
	int e = v - 128;

	r = clamp((298 * c + 409 * e + 128) >> 8);
	g = clamp((298 * c - 100 * d - 208 * e + 128) >> 8);
	b = clamp((298 * c + 516 * d + 128) >> 8);
}
