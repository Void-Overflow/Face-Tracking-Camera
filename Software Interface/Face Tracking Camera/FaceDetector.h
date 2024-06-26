#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <thread>
#include <atomic>


class FaceDetector {
private:
	cv::CascadeClassifier _face_cascade;

	cv::VideoCapture _cap;
	cv::Mat _frame;

	std::atomic_bool _isRunning;
	std::thread _captureThread;
	void _captureFrame(cv::VideoCapture& cap, cv::Mat& frame);
public:
	int dx, dy;

	bool load(std::string cascadePath);
	void run();
	void stop();
};

#endif
