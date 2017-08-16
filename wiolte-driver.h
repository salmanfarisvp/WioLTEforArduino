#pragma once

#include <Arduino.h>
#include <Seeed_ws2812.h>
#include <vector>

#define WIOLTE_TCP	(WioLTE::SOCKET_TCP)
#define WIOLTE_UDP	(WioLTE::SOCKET_UDP)

class WioLTE
{
	/////////////////////////////////////////////////////////////////////
	// Stopwatch

private:
	class Stopwatch
	{
	private:
		unsigned long _BeginTime;
		unsigned long _EndTime;

	public:
		Stopwatch() : _BeginTime(0), _EndTime(0)
		{
		}

		void Start()
		{
			_BeginTime = millis();
			_EndTime = 0;
		}

		void Stop()
		{
			_EndTime = millis();
		}

		unsigned long ElapsedMilliseconds() const
		{
			if (_EndTime == 0) {
				return millis() - _BeginTime;
			}
			else {
				return _EndTime - _BeginTime;
			}
		}

	};

	/////////////////////////////////////////////////////////////////////
	// ModuleSerial

private:
	class ModuleSerial
	{
	private:
		void SerialInit() { Serial1.begin(115200); }
		bool SerialAvailable() const { return Serial1.available() >= 1 ? true : false; }
		byte SerialRead() { return Serial1.read(); }
		void SerialWrite(byte data) { Serial1.write(data); }
		void SerialWrite(const byte* data, int dataSize) { Serial1.write(data, dataSize); }

	private:
		std::vector<char> _LastResponse;

	public:
		ModuleSerial();

	private:
		void DiscardRead();
		bool WaitForAvailable(Stopwatch* sw, long timeout) const;
		int Read(byte* data, int dataSize);
		const char* ReadResponse(const char* match = NULL);
	public:
		bool ReadLine(char* data, int dataSize, long timeout);									// TODO

	public:
		void Init();
		void Write(const byte* data, int dataSize);
		void Write(const char* str);
		int Read(byte* data, int dataSize, long timeout);

		void WriteCommand(const char* command);
		const char* WaitForResponse(const char* waitResponse, long timeout, bool withoutDelim = false);
		const char* WriteCommandAndWaitForResponse(const char* command, const char* response, long timeout);
		bool WaitForResponse(const char* response, char* parameter, int parameterSize, long timeout);	// TODO

	};

	/////////////////////////////////////////////////////////////////////
	// WioLTE

private:
	static const int MODULE_PWR_PIN = 18;		// PB2
	static const int ANT_PWR_PIN = 28;			// PB12
	static const int ENABLE_VCCB_PIN = 26;		// PB10    

	static const int PWR_KEY_PIN = 36;			// PC4 
	static const int WAKEUP_IN_PIN = 32;		// PC0
	static const int AP_READY_PIN = 33;			// PC1
	static const int WAKEUP_DISABLE_PIN = 34;	// PC2
	static const int RESET_MODULE_PIN = 35;		// PC3
	static const int STATUS_PIN = 31;			// PB15
	static const int RGB_LED_PIN = 17;			// PB1

private:
	ModuleSerial _Module;
	WS2812 _Led;

public:
	enum SocketType {
		SOCKET_TCP,
		SOCKET_UDP,
	};

public:
	bool Reset();
	bool TurnOn();

public:
	WioLTE();
	void Init();
	void LedSetRGB(byte red, byte green, byte blue);
	void PowerSupplyLTE(bool on);
	void PowerSupplyGNSS(bool on);
	void PowerSupplyGrove(bool on);
	bool IsBusy() const;
	bool TurnOnOrReset();

	bool SendSMS(const char* dialNumber, const char* message);

	bool Activate(const char* accessPointName, const char* userName, const char* password);

	int SocketOpen(const char* host, int port, SocketType type);
	bool SocketSend(int connectId, const char* data);
	int SocketReceive(int connectId, byte* data, int dataSize);
	int SocketReceive(int connectId, char* data, int dataSize);
	bool SocketClose(int connectId);

};
