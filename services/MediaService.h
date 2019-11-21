//
// Created by jin on 11/21/19.
//

#ifndef CPPDETECTOR_MEDIASERVICE_H
#define CPPDETECTOR_MEDIASERVICE_H

#include <onvif/soapMediaBindingProxy.h>
#include "Service.h"

class MediaService: public Service {
public:
    MediaService(struct soap* soap)
    : Service(soap), _media_service(this->soap())
    {}

    void set_media_endpoint(_tds__GetCapabilitiesResponse const &capabilities) {
        _media_service.soap_endpoint = capabilities.Capabilities->Media->XAddr.c_str();
    }
    MediaBindingProxy & media() { return _media_service; }
    _trt__GetProfilesResponse & profiles() {
        _trt__GetProfiles _get_profiles;
        __set_credentials(USERNAME, PASSWORD);
        if (_media_service.GetProfiles(&_get_profiles, _media_profiles))
            __report_error();
        return _media_profiles;
    }



private:
    MediaBindingProxy           _media_service;
    std::string                 _endpoint;
    _trt__GetProfilesResponse   _media_profiles;
};


#endif //CPPDETECTOR_MEDIASERVICE_H
