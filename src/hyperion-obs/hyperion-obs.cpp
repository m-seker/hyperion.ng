#include "ObsWrapper.h"

#include <utils/ColorRgb.h>
#include <utils/Image.h>
#include <flatbufserver/FlatBufferConnection.h>
#include <ssdp/SSDPDiscover.h>

ObsWrapper *grabber = nullptr;
FlatBufferConnection *flatbuf = nullptr;

void start_hyperion_obs()
{
	printf("--------\n\n\n\n LOADING HYPERION PLUGIN \n\n\n\n--------------");

	grabber = new ObsWrapper(
		25,// 1000 / argFps.getInt(parser),
		0,//argCropLeft.getInt(parser),
		0,//argCropRight.getInt(parser),
		0,//argCropTop.getInt(parser),
		0,//argCropBottom.getInt(parser),
		4//argSizeDecimation.getInt(parser)
	);

	// server searching by ssdp
	QString address = "127.0.0.1:19400";

#if 0
	{
		SSDPDiscover discover;
		address = discover.getFirstService(searchType::STY_FLATBUFSERVER);
		if(address.isEmpty())
		{
			address = argAddress.value(parser);
		}
	}
#endif

	// Create the Flabuf-connection
	bool argSkipReply = false;
	int argPriority = 230;

	flatbuf = new FlatBufferConnection("Hyperion OBS Plugin", address, argPriority, argSkipReply);

	// Connect the screen capturing to flatbuf connection processing
	QObject::connect(grabber, SIGNAL(sig_screenshot(const Image<ColorRgb> &)), flatbuf, SLOT(setImage(Image<ColorRgb>)));

	// Start the capturing
	//grabber->start();
}
