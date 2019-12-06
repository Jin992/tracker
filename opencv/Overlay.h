//
// Created by jin on 12/4/19.
//

#ifndef CPPDETECTOR_OVERLAY_H
#define CPPDETECTOR_OVERLAY_H


#include <cstdint>
#include <helpers/helpers.h>
#include "opencv/roi/Roi.h"
#include <thread>

#define IMAGE_BLOB_ID 0

class Overlay {
public:
	Overlay(uint32_t width, uint32_t height)
	:_overlay_width(width), _overlay_height(height),_roi(width, height), _frame_cnt(0)
	{
		std::thread worker(&Overlay::_overlay_maker, this);
		if (worker.joinable())
			worker.detach();
	}
	ROI & roi() { return _roi; }

	void drawRoi() {
		cv::Mat overlay(_overlay_height, _overlay_width, CV_8UC4, cv::Scalar(0, 0, 0, 0));
		/// Get overlay image
		ROI_Rect roi_rect = _roi.roi();

		if (!roi_rect.empty()) {
			roi_rect.mat().copyTo(overlay(cv::Rect(roi_rect.y(), roi_rect.x(), roi_rect.mat().cols, roi_rect.mat().rows)));
			/// Draw overlay frame
			cv::Scalar color(255, 0, 0, 255);
			cv::rectangle(overlay, roi_rect.top_left_point(), roi_rect.bottom_right_point(), color, 3);
			std::vector<uchar> data;
				cv::imencode(".png", overlay, data);
			{
				std::lock_guard<std::mutex> lock(_mutex);
				_overlay_binary = std::make_shared<std::vector<unsigned char>>(data.size() + 5);
				*_overlay_binary.get()->data() = IMAGE_BLOB_ID;
				memcpy(_overlay_binary.get()->data() + 1, &_frame_cnt, sizeof(_frame_cnt));
				//std::cout << "image blob id: " << IMAGE_BLOB_ID << " image sequence : " << _frame_cnt;
				//std::cout << "VECTOR image blob id: " << (unsigned)_overlay_binary.get()->data()[0] << " image sequence : "  << *(unsigned*)(_overlay_binary.get()->data()+1) << std::endl;
				memcpy(_overlay_binary.get()->data() + sizeof(_frame_cnt) + 1, data.data(), data.size());
			}


			_frame_cnt++;
		}
	}
	std::mutex &overlay_mutex() { return _mutex; }

	std::shared_ptr<std::vector<uchar>> overalay_buf() {
		std::lock_guard<std::mutex> lock(_mutex);
		return _overlay_binary;
	}

private:
	void _overlay_maker() {
		while (true) drawRoi();
	}

private:
	 uint32_t 								_overlay_width;
	 uint32_t 								_overlay_height;
	 ROI									_roi;
	 std::shared_ptr<std::vector<uchar>>	_overlay_binary;
	 std::string							_overlay;
	 std::mutex								_mutex;
	 uint32_t 								_frame_cnt;
};


#endif //CPPDETECTOR_OVERLAY_H
