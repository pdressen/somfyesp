#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#include <RollingCodeStorage.h>

/// @brief Stores the rolling codes in the EEPROM, the codes require two bytes.
///  When using with ESP32 or ESP8266 don't forget to initialize the EEPROM.
///
///  This class has been overridden to add the autoCommit constructor parameter.
class EEPROMRollingCodeStorage : public RollingCodeStorage {
private:
	int address;
	bool autoCommit = true;

public:
	EEPROMRollingCodeStorage(int address);
	
	/// @brief Creates a new rolling code storage at the given EEPROM address.
	/// On ESP8266 and ESP32 executes EEPROM.commit() after every new code by default.
	/// This can be disabled by setting autoCommit to false. Then EEPROM.commit() must be called
	/// elsewhere to persist rolling codes between power cycles (eg. in a regular time interval).
	/// @param address The EEPROM address to use for this rolling code storage (each storage takes 2 bytes)
	/// @param autoCommit  If EEPROM.commit() should be called automatically on ESP8266/ESP32
	EEPROMRollingCodeStorage(int address, bool autoCommit);

	uint16_t nextCode() override;
};
