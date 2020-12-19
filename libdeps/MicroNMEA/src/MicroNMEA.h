#ifndef MICRONMEA_H
#define MICRONMEA_H

#define MICRONMEA_VERSION "2.0.3"
#include <limits.h>

/*
 * A simple Arduino class to process MicroNMEA sentences from GPS and GNSS
 * receivers.
 */

#include <Arduino.h>

class MicroNMEA {
public:

	static const char* skipField(const char* s);
	static unsigned int parseUnsignedInt(const char* s, uint8_t len);
	static long parseFloat(const char* s, uint8_t log10Multiplier,
						   const char** eptr = NULL);
	static long parseDegreeMinute(const char* s, uint8_t degWidth,
								  const char** eptr = NULL);
	static const char* parseToComma(const char* s, char *result = NULL,
									int len = 0);
	static const char* parseField(const char* s, char *result = NULL,
								  int len = 0);
	static const char* generateChecksum(const char* s, char* checksum);
	static bool testChecksum(const char* s);

	// Write MicroNMEA sentence to oputput stream. Sentence must start with
	// '$', the checksum and <CR><NL> terminators will be appended
	// automatically.
	static Stream& sendSentence(Stream &s, const char* sentence);

	// Object with no buffer allocated, must call setBuffer later
	MicroNMEA(void);

	// User must decide and allocate the buffer
	MicroNMEA(void* buffer, uint8_t len);

	void setBuffer(void* buf, uint8_t len);

	// Clear all fix information. isValid() will return false, Year,
	// month and day will all be zero. Hour, minute and second time will
	// be set to 99. Speed, course and altitude will be set to
	// LONG_MIN; the altitude validity flag will be false. Latitude and
	// longitude will be set to 999 degrees.
	void clear(void);

	// Navigation system, N=GNSS, P=GPS, L=GLONASS, A=Galileo, '\0'=none
	char getNavSystem(void) const {
		return _navSystem;
	}

	uint8_t getNumSatellites(void) const {
		return _numSat;
	}

	// Horizontal dilution of precision, in tenths
	uint8_t getHDOP(void) const {
		return _hdop;
	}

	// Validity of latest fix
	bool isValid(void) const {
		return _isValid;
	}

	// Latitude in millionths of a degree. North is positive.
	long getLatitude(void) const {
		return _latitude;
	}

	// Longitude in millionths of a degree. East is positive.
	long getLongitude(void) const {
		return _longitude;
	}

	// Altitude in millimetres.
	bool getAltitude(long &alt) const {
		if (_altitudeValid)
			alt = _altitude;
		return _altitudeValid;
	}

	uint16_t getYear(void) const {
		return _year;
	}

	uint8_t getMonth(void) const {
		return _month;
	}

	uint8_t getDay(void) const {
		return _day;
	}

	uint8_t getHour(void) const {
		return _hour;
	}

	uint8_t getMinute(void) const {
		return _minute;
	}

	uint8_t getSecond(void) const {
		return _second;
	}

	uint8_t getHundredths(void) const {
		return _hundredths;
	}

	long getSpeed(void) const {
		return _speed;
	}

	long getCourse(void) const {
		return _course;
	}

	bool process(char c);

	void setBadChecksumHandler(void (*handler)(const MicroNMEA& nmea)) {
		_badChecksumHandler = handler;
	}

	void setUnknownSentenceHandler(void (*handler)(const MicroNMEA& nmea)) {
		_unknownSentenceHandler = handler;
	}

	// Current MicroNMEA sentence.
	const char* getSentence(void) const {
		return _buffer;
	}

	// Talker ID for current MicroNMEA sentence
	char getTalkerID(void) const {
		return _talkerID;
	}

	// Message ID for current MicroNMEA sentence
	const char* getMessageID(void) const {
		return (const char*)_messageID;
	}


protected:
	static inline bool isEndOfFields(char c) {
		return c == '*' || c == '\0' || c == '\r' || c == '\n';
	}

	const char* parseTime(const char* s);
	const char* parseDate(const char* s);

	bool processGGA(const char *s);
	bool processRMC(const char* s);

private:
	// Sentence buffer and associated pointers
	// static const uint8_t _bufferLen = 83; // 82 + NULL
	// char _buffer[_bufferLen];
	uint8_t _bufferLen;
	char* _buffer;
	char *_ptr;

	// Information from current MicroNMEA sentence
	char _talkerID;
	char _messageID[6];

	// Variables parsed and kept for user
	char _navSystem;
	bool _isValid;
	long _latitude, _longitude; // In millionths of a degree
	long _altitude; // In millimetres
	bool _altitudeValid;
	long _speed, _course;
	uint16_t _year;
	uint8_t _month, _day, _hour, _minute, _second, _hundredths;
	uint8_t _numSat;
	uint8_t _hdop;

	void (*_badChecksumHandler)(const MicroNMEA &nmea);
	void (*_unknownSentenceHandler)(const MicroNMEA &nmea);

};


#endif
