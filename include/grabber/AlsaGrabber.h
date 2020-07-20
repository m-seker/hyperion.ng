#pragma once

#include <utils/ColorRgb.h>
#include <hyperion/Grabber.h>

#include <thread>

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

class AudioPacket;
class Logger;

class AlsaGrabber : public Grabber
{
	Q_OBJECT
public:
	AlsaGrabber();
	~AlsaGrabber() override;

	bool Setup();
	int grabFrame(Image<ColorRgb> & image, bool forceUpdate=false);

signals:
	void systemAudio(const QString& name, const AudioPacket& audioPacket);

private:
	bool setupResources();
	void freeResources();
	bool configure();
	void listen();
	void get_properties();

	Logger *_logger {};
	char *_device;
	std::thread * _listen_thread;
	std::atomic<bool> _listen;
	snd_pcm_t *_handle;
	snd_pcm_format_t _format;
	snd_pcm_uframes_t _period_size;
	unsigned int _channels;
	unsigned int _rate;
	unsigned int _sample_size;
	uint8_t *_buffer;
};
