#include "FaceDetector.h"
#include "SerialCOM.h"

#include <iostream>
#include <windows.h>

std::atomic_int xCoord = 0, yCoord = 0;
std::atomic_bool isRunningSerialThread = false;

void deviceCOM() {
	SerialCOM device;
	while (!device.autoConnect("Connected to Face Tracking Camera Receiver!")) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	};

	while (isRunningSerialThread) {
		const char* read = device.read();
		if (read == "")
			device.autoConnect("Connected to Face Tracking Camera Receiver!");
		else {
			device.write(std::string(std::to_string(xCoord) + "|" + std::to_string(yCoord) + '\n').c_str());
			std::cout << read << '\n';
		}
	}
	device.close();
}

int main() {
	FaceDetector detect;
	if (!detect.load("libs/opencv/install/etc/haarcascades/haarcascade_frontalface_alt2.xml"))
		return -1;

	std::thread serialCOMThread(deviceCOM);

	while (true) {
		detect.run();
		xCoord = detect.dx;
		yCoord = detect.dy;
		isRunningSerialThread = true;
	}
	isRunningSerialThread = false;
	detect.stop();

	return 0;
}