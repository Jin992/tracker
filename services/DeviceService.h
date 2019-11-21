//
// Created by jin on 11/21/19.
//

#ifndef CPPDETECTOR_DEVICESERVICE_H
#define CPPDETECTOR_DEVICESERVICE_H

#include "stdsoap2.h"
#include <onvif/soapDeviceBindingProxy.h>
#include <string>
#include "Service.h"
#include "config.h"


class DeviceService: public Service {
public:
    DeviceService(struct soap *soap)
    : Service(soap), _service_proxy(this->soap()), _host(nullptr), _user(nullptr), _pass(nullptr)
    {}

    _tds__GetDeviceInformationResponse  & device_info() {
        _tds__GetDeviceInformation  get_dev_info;
        _service_proxy.soap_endpoint = HOSTNAME;
        __set_credentials(_user, _pass);
        if (_service_proxy.GetDeviceInformation(&get_dev_info, _device_info_response))
            __report_error();
        return _device_info_response;
    }

    _tds__GetCapabilitiesResponse & capabilities() {
        _tds__GetCapabilities get_capabilities;
        if (_service_proxy.GetCapabilities(&get_capabilities, _device_capabilities_response))
            __report_error();
        if (!_device_capabilities_response.Capabilities || !_device_capabilities_response.Capabilities->Media) {
            std::cerr << "Missing device capabilities info" << std::endl;
            exit(EXIT_FAILURE);
        }
        return _device_capabilities_response;
    }

    void set_credentials(char*hostname, char*username, char*password){
            _host = hostname;
            _user = username;
            _pass = password;
    }

private:
    DeviceBindingProxy                  _service_proxy;
    _tds__GetDeviceInformationResponse  _device_info_response;
    _tds__GetCapabilitiesResponse       _device_capabilities_response;
    char* _host;
    char* _user;
    char* _pass;
};


#endif //CPPDETECTOR_DEVICESERVICE_H
