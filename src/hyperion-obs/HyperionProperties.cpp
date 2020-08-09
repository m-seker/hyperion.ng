#include "HyperionProperties.h"

#include <obs-frontend-api.h>
#include <util/config-file.h>
#include <util/platform.h>

#include <QMainWindow>
#include <QClipboard>

#include "ui_HyperionProperties.h"

#define CONFIG_SECTIION "HyperionOutput"

static bool make_config_dir()
{
	auto path = obs_module_config_path("");
	auto ret = os_mkdirs(path);
	bfree(path);
	return ret == MKDIR_SUCCESS || ret == MKDIR_EXISTS;
}

static obs_data_t *hyperion_output_read_data(bool create = false)
{
	obs_data_t *data = nullptr;
	if (create)
		data = obs_data_create();
	else {
		auto path = obs_module_config_path("hyperion_output.json");
		data = obs_data_create_from_json_file_safe(path, "bak");
		bfree(path);
	}
	obs_data_set_default_int(data, "port", 554);
	return data;
}

static bool hyperion_output_save_data(obs_data_t *data)
{
	if (!make_config_dir())
		return false;

	auto path = obs_module_config_path("hyperion_output.json");
	auto ret = obs_data_save_json_safe(data, path, "tmp", "bak");
	bfree(path);
	return ret;
}

static config_t *hyperion_properties_open_config()
{
	if (!make_config_dir())
		return nullptr;

	auto path = obs_module_config_path("config.ini");
	config_t *config;
	auto ret = config_open(&config, path, CONFIG_OPEN_ALWAYS);
	bfree(path);
	config_set_default_bool(config, CONFIG_SECTIION, "AutoStart", false);
	return config;
}

obs_output_t* _hyperionOutput;

HyperionProperties::HyperionProperties(QWidget *parent)
	: QDialog(parent)
	  , ui(new Ui::HyperionProperties)
{
	ui->setupUi(this);
	connect(ui->pushButtonStart, SIGNAL(clicked()), this, SLOT(onStart()));
	connect(ui->pushButtonStop, SIGNAL(clicked()), this, SLOT(onStop()));

	obs_data_t *settings = hyperion_output_read_data();
	_hyperionOutput = obs_output_create("hyperion_output", "HyperionOutput", settings, nullptr);
	obs_data_release(settings);

	auto handler = obs_output_get_signal_handler(_hyperionOutput);
	signal_handler_connect(handler, "start", OnStartSignal, this);
	signal_handler_connect(handler, "stop", OnStartSignal, this);

	config_t *config = hyperion_properties_open_config();
	auto autoStart = config_get_bool(config, CONFIG_SECTIION, "AutoStart");
	config_close(config);

	obs_data_t *data = hyperion_output_read_data();
	int port = obs_data_get_int(data, "port");
	obs_data_release(data);

	ui->checkBoxAuto->setChecked(autoStart);
	ui->spinBoxPort->setValue(port);
	ui->labelMessage->setStyleSheet("QLabel { color : red; }");
	ui->labelMessage->setVisible(false);
	ui->pushButtonStop->setEnabled(false);
}

HyperionProperties::~HyperionProperties()
{
	auto handler = obs_output_get_signal_handler(_hyperionOutput);
	signal_handler_connect(handler, "start", OnStartSignal, this);
	signal_handler_connect(handler, "stop", OnStartSignal, this);

	delete ui;
}

void HyperionProperties::SetVisable()
{
	setVisible(!isVisible());
}

void HyperionProperties::EnableOptions(bool enable)
{
	ui->spinBoxPort->setEnabled(enable);
	ui->pushButtonStart->setEnabled(enable);
	ui->pushButtonStop->setEnabled(!enable);
}

void HyperionProperties::ShowWarning(bool show)
{
	ui->labelMessage->setVisible(show);
}

void HyperionProperties::onStart()
{
	UpdateParameter();
	bool started = obs_output_start(_hyperionOutput);
	ShowWarning(!started);
}

void HyperionProperties::onStop()
{
	obs_output_stop(_hyperionOutput);
}

void HyperionProperties::OnStartSignal(void *data, calldata_t *cd)
{
	auto page = (HyperionProperties *)data;
	page->EnableOptions(false);
}

void HyperionProperties::OnStopSignal(void *data, calldata_t *cd)
{
	auto page = (HyperionProperties *)data;
	page->EnableOptions(true);
}

void HyperionProperties::UpdateParameter()
{
	SaveSetting();
	auto data = hyperion_output_read_data();
	obs_output_update(_hyperionOutput, data);
	obs_data_release(data);
}

void HyperionProperties::showEvent(QShowEvent *event)
{

}

void HyperionProperties::closeEvent(QCloseEvent *event)
{
	SaveSetting();
}

void HyperionProperties::SaveSetting()
{
	config_t *config = hyperion_properties_open_config();
	if (config) {
		bool autoStart = ui->checkBoxAuto->isChecked();
		config_set_bool(config, CONFIG_SECTIION, "AutoStart",
				autoStart);
		config_save(config);
		config_close(config);
	}
	auto data = hyperion_output_read_data(true);
	int port = ui->spinBoxPort->value();
	obs_data_set_int(data, "port", port);
	hyperion_output_save_data(data);
}
