#include<H4Plugins.h>
H4_USE_PLUGINS(0,10,true) // Serial baud rate, Q size, SerialCmd autostop
//
//    Configuration
//
const char* WIFI_SSID="XXXXXXXX";
const char* WIFI_PASS="XXXXXXXX";
const char* MQTT_SERVER="192.168.1.4";
const int   MQTT_PORT=1883;

H4P_WiFi h4wifi(WIFI_SSID,WIFI_PASS);
H4P_AsyncMQTT h4mqtt(MQTT_SERVER,MQTT_PORT);
H4P_BinarySwitch h4onof(RELAY_BUILTIN,ACTIVE_HIGH,OFF);
H4P_UPNPServer h4upnp; // friendly name defaults to "upnp <chip id>"
H4P_MultiFunctionButton h4mfb(BUTTON_BUILTIN,INPUT,ACTIVE_LOW,15);
