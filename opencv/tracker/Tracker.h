//
// Created by jin on 12/3/19.
//

#ifndef CPPDETECTOR_TRACKER_H
#define CPPDETECTOR_TRACKER_H

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include "services/include/ClientImaging.h"
#include "cam_ctl/CamCtl.h"
#include <chrono>
#include <thread>

class Tracker {
public:
    Tracker(std::string url, CamCtl &cam_ctl)
    : _url(url), _cap(url), _cam_ctl(cam_ctl), _brightness_timer(std::chrono::steady_clock::now()), _contrast_timer(std::chrono::steady_clock::now())
    {
        if (!_cap.isOpened())
            throw std::string("Can't open device " + _url);
    }
    ~Tracker() {
        _cap.release();
    }

    void start(){
    	std::thread capture(&Tracker::decode_video_stream, this);
    	if (capture.joinable())
    		capture.detach();
        while (true) {
            cv::Mat gray;
            std::unique_lock<std::mutex> lock(_mutex);
            if (_frame_to_process.empty()) {
            	lock.unlock();
				continue;
            }
            cv::cvtColor(_frame_to_process, gray, cv::COLOR_BGR2GRAY);
            lock.unlock();
            //cv2.threshold(grayscale, 128, 255, cv2.THRESH_BINARY)
            cv::Mat blackAndWhite;
            gray.copyTo(blackAndWhite);
            cv::threshold(gray, blackAndWhite, 128, 255, cv::THRESH_BINARY);
            _cam_ctl.overlay().roi().set_frame(gray);
			_brightness_constast_control(gray);

        }
    }
//auto start = std::chrono::steady_clock::now();
//        std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
//        auto duration = std::chrono::duration_cast< TimeT>
//                            (std::chrono::steady_clock::now() - start);
//        return duration.count();
private:
	void _brightness_constast_control(cv::Mat const & gray) {

		uint32_t x = 0, y = 0, width = 0, height = 0;
		x =  _cam_ctl.overlay().roi().roi().x();
		y = _cam_ctl.overlay().roi().roi().y();
		width = _cam_ctl.overlay().roi().roi().mat().cols;
		height = _cam_ctl.overlay().roi().roi().mat().rows;
		cv::Rect roi_rect(y, x, width, height);
		cv::Mat roi = gray(roi_rect);

		uint min = 1000;
		uint max = 0;
		uchar* ptr;
		for (int row = 0; row < roi.rows; ++row) {
			ptr = roi.ptr<uchar>(row);
			for (int col = 0; col < roi.cols; ++col) {
				if(*ptr < min) min = *ptr;
				if(*ptr > max) max = *ptr;
				ptr++;
			}
		}
		ImagingParams params;
		_cam_ctl.imaging()->GetImagingSettings("", params);
		float brightness = params.brightness().current();
		float contrast = params.contrast().current();
		bool send = false;
		if ( std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - _brightness_timer).count() > 100000) {
			if (min == 0) brightness = brightness + 1;
			else brightness = brightness - 1;
			send = true;
			std::cout << "Brighness " << brightness << std::endl;
			_brightness_timer = std::chrono::steady_clock::now();
		}
		if ( std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - _contrast_timer).count() > 100000) {
			if (max == 255) contrast = contrast - 1;
			else contrast = contrast + 1;
			send = true;
			std::cout << "Contrast " << contrast << std::endl;
			_contrast_timer = std::chrono::steady_clock::now();
		}
		if (send) {
			_cam_ctl.imaging()->setImagingSettings("", brightness, contrast, true);
			std::cout << "Send" << std::endl;
		}
		}

    void decode_video_stream() {
    	while (true) {
			cv::Mat frame;
			_cap >> frame;
			if (frame.empty())
				continue;
			std::unique_lock<std::mutex> lock(_mutex);
			frame.copyTo(_frame_to_process);
			lock.unlock();
		}
    }

private:
    std::string											_url;
    cv::VideoCapture									_cap;
    CamCtl												&_cam_ctl;
	std::chrono::time_point<std::chrono::steady_clock>	_brightness_timer;
	std::chrono::time_point<std::chrono::steady_clock>	_contrast_timer;
	cv::Mat												_frame_to_process;
	std::mutex											_mutex;



};


#endif //CPPDETECTOR_TRACKER_H
