#ifndef ARDUINO_MQTT_HELPER
#define ARDUINO_MQTT_HELPER

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

typedef enum 
{
    ArduinoMqttStateWifiDisconnected, 
    ArduinoMqttStateMqttDisconnected, 
    ArduinoMqttStateConnected
} ArduinoMqttState;

/// @brief Initialize Wifi + MQTT connection
/// @param wifiSsid wifi name
/// @param wifiPassword wifi password
/// @param mqttUrl Mqtt server url (eg. "broker.hivemq.com")
/// @param mqttPort Mqtt server port (eg. 1883)
/// @param uniqueId Mqtt unique id. May not included spaces
/// @param baseTopic Mqtt base topic, this path will be added as prefix to all published values. May not included spaces
void MqttHelperInit(const char* wifiSsid, const char* wifiPassword, const char* mqttUrl, uint16_t mqttPort, const char* uniqueId, const char* baseTopic);

/// @brief Send a value to the specified mqtt topic
/// @param topic topic to send the value to. May not included spaces. note: data will be published on "baseTopic/topic"
/// @param format format specifier to write
/// @return true on success
bool MqttHelperPublish(const char* topic, const char* format, ...);

/// @brief Send an floating point value to a specified topic
/// @param topic topic to send the value to. May not included spaces. note: data will be published on "baseTopic/topic"
/// @param value value to send
/// @return true on success
bool MqttHelperPublishFloat(const char* topic, float value);

/// @brief Send an integer value to a specified topic
/// @param topic topic to send the value to. May not included spaces. note: data will be published on "baseTopic/topic"
/// @param value value to send
/// @return true on success
bool MqttHelperPublishInt(const char* topic, int value);

/// @brief Get the current state of the Mqtt Helper
/// @return current connection state
ArduinoMqttState MqttHelperState(void);

#endif