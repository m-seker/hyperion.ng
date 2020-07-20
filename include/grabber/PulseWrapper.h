#pragma once

#include <hyperion/GrabberWrapper.h>
#include <grabber/PulseGrabber.h>

class PulseWrapper : public GrabberWrapper
{
	Q_OBJECT

public:
	PulseWrapper(const unsigned updateRate_Hz);
	~PulseWrapper() override;

public slots:
	virtual void action();

private:
	PulseGrabber _grabber;

	bool _init;
};
