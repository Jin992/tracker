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

class Responder {
public:
    Responder(ClientPTZ* ptz, ClientImaging* img);
    json11::Json process_request(std::string const &request);

private:
    /// PTZ Request parser
    json11::Json __ptz_status(std::string type);
    json11::Json __ptz_move(json11::Json const &data);
    json11::Json _ptz(json11::Json const &request);
    /// INIT Request parser
    json11::Json __init_set(json11::Json const &data);
    json11::Json __init_get(json11::Json const &data);
    json11::Json _init(json11::Json const &request);
    /// OVERLAY Request parser
    json11::Json _overlay(json11::Json const &request);

    /// IMAGING Request parser
    json11::Json _imaging(json11::Json const &request);
    json11::Json __set_imaging(json11::Json const &data);
    json11::Json __get_imaging(json11::Json const &data);

    /// TRACKER Request parser
    json11::Json _tracker(json11::Json const &request);
    json11::Json __select_roi_tracker(json11::Json const &request);


private:
    ClientPTZ* _ptz_ptr;
    ClientImaging* _imaging_ptr;
    ImagingParams _img_params;
};


#endif //CPPDETECTOR_RESPONDER_H
