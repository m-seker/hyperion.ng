
// Utils includes
#include <utils/Image.h>
#include <utils/jsonschema/QJsonFactory.h>

// Hyperion includes
#include <utils/hyperion.h>
#include <hyperion/ImageToLedsMap.h>

#include <gtest/gtest.h>

using namespace testing;

TEST(TestImage2LedsMap, main)
{
	QString homeDir = getenv("RASPILIGHT_HOME");

	const QString schemaFile = homeDir + "/hyperion.schema.json";
	const QString configFile = homeDir + "/hyperion.config.json";

	QJsonObject config;
	int retval = QJsonFactory::load(schemaFile, configFile, config);
	ASSERT_GE(0, retval) << "Unable to load configuration : " << schemaFile.toStdString();

	const LedString ledString = hyperion::createLedString(config["leds"].toArray(), hyperion::createColorOrder(config["device"].toObject()));

	const ColorRgb testColor = {64, 123, 12};

	Image<ColorRgb> image(64, 64, testColor);
	hyperion::ImageToLedsMap map(64, 64, 0, 0, ledString.leds());

	std::vector<ColorRgb> ledColors(ledString.leds().size());
	map.getMeanLedColor(image, ledColors);

	std::cout << "[";
	for (const ColorRgb & color : ledColors)
	{
		std::cout << color;
	}
	std::cout << "]" << std::endl;
}
