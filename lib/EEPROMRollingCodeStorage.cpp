
#include "EEPROMRollingCodeStorage.h"

EEPROMRollingCodeStorage::EEPROMRollingCodeStorage(int address) : address(address) {}

EEPROMRollingCodeStorage::EEPROMRollingCodeStorage(int address, bool autoCommit) : address(address), autoCommit(autoCommit) {}

uint16_t EEPROMRollingCodeStorage::nextCode() {
	uint16_t code;
	EEPROM.get(address, code);
#ifdef DEBUG
	Serial.print("Rolling code: ");
	Serial.println(code);
#endif
	EEPROM.put(address, (uint16_t)(code + 1));
#if defined(ESP32) || defined(ESP8266)
	if (autoCommit) {
		EEPROM.commit();
	}
#endif
	return code;
}
