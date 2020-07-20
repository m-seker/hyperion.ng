#include <grabber/SoundIOGrabber.h>

#include <utils/Logger.h>
#include <utils/AudioPacket.h>

#include <soundio/soundio.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <chrono>

static Logger* _logger = nullptr;

static enum SoundIoFormat prioritized_formats[] = {
    SoundIoFormatFloat32NE,
    SoundIoFormatFloat32FE,
    SoundIoFormatS32NE,
    SoundIoFormatS32FE,
    SoundIoFormatS24NE,
    SoundIoFormatS24FE,
    SoundIoFormatS16NE,
    SoundIoFormatS16FE,
    SoundIoFormatFloat64NE,
    SoundIoFormatFloat64FE,
    SoundIoFormatU32NE,
    SoundIoFormatU32FE,
    SoundIoFormatU24NE,
    SoundIoFormatU24FE,
    SoundIoFormatU16NE,
    SoundIoFormatU16FE,
    SoundIoFormatS8,
    SoundIoFormatU8,
    SoundIoFormatInvalid,
};

static int prioritized_sample_rates[] = {
    48000,
    44100,
    96000,
    24000,
    0,
};


__attribute__ ((cold))
__attribute__ ((noreturn))
__attribute__ ((format (printf, 1, 2)))
static void panic(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
	va_end(ap);
	abort();
}


void read_callback(struct SoundIoInStream *instream, int frame_count_min, int frame_count_max)
{
	for (;;)
	{
		struct SoundIoChannelArea *areas;
		int err;
		int frame_count;

		if ((err = soundio_instream_begin_read(instream, &areas, &frame_count)))
			panic("begin read error: %s", soundio_strerror(err));

		if (!frame_count)
			break;

		if (!areas)
		{
			// Due to an overflow there is a hole.
			fprintf(stderr, "Dropped %d frames due to internal overflow\n", frame_count);
			return;
		}
		else
		{
			for (int frame = 0; frame < frame_count; frame += 1)
			{
				for (int ch = 0; ch < instream->layout.channel_count; ch += 1)
				{
					AudioPacket packet(instream->bytes_per_sample / 2);
					memcpy(packet.memptr(), areas[ch].ptr, instream->bytes_per_sample);

					SoundIOGrabber * grabber = static_cast<SoundIOGrabber*>(instream->userdata);
					emit grabber->systemAudio("SoundIO", packet);

					areas[ch].ptr += areas[ch].step;
				}
			}
		}

		if ((err = soundio_instream_end_read(instream)))
			panic("end read error: %s", soundio_strerror(err));
	}
}

SoundIOGrabber::SoundIOGrabber()
{
	_logger = Logger::getInstance("SoundIO");

	_soundio = soundio_create();
	if (!_soundio)
		panic("out of memory");

	setupResources();
}

SoundIOGrabber::~SoundIOGrabber()
{
	soundio_destroy(_soundio);
}

bool SoundIOGrabber::setupResources()
{
	/* TODO : MURSE */
	enum SoundIoBackend backend = SoundIoBackendJack;
	char *in_device_id = "MURSE";
	bool in_raw = false;
	double microphone_latency = 0.2; // seconds

	int err = (backend == SoundIoBackendNone) ?
	soundio_connect(_soundio) : soundio_connect_backend(_soundio, backend);
	if (err)
		panic("error connecting: %s", soundio_strerror(err));

	soundio_flush_events(_soundio);

	int default_in_device_index = soundio_default_input_device_index(_soundio);
	if (default_in_device_index < 0)
		panic("no input device found");

	int in_device_index = default_in_device_index;
	if (in_device_id)
	{
		bool found = false;
		for (int i = 0; i < soundio_input_device_count(_soundio); i += 1)
		{
			struct SoundIoDevice *device = soundio_get_input_device(_soundio, i);
			if (device->is_raw == in_raw && strcmp(device->id, in_device_id) == 0)
			{
				in_device_index = i;
				found = true;
				soundio_device_unref(device);
				break;
			}
			soundio_device_unref(device);
		}

		if (!found)
			panic("invalid input device id: %s", in_device_id);
	}

	_in_device = soundio_get_input_device(_soundio, in_device_index);
	if (!_in_device)
		panic("could not get input device: out of memory");

	fprintf(stderr, "Input device: %s\n", _in_device->name);

	int *sample_rate;
	for (sample_rate = prioritized_sample_rates; *sample_rate; sample_rate += 1) {
		if (soundio_device_supports_sample_rate(_in_device, *sample_rate))
		{
			break;
		}
	}

	if (!*sample_rate)
	{
		panic("incompatible sample rates");
		return false;
	}

	enum SoundIoFormat *fmt;
	for (fmt = prioritized_formats; *fmt != SoundIoFormatInvalid; fmt += 1) {
		if (soundio_device_supports_format(_in_device, *fmt))
		{
			break;
		}
	}

	if (*fmt == SoundIoFormatInvalid)
	{
		panic("incompatible sample formats");
		return false;
	}

	_instream = soundio_instream_create(_in_device);
	if (!_instream)
	{
		panic("out of memory");
		return false;
        }

	_instream->format = *fmt;
	_instream->sample_rate = *sample_rate;
	//_instream->layout = nullptr;
	_instream->software_latency = microphone_latency;
	_instream->read_callback = read_callback;
	_instream->userdata = this;

	if ((err = soundio_instream_open(_instream))) {
		fprintf(stderr, "unable to open input stream: %s", soundio_strerror(err));
		return false;
	}

	if ((err = soundio_instream_start(_instream)))
		panic("unable to start input device: %s", soundio_strerror(err));

	_listenThread = new std::thread([this]() {
		for (;;)
			soundio_wait_events(_soundio);
	});

	return true;
}

void SoundIOGrabber::freeResources()
{
	soundio_instream_destroy(_instream);
	soundio_device_unref(_in_device);
}
