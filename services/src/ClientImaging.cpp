//
// Created by jin on 11/27/19.
//

#include "ClientImaging.h"


ClientImaging::ClientImaging(std::string url, std::string user, std::string password, bool showCapabilities) :ClientDevice(url, user, password, showCapabilities) {
    proxyImaging.soap_endpoint = _strUrl.c_str();
    soap_register_plugin(proxyImaging.soap, soap_wsse);
}

ClientImaging::~ClientImaging() {

}

int ClientImaging::getOptions(std::string const & videoToken, ImagingParams &img_params) {
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, _user.c_str(), _password.c_str())){
        return -1;
    }
    _timg__GetOptions* _timg__get_options = soap_new__timg__GetOptions(soap, -1);
    _timg__GetOptionsResponse* _timg__get_options_response = soap_new__timg__GetOptionsResponse(soap, -1);
    _timg__get_options->VideoSourceToken = "VideoSource_1";
    if (SOAP_OK == proxyImaging.GetOptions(_timg__get_options, *_timg__get_options_response)) {
        img_params.brightness().set_min(_timg__get_options_response->ImagingOptions->Brightness->Min);
        img_params.brightness().set_max(_timg__get_options_response->ImagingOptions->Brightness->Max);
        img_params.contrast().set_min(_timg__get_options_response->ImagingOptions->Contrast->Min);
        img_params.contrast().set_max(_timg__get_options_response->ImagingOptions->Contrast->Max);
    }
    soap_destroy(soap);
    soap_end(soap);
    return 0;
}

/*
 * Token:VideoSourceToken
   SourceToken:VideoSource_1
 */
int ClientImaging::GetImagingSettings(std::string const & videoToken, ImagingParams &img_params) {
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, _user.c_str(), _password.c_str())){
        return -1;
    }
    _timg__GetImagingSettings* _timg__settings = soap_new__timg__GetImagingSettings(soap, -1);
    _timg__GetImagingSettingsResponse* _timg__setting_response = soap_new__timg__GetImagingSettingsResponse(soap, -1);
    _timg__settings->VideoSourceToken = "VideoSource_1";
    if (SOAP_OK == proxyImaging.GetImagingSettings(_timg__settings, *_timg__setting_response)){
        img_params.brightness().set_cur(*_timg__setting_response->ImagingSettings->Brightness);
        img_params.contrast().set_cur(*_timg__setting_response->ImagingSettings->Contrast);
    }
    soap_destroy(soap);
    soap_end(soap);
    return 0;
}

int ClientImaging::setImagingSettings(std::string const & videoToken, float brightness, float contrast, float irCut){
    if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, _user.c_str(), _password.c_str())){
        return -1;
    }
    _timg__GetImagingSettings* _timg__settings = soap_new__timg__GetImagingSettings(soap, -1);
    _timg__GetImagingSettingsResponse* _timg__setting_response = soap_new__timg__GetImagingSettingsResponse(soap, -1);
    _timg__settings->VideoSourceToken = "VideoSource_1";
    if (SOAP_OK == proxyImaging.GetImagingSettings(_timg__settings, *_timg__setting_response)){
        if (SOAP_OK != soap_wsse_add_UsernameTokenDigest(proxyImaging.soap, NULL, _user.c_str(), _password.c_str())){
            return -1;
        }
        _timg__SetImagingSettings *_timg__settings = soap_new__timg__SetImagingSettings(soap, -1);
        _timg__SetImagingSettingsResponse *_time__settings_response = soap_new__timg__SetImagingSettingsResponse(soap,-1);
        _timg__settings->VideoSourceToken = "VideoSource_1";
        _timg__settings->ImagingSettings = _timg__setting_response->ImagingSettings;
        if (brightness >= 0) *_timg__settings->ImagingSettings->Brightness = brightness;
        if (contrast >= 0) *_timg__settings->ImagingSettings->Contrast= contrast;
        if (SOAP_OK == proxyImaging.SetImagingSettings(_timg__settings, *_time__settings_response)) {
        }
    }
    soap_destroy(soap);
    soap_end(soap);
    return 0;
}