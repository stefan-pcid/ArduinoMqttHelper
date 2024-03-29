#include "ArduinoMqttHelper.h"

#ifndef ArduinoMqttHelperBufferSize
    #define ArduinoMqttHelperBufferSize   128
#endif

#ifndef ArduinoMqttHelperCore
    #define ArduinoMqttHelperCore         0
#endif

#ifdef ArduinoMqttHelperDebug
    #define MQTT_LOG(...)   printf(__VA_ARGS__)
#else
    #define MQTT_LOG(...)   {}
#endif

static char _mqttTopicBuffer[ArduinoMqttHelperBufferSize];
static char _mqttDataBuffer[ArduinoMqttHelperBufferSize];
static TaskHandle_t _MqttTaskHandle;
static WiFiClient espClient;
static PubSubClient client(espClient);
static const char* _wifiSsid;
static const char* _wifiPassword;
static const char* _mqttUrl;
static uint16_t _mqttPort;
static const char* _uniqueId;
static const char* _baseTopic;

static void Reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        MQTT_LOG("Attempting MQTT connection...\n");

        // Attempt to connect
        if (client.connect(_uniqueId))
        {
            MQTT_LOG("connected\n");
        }
        else
        {
            MQTT_LOG("failed, rc=%d try again in 5 seconds\n", client.state());
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}


static void MqttTask(void* parameter)
{
    MQTT_LOG("Connecting to: %s\n", _wifiSsid);
    WiFi.begin(_wifiSsid, _wifiPassword);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        MQTT_LOG(".");
    }

    MQTT_LOG("\nWiFi connected\n");
    MQTT_LOG("IP address: %s\n", WiFi.localIP().toString());    

    client.setServer(_mqttUrl, _mqttPort);

    while(true)
    {
        if (!client.connected())
        {
            Reconnect();
        }
        client.loop();
    }
}

void MqttHelperInit(const char* wifiSsid, const char* wifiPassword, const char* mqttUrl, uint16_t mqttPort, const char* uniqueId, const char* baseTopic)
{
    _wifiSsid = wifiSsid;
    _wifiPassword = wifiPassword;
    _mqttUrl = mqttUrl;
    _mqttPort = mqttPort;
    _uniqueId = uniqueId;
    _baseTopic = baseTopic;
    
    xTaskCreatePinnedToCore(
        MqttTask, /* Function to implement the task */
        "MqttTask", /* Name of the task */
        10000,  /* Stack size in words */
        NULL,  /* Task input parameter */
        0,  /* Priority of the task */
        &_MqttTaskHandle,  /* Task handle. */
        ArduinoMqttHelperCore); /* Core where the task should run */
}

bool MqttHelperPublish(const char* topic, const char* format, ...)
{
    if (MqttHelperState() != ArduinoMqttStateConnected)
    {
        return false;
    }
    
    bool success = false;
    va_list args;
    va_start(args, format);
    int len = vsnprintf(_mqttDataBuffer, sizeof(_mqttDataBuffer), format, args);
    if(len > 0) 
    {
        snprintf(_mqttTopicBuffer, sizeof(_mqttTopicBuffer), "%s/%s", _baseTopic, topic);
        success = client.publish(_mqttTopicBuffer, _mqttDataBuffer);

        MQTT_LOG("Publish on: %s - value: %s\n", _mqttTopicBuffer, _mqttDataBuffer);
    }
    va_end(args);
    return success;
}

bool MqttHelperPublishFloat(const char* topic, float value)
{
    return MqttHelperPublish(topic, "%f", value) > 0;
}

bool MqttHelperPublishInt(const char* topic, int value)
{
    return MqttHelperPublish(topic, "%d", value) > 0;
}

ArduinoMqttState MqttHelperState(void)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return ArduinoMqttStateWifiDisconnected;
    }
    if (!client.connected())
    {
        return ArduinoMqttStateMqttDisconnected;
    }

    return ArduinoMqttStateConnected;
}
