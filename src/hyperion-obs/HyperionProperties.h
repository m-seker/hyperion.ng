#ifndef HYPERION_PROPERTIES_H
#define HYPERION_PROPERTIES_H

#include <memory>
#include <QDialog>
#include <obs-module.h>

namespace Ui {
class HyperionProperties;
}

class HyperionProperties : public QDialog {
	Q_OBJECT

public:
	explicit HyperionProperties(QWidget *parent = 0);
	~HyperionProperties();
	void SetVisable();
	void showEvent(QShowEvent *event);
	void closeEvent(QCloseEvent *event);
	void EnableOptions(bool enable);
	void ShowWarning(bool show);

private Q_SLOTS:
	void onButtonAddressCopy();
	void onStart();
	void onStop();

private:
	Ui::HyperionProperties *ui;
	obs_output_t *_hyperionOutput;
	signal_handler_t *signalHandler;
	void UpdateParameter();
	void SaveSetting();
	static void OnStartSignal(void *data, calldata_t *cd);
	static void OnStopSignal(void *data, calldata_t *cd);
};

#endif // HYPERION_PROPERTIES_H
