#include <Main.h>

#define WIFI_SSID "assid"
#define WIFI_PASSWORD "apassword"

extern "C" {
#include "user_interface.h"
}

void discharge() {
  // loop to discharge C1 until it is actually discharged
  while(true) {
    Serial.printf("Discharging!\n");
    pinMode(WAKEUP_PIN, OUTPUT); // make output
    digitalWrite(WAKEUP_PIN, LOW); // discharge capacitor
    delay(10);
    pinMode(WAKEUP_PIN, INPUT); // prepare for reading
    delay(1);
    if (digitalRead(WAKEUP_PIN) == LOW) { // do it until C1 discharged
    	break;
    }
  }
  Serial.printf("Done discharging! (was low)\n");
  delay(10);
}



/**
 *   3.3V
 *  --+--
 *    |
 *   [R2]
 *    |                    R2=100k
 *    +------- WAKEUP_PIN  C1=100uF
 *    |                    R1=470
 *   [C1]                  t = (R1 + R2) * C1 ~= R2*C1 = 10000msec = 10sec
 *    |
 *   [R1]
 *    |
 *   -+- GND
 *    -
 *
 * Explanation:
 * When WAKEUP_PIN is in mode OUTPUT and set to logic low (~0v)
 * C1 discharges quickly through R1 (current must be < 12mA max allowed per GPIO).
 * Once C1 is discharged, WAKEUP_PIN is set to INPUT mode. C1 slowly charges through
 * R1 + R2. WAKEUP_PIN voltage goes slowly high until it makes an GPIO interrupt.
 *
 * Notes:
 * If C1 charged => WAKEUP_PIN is HIGH.
 * If C1 dischgd => WAKEUP_PIN is LOW (no voltage drop in C1) .
 * WAKEUP_PIN set to LOW to discharge C1 (and WAKEUP_PIN keeps LOW).
 */

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

  //Serial.setDebugOutput(true);
  Serial.printf("\n\nLog setup...\n");

  Serial.printf("Setup wifi...\n");

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attemptsLeft = 10;
  wl_status_t status;
  while (true) {
    Serial.printf(".");
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

}
void loop() {
  Serial.printf("\nLoop...\n");

  gpio_pin_wakeup_disable();
  wifi_fpm_set_sleep_type(NONE_SLEEP_T);
  discharge();
  wifi_station_disconnect();

  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func1); // Set wakeup callback
  gpio_pin_wakeup_enable(WAKEUP_PIN, WAKEUP_PIN_STATE); // when capacitor is charged
  wifi_fpm_do_sleep(0xFFFFFFF); // required to go to light sleep 1mA
  delay(1); // required to go to light sleep 1mA
}
