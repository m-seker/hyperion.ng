#pragma once

#include <utils/ColorRgb.h>
#include <hyperion/Grabber.h>

#include <thread>

#include <pulse/mainloop.h>
#include <pulse/context.h>
#include <pulse/introspect.h>
#include <pulse/stream.h>
#include <pulse/error.h>


class AudioPacket;
class Logger;

class PulseAudioException : public std::exception {
public:
	inline virtual const char* what() const throw() override {
		return "PulseAudioException";
	}
};

class PulseGrabber : public Grabber
{
	Q_OBJECT
public:
	struct Source {
		std::string m_name, m_description;
		inline Source(const std::string& name, const std::string& description) : m_name(name), m_description(description) {}
	};

	PulseGrabber();
	~PulseGrabber() override;

	bool Setup();
	int grabFrame(Image<ColorRgb> & image, bool forceUpdate=false);

	// Returns whether an error has occurred in the input thread.
	// This function is thread-safe.
	inline bool HasErrorOccurred() { return m_error_occurred; }

	static std::vector<Source> GetSourceList();

signals:
	void systemAudio(const QString& name, const AudioPacket& audioPacket);

private:
	static const int64_t START_DELAY;

	bool setupResources();
	void freeResources();

	QString m_source_name;
	unsigned int m_sample_rate, m_channels;

	pa_mainloop *m_pa_mainloop;
	pa_context *m_pa_context;
	pa_stream *m_pa_stream;
	unsigned int m_pa_period_size;

	bool m_stream_is_monitor;
	bool m_stream_suspended, m_stream_moved;

	std::thread m_thread;
	std::atomic<bool> m_should_stop, m_error_occurred;

	void Init();
	void Free();

	void DetectMonitor();

	static void SourceInfoCallback(pa_context* context, const pa_source_info* info, int eol, void* userdata);
	static void SuspendedCallback(pa_stream* stream, void* userdata);
	static void MovedCallback(pa_stream* stream, void* userdata);

	void InputThread();

	std::vector<uint16_t> _segment;

};

