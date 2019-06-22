/**
 * This file contains:
 * - entry point for arduino programs (setup and loop functions)
 * - declaration of HW specific functions (the definition is in a dedicated file)
 * - other functions that are not defined by HW specific but that use them, that are required by the module
 *   (so that it can be passed as callback).
 * The rest should be put in Module so that they can be tested regardless of the HW used behind.
 */
#include <Main.h>

extern "C" {
#include "user_interface.h"
}

void fpm_wakup_cb_func1(void)
{
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

  Serial.printf("Setup wifi\n");

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin("assid", "apassword");

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
  Serial.printf("Loop...n");
  //wifi_station_disconnect();
  //wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  Serial.printf("LS...n");
  wifi_fpm_open();
  Serial.printf("LS...n");
  gpio_pin_wakeup_enable(GPIO_ID_PIN(0), GPIO_PIN_INTR_LOLEVEL);
  Serial.printf("0S...n");
  gpio_pin_wakeup_enable(GPIO0_PIN, GPIO_PIN_INTR_LOLEVEL);
  Serial.printf("1S...n");
  //wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1); // Set wakeup callback
  wifi_fpm_do_sleep(0xFFFFFFF); // required to go to light sleep 1mA
  Serial.printf("2S...n");
  wifi_fpm_close();
  Serial.printf("3S...n");
  delay(2000); // required to go to light sleep 1mA
}
