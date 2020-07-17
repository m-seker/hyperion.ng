
#include "Simd/SimdConst.h"
#include "Simd/SimdConfig.h"
#include "Simd/SimdDefs.h"
#include "Simd/SimdStore.h"
#include "Simd/SimdMemory.h"

#include <chrono>
#include <QFile>
#include <QImage>


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
inline __m256i BgraToRgb(const uint8_t* bgra)
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
