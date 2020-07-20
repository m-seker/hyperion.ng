#pragma once

#include <hyperion/GrabberWrapper.h>
#include <grabber/AlsaGrabber.h>

class AlsaWrapper : public GrabberWrapper
{
	Q_OBJECT

public:
	AlsaWrapper(const unsigned updateRate_Hz);
	~AlsaWrapper() override;

public slots:
	virtual void action();

private:
	AlsaGrabber _grabber;

	bool _init;
};
