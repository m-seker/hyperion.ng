#include <grabber/AlsaGrabber.h>

#include <utils/Logger.h>
#include <utils/AudioPacket.h>

#include <chrono>
#include <iostream>

#include <fftw3.h>

AlsaGrabber::AlsaGrabber() :
	  _logger(Logger::getInstance("ALSA"))
	, _device(strdup("hw:1,1,4"))
	, _listen(false)
	, _handle(nullptr)
	, _listen_thread(nullptr)
	, _buffer(nullptr)
	, _rate(44100)
{
	Setup();
}

AlsaGrabber::~AlsaGrabber()
{
	freeResources();
}

bool AlsaGrabber::Setup()
{
	setupResources();

	return true;
}

bool AlsaGrabber::setupResources()
{
	int err = snd_pcm_open(&_handle, _device, SND_PCM_STREAM_CAPTURE, 0);
	if (err < 0)
	{
		Error(_logger, "Failed to open '%s': %s", _device, snd_strerror(err));
		return false;
	}

	if (!configure())
	{
		goto cleanup;
	}

	if (snd_pcm_state(_handle) != SND_PCM_STATE_PREPARED)
	{
		Error(_logger, "Device not prepared: '%s'", _device);
		goto cleanup;
	}

	/* start listening */
	err = snd_pcm_start(_handle);
	if (err < 0)
	{
		Error(_logger, "Failed to start '%s': %s", _device, snd_strerror(err));
		goto cleanup;
	}

	/* create capture thread */
	_listen_thread = new std::thread([this]() {
#ifndef _WIN32
		auto handle = _listen_thread->native_handle();
		pthread_setname_np(handle, "ALSATHREAD");
#endif
		listen();
	});
	return true;

cleanup:
	freeResources();
	return false;
}

void AlsaGrabber::freeResources()
{
	if (_handle)
	{
		if (_listen_thread)
		{
			_listen = false;
			_listen_thread->join();
			delete _listen_thread;
			_listen_thread = nullptr;
		}

		if (_handle)
		{
			snd_pcm_drop(_handle);
			snd_pcm_close(_handle);
			_handle = nullptr;
		}

		free(_buffer);
		_buffer = nullptr;

		free(_device);
		_device = nullptr;
	}
}

bool AlsaGrabber::configure()
{
	snd_pcm_hw_params_t * hwparams = nullptr;
	snd_pcm_hw_params_alloca(&hwparams);

	int err = snd_pcm_hw_params_any(_handle, hwparams);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params_any failed: %s", snd_strerror(err));
		return false;
	}

	err = snd_pcm_hw_params_set_access(_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params_set_access failed: %s", snd_strerror(err));
		return false;
	}

	_format = SND_PCM_FORMAT_S16;
	err = snd_pcm_hw_params_set_format(_handle, hwparams, _format);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params_set_format failed: %s", snd_strerror(err));
		return false;
	}

	err = snd_pcm_hw_params_set_rate_near(_handle, hwparams, &_rate, 0);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params_set_rate_near failed: %s", snd_strerror(err));
		return false;
	}

	Info(_logger, "PCM '%s' rate set to %d", _device, _rate);

	err = snd_pcm_hw_params_get_channels(hwparams, &_channels);
	if (err < 0)
	{
		_channels = 2;
	}

	err = snd_pcm_hw_params_set_channels_near(_handle, hwparams, &_channels);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params_set_channels_near failed: %s", snd_strerror(err));
		return false;
	}

	Info(_logger, "PCM '%s' channels set to %d", _device, _channels);

	err = snd_pcm_hw_params(_handle, hwparams);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params failed: %s", snd_strerror(err));
		return false;
	}

	int dir = 0;
	err = snd_pcm_hw_params_get_period_size(hwparams, &_period_size, &dir);
	if (err < 0)
	{
		Error(_logger, "snd_pcm_hw_params_get_period_size failed: %s", snd_strerror(err));
		return false;
	}

	_sample_size = (_channels * snd_pcm_format_physical_width(_format)) / 8;

	free(_buffer);

	_buffer = (uint8_t *)malloc(_period_size * _sample_size);
	memset(_buffer, 0, _period_size * _sample_size);

	return true;
}

void AlsaGrabber::listen()
{
	Debug(_logger, "Capture thread started.");

	_listen = true;

	do
	{
		snd_pcm_sframes_t frames = snd_pcm_readi(_handle, _buffer, _period_size);

		if (!_listen)
		{
			break;
		}

		if (frames <= 0)
		{
			frames = snd_pcm_recover(_handle, frames, 0);
			if (frames <= 0)
			{
				snd_pcm_wait(_handle, 100);
				continue;
			}

			/* TODO : MURSE uint16_t memptry uint8_t half copy */
			AudioPacket packet(frames);
 			memcpy(packet.memptr(), _buffer, frames);
			emit systemAudio("ALSA", packet);
		}
	} while (_listen);

	Debug(_logger, "Capture thread is about to exit.");
}

int AlsaGrabber::grabFrame(Image<ColorRgb> & /*image*/, bool /*forceUpdate*/)
{
	return 0;
}

void AlsaGrabber::get_properties()
{
	void **hints;
	void **hint;
	char *name = NULL;
	char *descr = NULL;
	char *io = NULL;
	char *descr_i;

	if (snd_device_name_hint(-1, "pcm", &hints) < 0)
	{
		return;
	}

	hint = hints;
	while (*hint != NULL)
	{
		/* check if we're dealing with an Input */
		io = snd_device_name_get_hint(*hint, "IOID");
		if (io != NULL && strcmp(io, "Input") != 0)
			goto next;

		name = snd_device_name_get_hint(*hint, "NAME");
		if (name == NULL || strstr(name, "front:") == NULL)
			goto next;

		descr = snd_device_name_get_hint(*hint, "DESC");
		if (!descr)
			goto next;

		descr_i = descr;
		while (*descr_i)
		{
			if (*descr_i == '\n')
			{
				*descr_i = '\0';
				break;
			}
			else
			{
				++descr_i;
			}
		}
		std::cout << "Description" << descr << std::endl;
		std::cout << "Name" << name << std::endl;

	next:
		if (name != NULL)
			free(name), name = NULL;

		if (descr != NULL)
			free(descr), descr = NULL;

		if (io != NULL)
			free(io), io = NULL;

		++hint;
	}

	snd_device_name_free_hint(hints);
}
