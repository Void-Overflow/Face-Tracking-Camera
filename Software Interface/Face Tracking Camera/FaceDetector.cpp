#include "FaceDetector.h"
#include <iostream>

bool FaceDetector::load(std::string cascadePath)
{
	if (!_face_cascade.load(cascadePath)) {
		std::cerr << "Error loading face cascade from " << cascadePath << std::endl;
		return false;
	}

	_cap = cv::VideoCapture(0);
	if (!_cap.isOpened()) {
		std::cerr << "Error opening video stream\n";
		return false;
	}

	_isRunning = true;
	_captureThread = std::thread(&FaceDetector::_captureFrame, this, std::ref(_cap), std::ref(_frame));
	return true;
}

void FaceDetector::_captureFrame(cv::VideoCapture& cap, cv::Mat& frame)
{
	while (_isRunning) {
		cv::Mat tempFrame;
		_cap >> tempFrame;
		if (!tempFrame.empty()) {
			cv::flip(tempFrame, tempFrame, 1);
			frame = tempFrame.clone();
		}
		else 
			std::cerr << "Captured empty frame\n"; // Log if a frame is empty
		std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Small delay to reduce CPU usage
	}
}

void FaceDetector::run()
{
	if (_frame.empty()) {
		cv::waitKey(10);
		return;
	}

	cv::Mat resizedFrame;
	cv::resize(_frame, resizedFrame, cv::Size(500, 500));

	cv::Mat gray;
	cv::cvtColor(resizedFrame, gray, cv::COLOR_BGR2GRAY);
	cv::equalizeHist(gray, gray);

	std::vector<cv::Rect> faces;
	_face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

	cv::Point frame_center(resizedFrame.cols / 2, resizedFrame.rows / 2);
	cv::circle(resizedFrame, frame_center, 5, cv::Scalar(0, 255, 0), -1);

	for (const auto& face : faces) {
		cv::rectangle(resizedFrame, face, cv::Scalar(255, 0, 0), 2);

		cv::Point face_center(face.x + face.width / 2, face.y + face.height / 2);
		cv::circle(resizedFrame, face_center, 5, cv::Scalar(0, 0, 255), -1);

		dx = face_center.x - frame_center.x;
		dy = frame_center.y - face_center.y;

		std::string text = "dx: " + std::to_string(dx) + ", dy: " + std::to_string(dy);
		cv::putText(resizedFrame, text, cv::Point(face.x, face.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
		//break; //skips rest of detected faces so only data from 1 face is sent
	}

	cv::imshow("Face Detection", resizedFrame);
	cv::waitKey(1);
}

void FaceDetector::stop()
{
	_isRunning = false;
	if (_captureThread.joinable()) 
		_captureThread.join();
	_cap.release();
	cv::destroyAllWindows();
}
