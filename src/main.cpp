#include <Arduino.h>
#include "HomeSpan.h"
#include "AsyncUDP.h"
#include <ArduinoJson.h>

#include <nvs_flash.h>

#define HARDWARE_NAMESPACE "deviceData"

AsyncUDP udp;

const int port = 50222;

SpanCharacteristic *temp;
SpanCharacteristic *rh;
SpanCharacteristic *light;
SpanCharacteristic *rain;
SpanCharacteristic *wind;
SpanCharacteristic *lightning;
SpanCharacteristic *snChar;

bool hardwareInfo = false;
static nvs_handle wf_nvs;
size_t len;

void connectUDPReceiver()
{
	if (udp.listen(port))
	{
		udp.onPacket([](AsyncUDPPacket packet) {
			StaticJsonDocument<384> doc;

			DeserializationError error = deserializeJson(doc, packet.data(), packet.length());

			if (error)
			{
				Serial.print(F("deserializeJson() failed: "));
				Serial.println(error.f_str());
				return;
			}
			const char *type = doc["type"];

			if (strcmp(type, "obs_st") == 0)
			{

				JsonArray obs = doc["obs"][0];
				double t = obs[7];
				double rhVal = obs[8];
				int l = obs[9];
				temp->setVal(t);
				rh->setVal(rhVal);
				light->setVal((float) l);

				Serial.print("Temperature: ");
				Serial.println(t);
				Serial.print("Humidity: ");
				Serial.println(rhVal);
				Serial.print("Lux: ");
				Serial.println(l);
			}

			if (strcmp(type, "rapid_wind") == 0)
			{
				JsonArray ob = doc["ob"];
				float w_s = ob[1];
				if (w_s > 10)
				{
					wind->setVal(0);
				}
			}

			if (strcmp(type, "evt_strike") == 0)
			{
				if (lightning->timeVal())
				{
					lightning->setVal(0);
				}
				if (lightning->timeVal() > 5000)
				{
					lightning->setVal(1);
				}
				if (lightning->timeVal() > 1000)
				{
					lightning->setVal(2);
				}
			}

			if (strcmp(type, "evt_precip") == 0)
			{
				if (lightning->timeVal() > 0)
				{
					rain->setVal(0);
				}
			}
		});
	}
}

void setup() {
	
	Serial.begin(115200);
	homeSpan.begin(Category::Bridges, "WeatherFlow Hub", "WeatherFlow", "Tempest");
	homeSpan.setHostNameSuffix("WeatherFlow");
	homeSpan.enableOTA(true);
	homeSpan.setQRID("WFHK");
	homeSpan.setApSSID("WeatherFlowHK");
	homeSpan.setApPassword("weatherflow");
	homeSpan.setWifiCallback(connectUDPReceiver);	
	
	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Name("WeatherFlow Hub");
	new Characteristic::Manufacturer("WeatherFlow");
	new Characteristic::SerialNumber("HB-00000001");           // const char *var
	new Characteristic::Model("Tempest");
	new Characteristic::FirmwareRevision("1.0");
	new Characteristic::Identify();
	new Service::HAPProtocolInformation();
	new Characteristic::Version("1.1.0");

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Name("Temperature");
	new Characteristic::Model("Tempest");
	new Characteristic::Manufacturer("WeatherFlow");
	new Characteristic::SerialNumber("ST-00000512");
	new Characteristic::FirmwareRevision("1.0");
	new Characteristic::Identify();

	new Service::TemperatureSensor();
	new Characteristic::Name("Temperature");
	temp = new Characteristic::CurrentTemperature(20);
	new SpanRange(-50, 100, 1);

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Name("Humidity");
	new Characteristic::Model("Tempest");
	new Characteristic::Manufacturer("WeatherFlow");
	new Characteristic::SerialNumber("ST-00000512");
	new Characteristic::FirmwareRevision("1.0");
	new Characteristic::Identify();

	new Service::HumiditySensor();
	new Characteristic::Name("Humidity");
	rh = new Characteristic::CurrentRelativeHumidity(50);

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Name("Light Sensor");
	new Characteristic::Model("Tempest");
	new Characteristic::Manufacturer("WeatherFlow");
	new Characteristic::SerialNumber("ST-00000512");
	new Characteristic::FirmwareRevision("1.0");
	new Characteristic::Identify();

	new Service::LightSensor();
	new Characteristic::Name("Light");
	light = new Characteristic::CurrentAmbientLightLevel();

	new SpanAccessory();
	new Service::AccessoryInformation();
	new Characteristic::Name("Events");
	new Characteristic::Model("Tempest");
	new Characteristic::Manufacturer("WeatherFlow");
	new Characteristic::SerialNumber("ST-00000512");
	new Characteristic::FirmwareRevision("1.0");
	new Characteristic::Identify();

	new Service::StatelessProgrammableSwitch();
	new Characteristic::Name("Wind Sensor");
	wind = new Characteristic::ProgrammableSwitchEvent();

	new Service::StatelessProgrammableSwitch();
	new Characteristic::Name("Lightning Sensor");
	lightning = new Characteristic::ProgrammableSwitchEvent();

	new Service::StatelessProgrammableSwitch();
	new Characteristic::Name("Rain Sensor");
	rain = new Characteristic::ProgrammableSwitchEvent();
}

void loop()
{
	homeSpan.poll();
}
