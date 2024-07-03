# Face-Tracking-Camera
An RP2040 based mechanical facial tracker with a C++ interface utilizing OpenCV to physically track faces detected in the user's webcam using stepper and servo motors.


THIS PROJECT IS CURRENTLY UNDER DEVELOPMENT. ALL ASPECTS HAVE NOT BEEN DEVELOPED YET, AND MUCH OF THE DESCRIPTION WILL NOT WORK AS OF NOW, NOR WILL IT LIKELY ACTUALLY EXIST IN THE CONTENTS OF THIS REPOSITORY.

Description: 

This project uses the Rasperry Pi Pico microcontroller to run Arduino code which uses UART to communicate with my C++ program. It receives the determined coordinates of a detected face in the webcam with the center of the camera as the origin. It then maps the values from those coordinates to a stepper motor to move it horizontally and a servo to vertically move the camera. The webcam should be mounted on top of the 3d printed base. A slip ring should be used to prevent the wires above the stepper from being tangled. The C++ interface was written in Visual Studio, and it uses OpenCV to detect and track faces from a pretrained haarcascade file. It additionally uses the Windows.h library to communicate with the detected RPi. I have designed my own libraries, SerialCOM and FaceDetect, which respectively open, establish, and interact with a device at a certain serial port, as well as use OpenCV to detect and receive coordinates of a face in the webcam. I addionally created my own function, autoConnect, in the   SerialCOM library which searches all serial ports on the windows computer from COM0-COM30 for a connection message to verify it is the desired device. This message should be sent at all times by the receiver, in this case the RPi Pico.                       

How to Run: 

Since I utilized Windows.h, this project only runs on x64 windows devices. To run this, build the circut according to the schematic in the project. Print my 3d printed design and use a slip ring to prevent the camera and servo from being tangled. See the attached video in this repository to see how it works. This repository doesn't include the OpenCV library as it is too large. To successfully run this program, install OpenCV as well as its extra modules, compile and bulid them using MINGW, go to Software Interface/Face Tracking Camera, create a folder named libs, and put the library in a folder called opencv. Make sure to add the environment variables for opencv as it may not work without it. If it doesn't work, try opening the solution file in Visual Studio and adjuting the library paths, and then rebuild. Once it's set up, run the executable, and it should open up a terminal and after a couple seconds a window from your camera. It should then show you the connection being established with your device on the detected COM port, show a blue square around your face, and the device should then start moving  its servo and stepper motors to center your face within the center of the camera.

Credits: 

This project was all created and designed by Comyar Dehlavi.
OpenCV library was used to detect faces from the webcam, with MINGW being used to compile it.
Visual Studio and Visual Studio code were the development of this as well, with the Arduino extension being used in VS Code to compile for the RPi Pico.


