//
// Created by jin on 12/4/19.
//

#ifndef CPPDETECTOR_ROI_H
#define CPPDETECTOR_ROI_H

#include <opencv4/opencv2/opencv.hpp>
#include <mutex>

class ROI_Cords {
public:
	ROI_Cords (float x, float y, float width, float height)
	: _x(x), _y(y), _height(height), _width(width)
	{}
	uint32_t x(uint32_t width)		{ return width * (_x / 100); }
	uint32_t y(uint32_t height)		{ return height * (_y / 100); }
	uint32_t width(uint32_t width)	{ return width * (_width / 100); }
	uint32_t height(uint32_t height)	{ return height * (_height / 100); }

private:
	float _x;
	float _y;
	float _height;
	float _width;
};

class ROI_Rect {
public:
	ROI_Rect(uint32_t frame_width, uint32_t frame_height, ROI_Cords cords)
	: _x(0), _y(0), _frame_width(frame_width), _frame_height(frame_height), _cords(cords)
	{}

	bool empty() { return _roi_rect.empty(); }

	void extract_roi(cv::Mat const &frame) {
		_x = _cords.x(_frame_width);
		_y = _cords.y(_frame_height);
		if (_cords.width(_frame_width) == 0 || _cords.height(_frame_height) == 0) return;
		cv::Rect rect(_cords.y(_frame_height), _cords.x(_frame_width), _cords.width(_frame_width), _cords.height(_frame_height));
		cv::Mat cropped = frame(rect);
		cv::cvtColor(cropped,_roi_rect,cv::COLOR_GRAY2BGRA);
	}

	uint32_t x() { return _x; }
	uint32_t y() { return _y; }

	cv::Mat & mat() { return _roi_rect; }
	cv::Point	top_left_point() {
		return {(int)_cords.y(_frame_height), (int)_cords.x(_frame_width)};
	}

	cv::Point	bottom_right_point() {
		return {(int)(_cords.y(_frame_height) + _cords.width(_frame_width)), (int)(_cords.x(_frame_width) + _cords.height(_frame_height))};
	}

private:
	uint32_t	_x;
	uint32_t	_y;
	cv::Mat		_roi_rect;
	uint32_t	_frame_width;
	uint32_t	_frame_height;
	ROI_Cords	_cords;
};

class ROI {
public:
	ROI( uint32_t overlay_width, uint32_t overlay_height)
	: _percent_roi(0.0, 0.0, 100.0,100.0),
	_overlay_width(overlay_width), _overlay_height(overlay_height)
	{}

	void set_frame(cv::Mat const &mat) {
		std::lock_guard<std::mutex> guard(_mutex);
		mat.copyTo(_frame);
	}
	void set_roi(float x, float y, float width, float height) {
		_percent_roi = ROI_Cords(x, y, width, height);
	}



	void clear() {
		_percent_roi = ROI_Cords(0.0, 0.0, 100.0, 100.0);
	}

	ROI_Rect roi() {
		return _extract_roi();
	}

private:
	ROI_Rect _extract_roi() {
		ROI_Rect roi(_overlay_width, _overlay_height, _percent_roi);
		std::unique_lock<std::mutex> lock(_mutex);
			if (_frame.empty()) return ROI_Rect(_overlay_width, _overlay_height, _percent_roi);
			roi.extract_roi(_frame);
		lock.unlock();
		return roi;
	}

private:
	ROI_Cords	_percent_roi;
	uint32_t	_overlay_width;
	uint32_t	_overlay_height;
	cv::Mat		_frame;
	std::mutex	_mutex;
};


#endif //CPPDETECTOR_ROI_H
