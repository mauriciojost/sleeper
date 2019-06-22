#include <Main.h>


#define WIFI_SSID "assid"
#define WIFI_PASSWORD "apassword"


extern "C" {
#include "user_interface.h"
}

void fpm_wakup_cb_func1(void)
{
  Serial.printf("\n\nWakeup!\n\n");
  wifi_fpm_close();
  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();
}

void setup() {

  // Let HW startup
  delay(10);

  Serial.begin(115200); // Initialize serial port

  Serial.setDebugOutput(true);
  Serial.printf("\n\nLog setup...\n");

  Serial.printf("Setup wifi...\n");

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attemptsLeft = 4;
  wl_status_t status;
  while (true) {
    Serial.printf("Connecting...\n");
    delay(4000);
    status = WiFi.status();
    attemptsLeft--;
    if (status == WL_CONNECTED) {
      Serial.printf("Connected! %s\n", WiFi.localIP().toString().c_str());
      return;
    }
    if (attemptsLeft < 0) {
      Serial.printf("Connection failed %d\n",  status);
      return;
    }
  }
  Serial.printf("Done setup\n");

}
void loop() {
  Serial.printf("\nLoop...\n");
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1); // Set wakeup callback
  gpio_pin_wakeup_enable(GPIO_ID_PIN(0), GPIO_PIN_INTR_LOLEVEL);
  gpio_pin_wakeup_enable(GPIO0_PIN, GPIO_PIN_INTR_LOLEVEL);
  wifi_fpm_do_sleep(0xFFFFFFF); // required to go to light sleep 1mA
  delay(1); // required to go to light sleep 1mA
}
