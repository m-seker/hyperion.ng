#include <grabber/JackGrabber.h>

#include <utils/Logger.h>
#include <utils/AudioPacket.h>

#include <chrono>

static Logger* _logger = nullptr;

JackGrabber::JackGrabber()
{
	_logger = Logger::getInstance("Jack");
}

JackGrabber::~JackGrabber()
{
}
/*
						AudioPacket packet(samples);
						memcpy(packet.memptr(), push_data, samples);
						emit systemAudio("Jack", packet);
*/
