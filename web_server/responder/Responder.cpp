//
// Created by jin on 11/29/19.
//

#include "Responder.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "helpers/helpers.h"

float constexpr pan_mod = 0.0055555556;
float constexpr tilt_mod = 0.011111111;

Responder::Responder(ClientPTZ* ptz, ClientImaging* imaging): _ptz_ptr(ptz), _imaging_ptr(imaging) {
}

json11::Json Responder::process_request(std::string const &request) {
    std::string err;
    /// Parse request string to json object
    const auto json = json11::Json::parse(request, err);
    /// Check for error and check if Request defined
    if (!err.empty() || json["Request"].is_null()) {
        std::cerr << err << std::endl;
        return json11::Json();
    }

    /// Extract Request object from json
    json11::Json::object service = json["Request"].object_items();
    /// check if service object defined
    if (service["service"].is_null()) {
        std::cerr << "Service is null" << std::endl;
        return false;
    }
    /// Check if service is ptz
    json11::Json::object action = service["action"].object_items();
    if (service["service"].string_value() == "PTZ") {
        return _ptz(action);
    } else if (service["service"].string_value() == "INIT") {
        return _init(action);
    } else if (service["service"].string_value() == "OVERLAY") {
        return _overlay(action);
    } else if (service["service"].string_value() == "IMAGING") {
        return _imaging(action);
    } else if (service["service"].string_value() == "TRACKER") {
        return _tracker(action);
    }
    return json11::Json();
}

json11::Json Responder::__ptz_status(std::string type) {
    /// status - store currenct camera position
    StatusPTZ status;
    /// request camera position with onvif
    _ptz_ptr->getStatus("PTZ", status);
    /// Generate response
    float zoomRange = 100 - 31;
    float zoomRangeOneDegree = 1.0 / zoomRange;
    float zoom_mod = 100 - status.zoom() * zoomRangeOneDegree ;
    //float zoom_mod = (((zoom - 31) - zoomRange) * -1) * zoomRangeOneDegree;
    json11::Json response = json11::Json::object{
            {"Response",json11::Json::object{
                    {"service", "PTZ"},
                    {"action",  json11::Json::object{
                            {"type", type},
                            {"data", json11::Json::object{
                                    {"pan", static_cast<int>(status.pan() / pan_mod)},
                                    {"tilt", static_cast<int>(status.tilt() / tilt_mod)},
                                    {"zoom", (int)zoom_mod}
    }}}}}}};
    return response;
}

json11::Json Responder::__ptz_move(json11::Json const &data) {
    if (_ptz_ptr != nullptr) {
        float pan = data["pan"].number_value() * pan_mod;
        if (data["pan"].number_value() > 180) {
            pan =  ((data["pan"].number_value() - 360) * -1) * pan_mod * - 1;
        }
        std::cout << pan << std::endl;
        if (pan < 0) {
            if (pan < -1.0)
                pan = -1.0;
        } else {
            if (pan > 1.0)
                pan = 1.0;
        }
        float tilt = data["tilt"].number_value() * tilt_mod;
        if (tilt < 0) {
            if (tilt < -1.0)
                tilt = -1.0;
        } else {
            if (tilt > 1.0)
                tilt = 1.0;
        }
        float zoom = data["zoom"].number_value();
        /// TODO get values from cameraconfig
        float zoomRange = 100 - 31;
        float zoomRangeOneDegree = 1.0 / zoomRange;
        float zoom_mod = (((zoom - 31) - zoomRange) * -1) * zoomRangeOneDegree;
        if (zoom_mod > 1)
            zoom_mod = 1;
        if (zoom_mod < 0)
            zoom_mod = 0;
        _ptz_ptr->absoluteMove("PTZ",  pan, 1, tilt, 1, zoom_mod, 1);
        return json11::Json::object{
                {"Response",json11::Json::object{
                        {"service", "PTZ"},
                        {"action",  json11::Json::object{
                                {"type", "move"},
                                {"data", json11::Json::object{
                                        {"pan", data["pan"].number_value()},
                                        {"tilt", data["tilt"].number_value() },
                                        {"zoom", data["zoom"].number_value()}
                }}}}}}};
    }
    return json11::Json();
}

json11::Json Responder::_ptz(json11::Json const &action) {
    if (!action["data"].is_null()) {
        json11::Json::object data = action["data"].object_items();
        if (action["type"].string_value() == "move") {
            return __ptz_move(data);
        }
    } else {
        if (action["type"].string_value() == "status") {
            return __ptz_status("status");
        }
    }
    return json11::Json();
}

json11::Json Responder::_overlay(json11::Json const &action) {
    static bool squeare = true;
    int defautl_y = 0;
    int defautl_x = 0;
    int defautl_w = 1920;
    int defautl_h = 1080;

    if (action["data"]["id"].is_null()) {
        cv::Mat png_image(defautl_h, defautl_w, CV_8UC4, cv::Scalar(0, 0, 0, 0));
        cv::Point p3(defautl_x, defautl_y), p4(defautl_w, defautl_h);
        int thicknessRectangle1 = 3;
        cv::Scalar colorRectangle1(0, 255, 00, 255);
        cv::rectangle(png_image, p3, p4, colorRectangle1, thicknessRectangle1);
        std::vector<uchar> buf;
        cv::imencode(".png", png_image, buf);
        uchar *enc_msg = new uchar[buf.size()];
        for (int i = 0; i < buf.size(); i++)
            enc_msg[i] = buf[i];
        std::string encoded = base64_encode(enc_msg, buf.size());
        json11::Json::object data = action["data"].object_items();

        if (action["type"].string_value() == "get") {
            json11::Json response = json11::Json::object{
                    {"Response", json11::Json::object{
                            {"service", "OVERLAY"},
                            {"action",  json11::Json::object{
                                    {"type", "get"},
                                    {"data", json11::Json::object{
                                            {"id",    1},
                                            {"image", "data:image/png;base64," + encoded}
            }}}}}}};
            return response;
        }
    }else {
        cv::Mat png_image(1080, 1920, CV_8UC4, cv::Scalar(0, 0, 0, 0));
        int x_mod =  defautl_w * (_region.x() / 100);
        int y_mod =  defautl_h * (_region.y() / 100);
        int x1_mod = x_mod + defautl_h * (_region.height() / 100);
        int y1_mod = y_mod + defautl_w * (_region.width() / 100);

        cv::Point p3(y_mod, x_mod), p4(y1_mod, x1_mod);

        cv::Scalar colorRectangle1(0, 0, 255, 255);
        cv::Scalar colorRectangle1Green(255, 0, 0, 255);
        int thicknessRectangle1 = 3;
        if (squeare) {
            cv::rectangle(png_image, p3, p4, colorRectangle1, thicknessRectangle1);
            squeare = false;
        } else {
            cv::rectangle(png_image, p3, p4, colorRectangle1Green, thicknessRectangle1);
            squeare = true;
        }

        std::vector<uchar> buf;
        cv::imencode(".png", png_image, buf);
        uchar *enc_msg = new uchar[buf.size()];
        for (int i = 0; i < buf.size(); i++)
            enc_msg[i] = buf[i];
        std::string encoded = base64_encode(enc_msg, buf.size());
        json11::Json::object data = action["data"].object_items();

        if (action["type"].string_value() == "get") {
            json11::Json response = json11::Json::object{
                {"Response", json11::Json::object{
                    {"service", "OVERLAY"},
                    {"action",  json11::Json::object{
                        {"type", "get"},
                        {"data", json11::Json::object{
                            {"id",    1},
                            {"image", "data:image/png;base64," + encoded}
            }}}}}}};
            return response;
        }
    }
    return json11::Json();
}

json11::Json Responder::__init_set(json11::Json const &data) {
    json11::Json response;
    if (!data.is_null()) {
        response = json11::Json::object{
                {"Response",json11::Json::object{
                        {"service", "INIT"},
                        {"action",  json11::Json::object{
                                {"type", "set"},
                                {"data", json11::Json::object{
                                        {"panRangeMin",  data["panRangeMin"].int_value()},
                                        {"panRangeMax",  data["panRangeMax"].int_value()},
                                        {"tiltRangeMin", data["tiltRangeMin"].int_value()},
                                        {"tiltRangeMax", data["tiltRangeMax"].int_value()},
                                        {"zoomRangeMin", data["zoomRangeMin"].int_value()},
                                        {"zoomRangeMax", data["oomRangeMax"].int_value()}
                                }}}}}}};
    }
    return response;
}

json11::Json Responder::__init_get(json11::Json const &data) {
    if (data.string_value() == "") {
        _imaging_ptr->getOptions("VideoToken", _img_params);
        return json11::Json::object{
                {"Response",json11::Json::object{
                        {"service", "INIT"},
                        {"action",  json11::Json::object{
                                {"type", "get"},
                                {"data", json11::Json::object{
                                        {"panRangeMin",  0},
                                        {"panRangeMax",  360},
                                        {"tiltRangeMin", 0},
                                        {"tiltRangeMax", 90},
                                        {"zoomRangeMin", 100},
                                        {"zoomRangeMax", 31},
                                        {"brightnessRangeMax", _img_params.brightness().max()},
                                        {"brightnessRangeMin", _img_params.brightness().min()},
                                        {"contrastRangeMax", _img_params.contrast().max()},
                                        {"contrastRangeMin", _img_params.contrast().min()}
                                }}}}}}};
    } else {
        return json11::Json::object{
                {"Response",json11::Json::object{
                        {"service", "INIT"},
                        {"action",  json11::Json::object{
                                {"type", "get"},
                                {"data", nullptr}
                        }}}}};

    }
}

json11::Json Responder::_init(json11::Json const &action) {
    json11::Json::object data = action["data"].object_items();
    if (action["type"].string_value() == "get") {
        return __init_get(data);
    } else if (action["type"].string_value() == "set") {
        return  __init_set(data);
    }
    return json11::Json();
}

json11::Json Responder::__set_imaging(json11::Json const &data) {
    json11::Json response;
    if (data.dump() != "") {
        _imaging_ptr->setImagingSettings("", data["brightness"].number_value(), data["contrast"].number_value(), 0);
        response = json11::Json::object{
                {"Response",json11::Json::object{
                        {"service", "IMAGING"},
                        {"action",  json11::Json::object{
                                {"type", "set"},
                                {"data", json11::Json::object{
                                        {"brightness",  data["brightness"].int_value()},
                                        {"contrast",  data["contrast"].int_value()},
                                        {"irCut", data["irCut"].int_value()}
                                }}}}}}};
    }
    return response;
}

json11::Json Responder::__get_imaging(json11::Json const &data) {
    json11::Json response;
    if (data.string_value() == "") {
        ImagingParams params;
        _imaging_ptr->GetImagingSettings("VideoToken", params);
        response = json11::Json::object{
                {"Response",json11::Json::object{
                        {"service", "IMAGING"},
                        {"action",  json11::Json::object{
                                {"type", "status"},
                                {"data", json11::Json::object{
                                        {"brightness",  params.brightness().current()},
                                        {"contrast",  params.contrast().current()},
                                        {"irCut", params.irCut()}
                                }}}}}}};
    }
    return response;
}

json11::Json Responder::_imaging(json11::Json const &action){
    json11::Json::object data = action["data"].object_items();
    if (action["type"].string_value() == "set") {
        return __set_imaging(data);
    } else if (action["type"].string_value() == "status") {
        return __get_imaging(data);
    }
    return json11::Json();
}

json11::Json Responder::_tracker(json11::Json const &action) {
    json11::Json::object data = action["data"].object_items();
    if (action["type"].string_value() == "select_roi") {
        return __select_roi_tracker(data);
    }
    return json11::Json();
}
//{"x":30(%), "y":40(%), "height": 20(%), "width":30(%)}
json11::Json Responder::__select_roi_tracker(json11::Json const &data) {
    json11::Json response;
    std::cout << "__select_roi_tracker data: " << data.dump() << std::endl;
    if (data.dump() != "") {
        _region = ROI(data["x"].number_value(), data["y"].number_value(), data["height"].number_value(), data["width"].number_value());
        response = json11::Json::object{{"Response",json11::Json::object{
                {"service", "TRACKER"},
                {"action", json11::Json::object{
                {"type", "select_roi"},
                {"data", json11::Json::object{
                        {"x", data["x"].number_value()},
                        {"y", data["y"].number_value()},
                        {"height", data["height"].number_value()},
                        {"width", data["width"].number_value()}}
                }}}}
        }};
    }
    return response;
}