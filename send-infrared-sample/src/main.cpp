#include <Arduino.h>
#include "M5Atom.h"
#include <IRac.h>

/// 赤外線送信に関するもの
const auto IR_pin = 12;               // 赤外線LEDのピン
IRHitachiAc424 ac_controller(IR_pin); // 送信オブジェクト

void setup() {
  M5.begin(true, false, true);
  M5.dis.clear();
  ac_controller.begin(); // 赤外線照射できるようにする
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

  // ボタンが押されたらエアコンONする
  if (M5.Btn.isPressed()) {
    ac_on();
  }

  delay(500);
  M5.update();
}