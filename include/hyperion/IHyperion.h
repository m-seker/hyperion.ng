#pragma once

// stl includes
#include <list>

// QT includes
#include <QString>
#include <QStringList>
#include <QSize>
#include <QJsonObject>
#include <QJsonDocument>

// hyperion-utils includes
#include <utils/ColorRgb.h>
#include <utils/Components.h>
#include <utils/VideoMode.h>

// Hyperion includes
#include <hyperion/LedString.h>
#include <hyperion/PriorityMuxer.h>
#include <hyperion/ColorAdjustment.h>

// Effect engine includes
#include <effectengine/EffectDefinition.h>
#include <effectengine/ActiveEffectDefinition.h>
#include <effectengine/EffectSchema.h>

#include <leddevice/LedDevice.h>

// settings utils
#include <utils/settings.h>

// Forward class declaration
class ComponentRegister;
class Image;
class HyperionDaemon;
class ImageProcessor;
class MessageForwarder;
class LinearColorSmoothing;
class EffectEngine;
class MultiColorAdjustment;
class ColorAdjustment;
class SettingsManager;
class BGEffectHandler;
class CaptureCont;
class BoblightServer;
class LedDeviceWrapper;

class IHyperion : public QObject
{
	Q_OBJECT
public:
	using InputInfo = PriorityMuxer::InputInfo;

	~Hyperion() override;

	void freeObjects(bool emitCloseSignal=false);
	ImageProcessor* getImageProcessor() { return _imageProcessor; }
	const quint8 & getInstanceIndex() { return _instIndex; }
	QSize getLedGridSize() const { return _ledGridSize; }
	const int & getLedMappingType();
	unsigned addSmoothingConfig(int settlingTime_ms, double ledUpdateFrequency_hz=25.0, unsigned updateDelay=0);
	unsigned updateSmoothingConfig(unsigned id, int settlingTime_ms=200, double ledUpdateFrequency_hz=25.0, unsigned updateDelay=0);
	const VideoMode & getCurrentVideoMode();
	const QString & getActiveDeviceType();
	LedDevice * getActiveDevice() const;

public slots:
	int getLatchTime() const;
	unsigned getLedCount() const;
	void registerInput(const int priority, const hyperion::Components& component, const QString& origin = "System", const QString& owner = "", unsigned smooth_cfg = 0);
	bool setInput(const int priority, const std::vector<ColorRgb>& ledColors, const int timeout_ms = -1, const bool& clearEffect = true);
	bool setInputImage(const int priority, const Image<ColorRgb>& image, const int64_t timeout_ms = -1, const bool& clearEffect = true);
	void setColor(const int priority, const std::vector<ColorRgb> &ledColors, const int timeout_ms = -1, const QString& origin = "System" ,bool clearEffects = true);
	bool setInputInactive(const quint8& priority);
	const QStringList & getAdjustmentIds() const;
	ColorAdjustment * getAdjustment(const QString& id);
	void adjustmentsUpdated();
	bool clear(const int priority, bool forceClearAll=false);
	EffectEngine* getEffectEngineInstance() { return _effectEngine; }
	QString saveEffect(const QJsonObject& obj);
	QString deleteEffect(const QString& effectName);
	int setEffect(const QString & effectName, int priority, int timeout = -1, const QString & origin="System");
	int setEffect(const QString &effectName
				, const QJsonObject &args
				, int priority
				, int timeout = -1
				, const QString &pythonScript = ""
				, const QString &origin="System"
				, const QString &imageData = ""
	);
	const std::list<EffectDefinition> &getEffects() const;
	const std::list<ActiveEffectDefinition> &getActiveEffects();
	const std::list<EffectSchema> &getEffectSchemas();
	PriorityMuxer* getMuxerInstance() { return &_muxer; }
	void setSourceAutoSelect(const bool state);
	bool setVisiblePriority(const int& priority);
	bool sourceAutoSelectEnabled();
	int getCurrentPriority() const;
	bool isCurrentPriority(const int priority) const;
	QList<int> getActivePriorities() const;
	const InputInfo getPriorityInfo(const int priority) const;
	QJsonDocument getSetting(const settings::type& type);
	const QJsonObject& getQJsonConfig();
	bool saveSettings(QJsonObject config, const bool& correct = false);
	ComponentRegister& getComponentRegister();
	void setNewComponentState(const hyperion::Components& component, const bool& state);
	std::map<hyperion::Components, bool> getAllComponents();
	int isComponentEnabled(const hyperion::Components& comp);
	void setLedMappingType(const int& mappingType);
	void setVideoMode(const VideoMode& mode);
	void start();
	void stop();

signals:
	void channelCleared(int priority);
	void allChannelsCleared();
	void compStateChangeRequest(const hyperion::Components component, bool enabled);
	void imageToLedsMappingChanged(const int& mappingType);
	void currentImage(const Image<ColorRgb> & image);
	void closing();
	void forwardJsonMessage(QJsonObject);
	void forwardSystemProtoMessage(const QString, const Image<ColorRgb>);
	void forwardV4lProtoMessage(const QString, const Image<ColorRgb>);
	void videoMode(const VideoMode& mode);
	void newVideoMode(const VideoMode& mode);
	void settingsChanged(const settings::type& type, const QJsonDocument& data);
	void adjustmentChanged();
	void effectListUpdated();
	void ledDeviceData(const std::vector<ColorRgb>& ledValues);
	void rawLedColors(const std::vector<ColorRgb>& ledValues);
	void finished();
	void started();

public slots:
	void update();

private slots:
	void handleVisibleComponentChanged(const hyperion::Components& comp);
	void handleSettingsUpdate(const settings::type& type, const QJsonDocument& config);
	void handleNewVideoMode(const VideoMode& mode) { _currVideoMode = mode; }

private:
	friend class HyperionDaemon;
	friend class HyperionIManager;
};
