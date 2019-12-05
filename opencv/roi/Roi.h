//
// Created by jin on 12/4/19.
//

#ifndef CPPDETECTOR_ROI_H
#define CPPDETECTOR_ROI_H

#include <opencv4/opencv2/opencv.hpp>
#include <mutex>

class Cords {
public:
private:
};

class ROI {
public:
	ROI( uint32_t overlay_width, uint32_t overlay_height)
	: _percent_x(0.0), _percent_y(0.0), _percent_height(100.0), _percent_width(100.0),
	_overlay_width(overlay_width), _overlay_height(overlay_height)
	{}

	void set_frame(cv::Mat const &mat) {
		std::lock_guard<std::mutex> guard(_mutex);
		mat.copyTo(_frame);
	}
	void set_roi(float x, float y, float width, float height) {
		_percent_x = x;
		_percent_y = y;
		_percent_width = width;
		_percent_height = height;
	}
	void clear() {
		_percent_x = 0;
		_percent_y = 0;
		_percent_width = 100;
		_percent_height = 100;
	}

	uint32_t x() { return _roi_x(); }
	uint32_t y() { return _roi_y(); }
	uint32_t height() { return _roi_height(); }
	uint32_t width() { return _roi_width(); }

	cv::Mat roi() {
		return _extract_roi();
	}

private:
	uint32_t _roi_x() { return _overlay_width * (_percent_x / 100);}
	uint32_t _roi_y() { return _overlay_height * (_percent_y / 100);}
	uint32_t _roi_height() { return _overlay_height * (_percent_height / 100);}
	uint32_t _roi_width() { return _overlay_width * (_percent_width / 100);}

	cv::Mat _extract_roi() {
		uint32_t x = 0, y = 0, width = 0, height = 0;
		x =  _roi_x();
		y = _roi_y();
		width = _roi_width();
		height = _roi_height();
		cv::Rect roi_rect(y, x, width, height);
		std::unique_lock<std::mutex> lock(_mutex);
		if (_frame.empty())
			return cv::Mat();
		cv::Mat roi = _frame(roi_rect);
		lock.unlock();
		return roi;
	}

private:
	float		_percent_x;
	float		_percent_y;
	float		_percent_height;
	float		_percent_width;
	uint32_t	_overlay_width;
	uint32_t	_overlay_height;
	cv::Mat		_frame;
	std::mutex	_mutex;
};


#endif //CPPDETECTOR_ROI_H
