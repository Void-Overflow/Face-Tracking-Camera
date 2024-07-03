#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <windows.h>

class SerialCOM
{
private:
	HANDLE _hSerial;

	void _configureSerialPort();
	void _resetDevice();

	char _readBuffer[128];
	DWORD _bytesRead;
public:
	bool connect(LPCWSTR port);
	bool autoConnect(const char* connectionMsg);

	const char* read();
	bool write(const char* msg);

	void close();
};

#endif 

