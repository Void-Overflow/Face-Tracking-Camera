#include "SerialCOM.h"

#include <iostream>
#include <thread>
#include <string>
#include <chrono>

void SerialCOM::_configureSerialPort()
{
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	if (!GetCommState(_hSerial, &dcbSerialParams)) {
		std::cerr << "Error getting state: " << GetLastError() << std::endl;
		return;
	}

	dcbSerialParams.BaudRate = CBR_115200;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	if (!SetCommState(_hSerial, &dcbSerialParams)) {
		std::cerr << "Error setting serial port state: " << GetLastError() << std::endl;
		return;
	}

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(_hSerial, &timeouts)) {
		std::cerr << "Error setting timeouts: " << GetLastError() << std::endl;
		return;
	}

	PurgeComm(_hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

void SerialCOM::_resetDevice()
{
	EscapeCommFunction(_hSerial, CLRDTR);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	EscapeCommFunction(_hSerial, SETDTR);
}

bool SerialCOM::connect(LPCWSTR port)
{
	_hSerial = CreateFile(port,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (_hSerial == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		std::cerr << "Error opening serial port: " << dwError << std::endl;

		if (dwError == ERROR_FILE_NOT_FOUND) {
			std::cerr << "The specified COM port does not exist." << std::endl;
		}
		else if (dwError == ERROR_ACCESS_DENIED) {
			std::cerr << "Access denied. The COM port might be in use by another application." << std::endl;
		}

		return false;
	}

	_configureSerialPort();
	_resetDevice();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	return true;
}
char* removeNonPrintable(const char* str) {
	size_t len = strlen(str);
	char* result = new char[len + 1];
	size_t j = 0;

	for (size_t i = 0; i < len; ++i) {
		if (isprint(static_cast<unsigned char>(str[i]))) {
			result[j++] = str[i];
		}
	}

	result[j] = '\0';
	return result;
}

bool SerialCOM::autoConnect(const char* connectionMsg)
{
	for (int i = 0; i <= 30; i++) {
		std::string port = "\\\\.\\COM" + std::to_string(i);
		_hSerial = CreateFile(std::wstring(port.begin(), port.end()).c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (_hSerial != INVALID_HANDLE_VALUE) {
			_configureSerialPort();
			_resetDevice();

			std::chrono::steady_clock::time_point startConnection = std::chrono::steady_clock::now();
			bool connectedFlag = true;
			while (std::string(removeNonPrintable(read())) != std::string(removeNonPrintable(connectionMsg))) {
				if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startConnection).count() >= 1) {
					connectedFlag = false;
					break;
				}
			}
			if (connectedFlag == true) {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				std::cout << "Successfully established connection with device at COM" << i << ".\n";
				return true;
			}
		}
	};
	std::cout << "Couldn't detect any device from ports COM0 - COM30.\n";
	return false;
}

const char* SerialCOM::read()
{
	if (ReadFile(_hSerial, _readBuffer, sizeof(_readBuffer) - 1, &_bytesRead, NULL)) {
		if (_bytesRead > 0)
			_readBuffer[_bytesRead] = '\0'; // Null-terminate the string
	}
	else {
		std::cerr << "Error reading from serial port: " << GetLastError() << std::endl;
		return "";
	}
	return _readBuffer;
}

bool SerialCOM::write(const char* msg)
{
	DWORD bytesWritten;
	if (!WriteFile(_hSerial, msg, strlen(msg), &bytesWritten, NULL)) {
		std::cerr << "Error writing to serial port: " << GetLastError() << std::endl;
		return false;
	}

	FlushFileBuffers(_hSerial);
	return true;
}

void SerialCOM::close()
{
	CloseHandle(_hSerial);
}
