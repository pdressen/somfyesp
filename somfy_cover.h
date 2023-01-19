#include "esphome.h"
#include <string>
#include <RollingCodeStorage.h>
#include "EEPROMRollingCodeStorage.h"
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <SomfyRemote.h>

#define CC1101_FREQUENCY 433.42

class SomfyESPCover : public Cover {
private:
  SomfyRemote *remote;
  RollingCodeStorage *storage;

public:
  SomfyESPCover(const std::string &name, uint32_t remoteCode, int eepromAddress, int emitterPin)
      : Cover(name) {
    device_class_override_ = std::string("shutter");
    storage = new EEPROMRollingCodeStorage(eepromAddress, false);
    remote = new SomfyRemote(emitterPin, remoteCode, storage);
    ESP_LOGI("somfy", "# %s #", name_.c_str());
    ESP_LOGI("somfy", "EEPROM %d", eepromAddress);
  }

  CoverTraits get_traits() override {
    auto traits = CoverTraits();
    traits.set_is_assumed_state(true);
    traits.set_supports_position(false);
    traits.set_supports_tilt(false);
    return traits;
  }

  void sendCC1101Command(Command command) {
    ELECHOUSE_cc1101.SetTx();
    remote->sendCommand(command);
    ELECHOUSE_cc1101.setSidle();
  }

  void control(const CoverCall &call) override {
    if (call.get_position().has_value()) {
      float pos = *call.get_position();

      if (pos == COVER_OPEN) {
        ESP_LOGI("somfy", "%s OPEN", name_.c_str());
        sendCC1101Command(Command::Up);
      } else if (pos == COVER_CLOSED) {
        ESP_LOGI("somfy", "%s CLOSE", name_.c_str());
        sendCC1101Command(Command::Down);
      } else {
        ESP_LOGW("somfy", "%s unsupported position %f", name_.c_str(), pos);
      }

      this->position = pos;
      this->publish_state();
    }

    if (call.get_stop()) {
      ESP_LOGI("somfy", "%s STOP", name_.c_str());
      sendCC1101Command(Command::My);
    }
  }

  void program() {
    ESP_LOGI("somfy", "%s PROG", name_.c_str());
    sendCC1101Command(Command::Prog);
  }
};

class SomfyESPProgButton : public Button {

private:
  SomfyESPCover *cover;

protected:
  void press_action() override {
    cover->program();
  }

public:
  SomfyESPProgButton(SomfyESPCover* cover)
      : Button(cover->get_name() + std::string(" PROG")), cover(cover) {}
};

class SomfyESPRemote : public Component {
private:
  int emitterPin;

public:
  static void TrySaveRollingCodes() {
    if (EEPROM.commit()) {
      ESP_LOGI("somfy", "Saved rolling codes to flash");
    } else {
      ESP_LOGE("somfy", "Error saving rolling codes to flash");
    }
  }

  static void DumpRollingCodes() {
    ESP_LOGI("somfy", "Dumping rolling codes");
    for (int i = 0; i < SOMFY_CHANNEL_COUNT; ++i)
    {
      int addr = 2 * i;
      uint16_t code = 0;
      EEPROM.get(addr, code);
      ESP_LOGI("somfy", "somfy%d: %u", i, code);
    }
  }

  std::vector<esphome::cover::Cover *> covers;
  std::vector<esphome::button::Button *> progButtons;

  SomfyESPRemote(int emitterPin) : emitterPin(emitterPin) {}

  void setup() override {
    // initialize EEPROM
    EEPROM.begin((2 * SOMFY_CHANNEL_COUNT));   // 2 bytes (uint16_t) per channel

    // configure emitter pin as OUTPUT
    pinMode(emitterPin, OUTPUT);
    digitalWrite(emitterPin, LOW);

    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(CC1101_FREQUENCY);
  }

  void add_cover(const std::string &name, uint32_t remoteCode) {
    int eepromAddress = covers.size() * 2;  // every remote needs 2 bytes (uint16_t) of storage
    
    // cover
    auto cover = new SomfyESPCover(name, remoteCode, eepromAddress, emitterPin);
    App.register_cover(cover);
    covers.push_back(cover);

    // prog button
    auto progButton = new SomfyESPProgButton(cover);
    App.register_button(progButton);    
    progButtons.push_back(progButton);
  }
};
