#pragma once

#include <hyperion/GrabberWrapper.h>
#include <grabber/JackGrabber.h>

class JackWrapper : public GrabberWrapper
{
	Q_OBJECT

public:
	JackWrapper(const unsigned updateRate_Hz);
	~JackWrapper() override;

public slots:
	virtual void action();

private:
	JackGrabber _grabber;

	bool _init;
};
