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
		//_roi.clear();
		/// Get overlay imege
		cv::Mat roi_image = _roi.roi();
		cv::Mat roi_enchanced ;//= cv::Mat::zeros(roi_image.rows, roi_image.cols, CV_8UC4);


		cv::cvtColor(roi_image,roi_enchanced,cv::COLOR_GRAY2BGRA);
		if (!roi_image.empty())
			roi_enchanced.copyTo(overlay(cv::Rect(_roi.y(), _roi.x(), roi_enchanced.cols, roi_enchanced.rows)));
		/// Draw overlay frame
		cv::Point p1(_roi.y(), _roi.x()), p2(_roi.y() + _roi.width(), _roi.x() + _roi.height());
		cv::Scalar colorRectangle(255, 0, 0, 255);
		int thicknessRectangle = 3;
		cv::rectangle(overlay, p1, p2, colorRectangle, thicknessRectangle);

		std::vector<uchar> buf;
		cv::imencode(".png", overlay, buf);
		uchar *enc_msg = new uchar[buf.size()];
		for (int i = 0; i < buf.size(); i++)
			enc_msg[i] = buf[i];
		return base64_encode(enc_msg, buf.size());
	}

private:
	 uint32_t 		_overlay_width;
	 uint32_t 		_overlay_height;
	 ROI			_roi;
};


#endif //CPPDETECTOR_OVERLAY_H
