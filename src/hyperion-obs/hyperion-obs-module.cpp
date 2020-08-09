#include <obs-module.h>
#include <obs-frontend-api.h>

#include <QAction>
#include <QMainWindow>

#include "HyperionProperties.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#endif

#define MODULE_NAME "hyperion-obs module"

extern void start_hyperion_obs();

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("hyperion-obs", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "Hyperion OBS output";
}

extern struct obs_output_info hyperion_output_info;

HyperionProperties *hyperionProperties;

MODULE_EXPORT bool obs_module_load(void)
{
#ifdef _WIN32
	WSADATA wsad;
	WSAStartup(MAKEWORD(2, 2), &wsad);
#endif
	obs_register_output(&hyperion_output_info);

	hyperionProperties = new HyperionProperties();

	QMainWindow *mainWindow = (QMainWindow *)obs_frontend_get_main_window();
	QAction *action = (QAction *)obs_frontend_add_tools_menu_qaction(
		obs_module_text("Hyperion"));

	auto menu_cb = [&] {
		hyperionProperties->setVisible(!hyperionProperties->isVisible());
	};

	action->connect(action, &QAction::triggered, menu_cb);

	//obs_frontend_add_event_callback(obs_frontend_event, nullptr);

	start_hyperion_obs();

	return true;
}

MODULE_EXPORT void obs_module_unload(void)
{
#ifdef _WIN32
	WSACleanup();
#endif
}
