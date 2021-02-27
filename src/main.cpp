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

SpanAccessory *bridge;
SpanService *bridgeService;

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

			if (!hardwareInfo) {
				nvs_flash_init();

				Serial.println("\n############################");

				// Mark hardwareinfo as found
				hardwareInfo = true;
				Serial.println("Writing device data to NV storage");
				// HB-00040538

				// Open NV Namespace
				Serial.printf("Opening storage with namespace %s ... ", HARDWARE_NAMESPACE);
				nvs_open(HARDWARE_NAMESPACE, NVS_READWRITE, &wf_nvs);
				Serial.println("Done!");

				// Parse datagram and convert value to non constant char
				const char *hub = doc["hub_sn"];
				// char hubSN[12] = const_cast<char[]>(&hub);
				// char hubSN[12];
				// memcpy(hubSN, &hub, sizeof(char*)); 
				// sprintf(hubSN,"%s",hub);
				// char hubSN[12] = "HB-0100121";										// Hardcoded value for testing
				// Save values
				nvs_set_str(wf_nvs, "hubSN", hub);
				nvs_commit(wf_nvs);

				// Retreive value for test
				// char sn[12] = "";
				Serial.println("Value Save!");
				char sn[12] = "";
				nvs_get_str(wf_nvs, "hubSN", sn, &len);
				Serial.printf("%s\n", sn);
				Serial.print("Saved and loaded the following value: ");
				Serial.println(sn);

				
				// Close nv storage
				Serial.println("Closing storage");
				nvs_close(wf_nvs);

				Serial.println("############################\n");
			}

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
				// long time_int = ob[0]; // 1588948614
				float w_s = ob[1]; // 0.27
				// int w_d = ob[2];	 // 144;
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
	nvs_flash_init();
	Serial.begin(115200);

	//////////// NV Storage Loading //////////////

	Serial.println("\n############################");

	// Initialize default values
	// const char *sn = "HB-1234"; 												// hardcoded value for testing.  Must be "const char *var"
	char hubSN[12] = "HB-00000001";	  											// Default from WeatherFlow API

	// Open NV Namespace
	nvs_open(HARDWARE_NAMESPACE, NVS_READWRITE, &wf_nvs);
	Serial.printf("Opening storage with namespace %s\n", HARDWARE_NAMESPACE);
	
	// If namespace exists, open it and set values otherwise, use hardcoded values
	Serial.printf("Looking for namespace %s.... ", HARDWARE_NAMESPACE);
	if (!nvs_get_str(wf_nvs, "hubSN", NULL, &len)) {
		Serial.println("Found!");

		// Set value from NV to char array
		nvs_get_str(wf_nvs, "hubSN", hubSN, &len);
		
		// Announce found value
		Serial.printf("Loaded value: %s\n", hubSN);

	}
	else {
		// Set values to hardcoded chars
		Serial.println("Not found :/\n");
		Serial.printf("Using hardcoded value: %s\n", hubSN);
	}

	// Close NV namespace
	Serial.printf("Closing namespace: %s\n", HARDWARE_NAMESPACE);
	nvs_close(wf_nvs);

	Serial.println("############################\n");


	//////////// NV Storage Loading //////////////

	homeSpan.begin(Category::Bridges, "WeatherFlow Hub", "WeatherFlow", "Tempest");
	// homeSpan.setLogLevel(2);
	homeSpan.setHostNameSuffix("WeatherFlow");
	homeSpan.enableOTA(true);
	homeSpan.setQRID("WFHK");
	homeSpan.setApSSID("TempestHK");
	homeSpan.setWifiCallback(connectUDPReceiver);	
	// const char *a;
	// sprintf(a,"%s",hubSN);
	char *b;
	b = &hubSN[0];
	char *c = hubSN;
	const char *a = b;
	Serial.println(hubSN);
	Serial.println(a);
	Serial.println(b);
	Serial.println(c);

	// const char *a = new char(hubSN);

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

	// cA->hapName = "12";
	// (*cA).setVal="5";
	// const char* x = "test";
	// cA->setVal(String("test"));

	// cB = bridgeService->Characteristics[2];
	// char *buff;
	// nameS->sprintfAttributes(buff, 0);
	// Serial.write(buff);
	// cB->sprintfAttributes(1);
}

void loop()
{
	//  char* buff;
	//  nameS->sprintfAttributes(buff, 0);
	//  Serial.write(buff);

	homeSpan.poll();
}
