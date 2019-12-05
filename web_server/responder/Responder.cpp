//
// Created by jin on 11/29/19.
//

#include "Responder.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "helpers/helpers.h"

float constexpr pan_mod = 0.0055555556;
float constexpr tilt_mod = 0.011111111;

Responder::Responder(CamCtl &cam_ctl): _cam_ctl(cam_ctl) {
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
		return json11::Json();
	}
	/// Check if service is ptz
	json11::Json::object action = service["action"].object_items();
	std::string service_str = service["service"].string_value();
	if (service_str == "PTZ")			return _ptz(action);
	else if (service_str == "INIT")		return _init(action);
	else if (service_str == "OVERLAY")	return _overlay(action);
	else if (service_str == "IMAGING")	return _imaging(action);
	else if (service_str == "TRACKER")	return _tracker(action);
	else if (service_str == "IO")		return _io(action);
	return json11::Json();
}

json11::Json Responder::__ptz_status(std::string type) {
	/// status - store currenct camera position
	StatusPTZ status;
	/// request camera position with onvif
	_cam_ctl.ptz()->getStatus("PTZ", status);
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
	if (_cam_ctl.ptz() != nullptr) {
		float pan = data["pan"].number_value() * pan_mod;
		if (data["pan"].number_value() > 180) {
			pan =  ((data["pan"].number_value() - 360) * -1) * pan_mod * - 1;
		}
		std::cout << pan << std::endl;
		if (pan < 0) {
			if (pan < -1.0) pan = -1.0;
		} else {
			if (pan > 1.0) pan = 1.0;
		}
		float tilt = data["tilt"].number_value() * tilt_mod;
		if (tilt < 0) {
			if (tilt < -1.0) tilt = -1.0;
		} else {
			if (tilt > 1.0) tilt = 1.0;
		}
		float zoom = data["zoom"].number_value();
		/// TODO get values from cameraconfig
		float zoomRange = 100 - 31;
		float zoomRangeOneDegree = 1.0 / zoomRange;
		float zoom_mod = (((zoom - 31) - zoomRange) * -1) * zoomRangeOneDegree;
		if (zoom_mod > 1) zoom_mod = 1;
		if (zoom_mod < 0) zoom_mod = 0;
		_cam_ctl.ptz()->absoluteMove("PTZ",  pan, 1, tilt, 1, zoom_mod, 1);
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
		if (action["type"].string_value() == "move") return __ptz_move(data);
	} else {
		if (action["type"].string_value() == "status") return __ptz_status("status");
	}
	return json11::Json();
}

json11::Json Responder::_overlay(json11::Json const &action) {
	if (action["data"]["id"].is_null()) {
		_cam_ctl.overlay().roi().clear();
		std::string encoded = _cam_ctl.overlay().drawRoi();
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
		std::string encoded = _cam_ctl.overlay().drawRoi();
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
		_cam_ctl.imaging()->getOptions("VideoToken", _img_params);
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
	if (action["type"].string_value() == "get") return __init_get(data);
	else if (action["type"].string_value() == "set") return  __init_set(data);
	return json11::Json();
}

json11::Json Responder::__set_imaging(json11::Json const &data) {
	json11::Json response;
	if (data.dump() != "") {
		_cam_ctl.imaging()->setImagingSettings("", data["brightness"].number_value(), data["contrast"].number_value(), 0);
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
		_cam_ctl.imaging()->GetImagingSettings("VideoToken", params);
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
	if (action["type"].string_value() == "set") return __set_imaging(data);
	else if (action["type"].string_value() == "status") return __get_imaging(data);
	return json11::Json();
}

json11::Json Responder::_tracker(json11::Json const &action) {
	json11::Json::object data = action["data"].object_items();
	if (action["type"].string_value() == "select_roi") return __select_roi_tracker(data);
    return json11::Json();
}

json11::Json Responder::__select_roi_tracker(json11::Json const &data) {
	json11::Json response;
	if (data.dump() != "") {
		_cam_ctl.overlay().roi().set_roi(
				data["x"].number_value(),
				data["y"].number_value(),
				data["width"].number_value(),
				data["height"].number_value());

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

json11::Json	Responder::_io(json11::Json const &action) {
	json11::Json::object data = action["data"].object_items();
	if (action["type"].string_value() == "cmd") return __cmd_io(data);
	return json11::Json();
}

json11::Json	Responder::__cmd_io(json11::Json const &data) {
	json11::Json response;
	if (data.dump() != "") {
		/* Do something */
		response = json11::Json::object{{"Response",json11::Json::object{
				{"service", "IO"},
				{"action", json11::Json::object{
						{"type", "cmd"},
						{"data", json11::Json::object{
								{"cmd_name", data["cmd_value"].string_value()},
								{"input", data["input"].string_value()},
								{"status", data["status"].string_value()}
		}}}}}}};
		return response;
	}
	return json11::Json();
}

