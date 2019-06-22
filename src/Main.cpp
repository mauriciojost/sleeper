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
  Serial.printf("Log setup...\n");

  delay(3000);
  //WiFi.persistent(false);

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
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1); // Set wakeup callback
  wifi_fpm_do_sleep(0xFFFFFFF);
  delay(2000);
}
