//
// Created by jin on 12/4/19.
//

#ifndef CPPDETECTOR_OVERLAY_H
#define CPPDETECTOR_OVERLAY_H


#include <cstdint>
#include <helpers/helpers.h>
#include "opencv/roi/Roi.h"

class Overlay {
public:
	Overlay(uint32_t width, uint32_t height)
	:_overlay_width(width), _overlay_height(height),_roi(width, height)
	{}
	ROI & roi() { return _roi; }

	std::string drawRoi() {
		cv::Mat overlay(_overlay_height, _overlay_width, CV_8UC4, cv::Scalar(0, 0, 0, 0));
		/// Get overlay image
		ROI_Rect roi_rect = _roi.roi();

		if (!roi_rect.empty())
			roi_rect.mat().copyTo(overlay(cv::Rect(roi_rect.y(), roi_rect.x(), roi_rect.mat().cols, roi_rect.mat().rows)));
		/// Draw overlay frame
		cv::Scalar color(255, 0, 0, 255);
		cv::rectangle(overlay, roi_rect.top_left_point(), roi_rect.bottom_right_point(), color, 3);

		std::vector<uchar> buf;
		cv::imencode(".png", overlay, buf);
		uchar *enc_msg = new uchar[buf.size()];
		for (int i = 0; i < buf.size(); i++)
			enc_msg[i] = buf[i];
		std::string base64_string = base64_encode(enc_msg, buf.size());;
		delete [] enc_msg;
		return base64_string;
	}

private:
	 uint32_t 		_overlay_width;
	 uint32_t 		_overlay_height;
	 ROI			_roi;
};


#endif //CPPDETECTOR_OVERLAY_H
