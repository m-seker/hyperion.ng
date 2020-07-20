#pragma once

#include <hyperion/LedString.h>
#include <utils/AudioPacket.h>

#include <utils/settings.h>

#include <utils/MelFrequencyCepstralCoefficients.h>
#include <utils/ExponentialMovingAverage.h>
#include <fftw3.h>

class Hyperion;
class Logger;

class AudioProcessor : public QObject
{
	Q_OBJECT

public:
	AudioProcessor(const LedString& ledString, Hyperion* hyperion);
	~AudioProcessor() override;

	std::vector<ColorRgb> process(const AudioPacket & packet);

private slots:
	void handleSettingsUpdate(const settings::type& type, const QJsonDocument& config) {}

private:
	Logger * _log;
	MFCC _mfcc;
	double_exponential_smoothing<double, 13> dbexpsmth; // or single_exponential_smoothing
	LedString _ledString;
	Hyperion * _hyperion;
	double* _input;
	fftw_complex* _output;
	fftw_plan _plan;
	std::vector<int16_t> _buffer;
	std::vector<ColorRgb> _prevColors;
};
