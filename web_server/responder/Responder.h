//
// Created by jin on 11/29/19.
//

#ifndef CPPDETECTOR_RESPONDER_H
#define CPPDETECTOR_RESPONDER_H

#include <string>
#include <json11/json11.hpp>
#include <iostream>
#include <services/include/ClientPTZ.h>
#include "services/include/ClientImaging.h"

class ROI {
public:
    ROI()
    : _percent_x(100.0), _percent_y(100.0), _percent_height(100.0), _percent_width(100.0)
    {}
    ROI(float percent_x, float percent_y, float percent_height, float percent_width)
    :_percent_x(percent_x), _percent_y(percent_y), _percent_height(percent_height), _percent_width(percent_width)
    {}

    float x() { return _percent_x; }
    float y() { return _percent_y; }
    float height() { return _percent_height; }
    float width() { return _percent_width; }

private:
    float _percent_x;
    float _percent_y;
    float _percent_height;
    float _percent_width;
};

class Responder {
public:
	Responder(ClientPTZ* ptz, ClientImaging* img);
	json11::Json process_request(std::string const &request);

private:
	/// PTZ Request parser
	json11::Json	__ptz_status(std::string type);
	json11::Json	__ptz_move(json11::Json const &data);
	json11::Json	_ptz(json11::Json const &action);
	/// INIT Request parser
	json11::Json	__init_set(json11::Json const &data);
	json11::Json	__init_get(json11::Json const &data);
	json11::Json	_init(json11::Json const &action);
	/// OVERLAY Request parser
	json11::Json	_overlay(json11::Json const &action);

	/// IMAGING Request parser
	json11::Json	_imaging(json11::Json const &action);
	json11::Json	__set_imaging(json11::Json const &data);
	json11::Json	__get_imaging(json11::Json const &data);

	/// TRACKER Request parser
	json11::Json	_tracker(json11::Json const &action);
	json11::Json	__select_roi_tracker(json11::Json const &data);


private:
	ClientPTZ*			_ptz_ptr;
	ClientImaging*		_imaging_ptr;
	ImagingParams		_img_params;
	ROI					_region;
};


#endif //CPPDETECTOR_RESPONDER_H
