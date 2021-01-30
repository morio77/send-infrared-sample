#include <Arduino.h>
#include "M5Atom.h"
#include <IRac.h>
#include "WiFi.h"
#include <FirebaseESP32.h>

/// 赤外線送信に関するもの
const auto IR_pin = 12;               // 赤外線LEDのピン
IRHitachiAc424 ac_controller(IR_pin); // 送信オブジェクト

/// Firebaseに関するもの
FirebaseData firebaseData;
const char* const firebase_host = "Realtime Databaseのホスト名（https://は除く）";
const char* const firebase_auth = "データベースシークレット";

const char* const is_power_on_key = "isPowerOn";
const char* const is_sent_IR_key = "isSentIR";

/// お家のWi-Fiに関するもの
const char* const ssid = "Wi-FiのSSID";
const char* const password = "Wi-Fiのパスワード";


void setup() {
  M5.begin(true, false, true);
  M5.dis.clear();             // LEDを全消し
  WiFi.begin(ssid, password); // Wi-Fiに接続
  ac_controller.begin();      // 赤外線照射できるようにする

  // 接続試行中はLEDを点滅させる
  auto is_led_on = false;
  while (WiFi.status() != WL_CONNECTED) {
    is_led_on ? M5.dis.clear() : M5.dis.drawpix(0, 0x00f000); 
    is_led_on = !is_led_on;
    delay(200);
  }
  M5.dis.clear();

  delay(500); // なんとなく0.5秒くらい待ってみる。

  // Firebase接続
  Firebase.begin(firebase_host, firebase_auth);
  Firebase.reconnectWiFi(true);
}


/// <summary>
/// 12番ピンから赤外線を照射します。
/// </summary>
/// <param name="ac_controller">設定値が格納されたIRHitachiAc424クラス</param>
/// <param name="repeat_count">照射回数</param>
void send_IR(IRHitachiAc424 ac_controller, const uint16_t repeat_count = 1) {

  // 照射（照射中は別のLEDを光らせる）
  M5.dis.drawpix(12, 0x00f000);
  ac_controller.send(repeat_count); // (repeat_count + 1) 回照射する
  M5.dis.clear();

  // Firebaseの送信済みフラグ(isSentIR)を立てる
  Firebase.setBool(firebaseData, is_sent_IR_key, true);
}


/// <summary>
/// エアコンをONします。
/// </summary>
void ac_on() {
  ac_controller.on();
  send_IR(ac_controller);
}


/// <summary>
/// エアコンをOFFします。
/// </summary>
void ac_off() {
  ac_controller.off();
  send_IR(ac_controller);
}


void loop() {

  // isSentIRの値を取得する
  Firebase.getBool(firebaseData, is_sent_IR_key);
  auto is_sent_IR = firebaseData.boolData();

  // isSendIRがfalseだったら、isPowerOnの値によってエアコンON/OFFの赤外線を照射する
  if (!is_sent_IR) {
    Firebase.getBool(firebaseData, is_power_on_key);
    firebaseData.boolData() ? ac_on() : ac_off();
  }

  delay(500);
  M5.update();
}