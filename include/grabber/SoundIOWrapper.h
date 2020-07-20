#pragma once

#include <hyperion/GrabberWrapper.h>
#include <grabber/SoundIOGrabber.h>

class SoundIOWrapper : public GrabberWrapper
{
	Q_OBJECT

public:
	SoundIOWrapper(const unsigned updateRate_Hz);
	~SoundIOWrapper() override;

public slots:
	virtual void action();

private:
	SoundIOGrabber _grabber;

	bool _init;
};
