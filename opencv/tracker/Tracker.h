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

class Tracker {
public:
    Tracker(std::string url, CamCtl &cam_ctl): _url(url), _cap(url), _cam_ctl(cam_ctl) {
        if (!_cap.isOpened())
            throw std::string("Can't open device " + _url);
    }
    ~Tracker() {
        _cap.release();
    }

    void start(){
        while (true) {
            cv::Mat frame;
            _cap >> frame;
            cv::Mat gray;
            frame.copyTo(gray);
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            //cv2.threshold(grayscale, 128, 255, cv2.THRESH_BINARY)
            cv::Mat blackAndWhite;
            gray.copyTo(blackAndWhite);
            cv::threshold(gray, blackAndWhite, 128, 255, cv::THRESH_BINARY);

            _cam_ctl.overlay().roi().set_frame(blackAndWhite);
			//_brightness_constast_control(gray);

        }
    }

private:
	/*void _brightness_constast_control(cv::Mat const & gray) {
		uint32_t x = 0, y = 0, width = 0, height = 0;
		x =  _cam_ctl.overlay().roi().x();
		y = _cam_ctl.overlay().roi().y();
		width = _cam_ctl.overlay().roi().width();
		height = _cam_ctl.overlay().roi().height();
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
		//if (min == 0)

    }*/

private:
    std::string _url;
    cv::VideoCapture _cap;
    CamCtl &_cam_ctl;
    //std::chrono::time_point<std::chrono::>



};


#endif //CPPDETECTOR_TRACKER_H
