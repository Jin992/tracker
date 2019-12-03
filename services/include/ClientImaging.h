//
// Created by jin on 11/27/19.
//

#ifndef CPPDETECTOR_CLIENTIMAGING_H
#define CPPDETECTOR_CLIENTIMAGING_H

#include <iostream>
#include "stdio.h"
#include "wsseapi.h"
#include <openssl/rsa.h>
#include "ClientDevice.h"
#include "onvif/soapImagingBindingProxy.h"


#include "ClientDevice.h"

class ImagingParam {
public:
    ImagingParam(): _min(0), _max(0), _cur(0) {}
    ImagingParam(float min, float max, float cur):_min(min), _max(max), _cur(cur) {}

    void set_min(float min) { _min = min;}
    void set_max(float max) { _max = max;}
    void set_cur(float cur) { _cur = cur;}
    float min() const { return _min; }
    float max() const { return _max; }
    float current() const { return _cur; }

private:
    float _min;
    float _max;
    float _cur;
};

class ImagingParams {
public:
    ImagingParams(){}

    ImagingParam & brightness() {return _brightness; }
    ImagingParam & contrast() { return _contrast; }
    bool irCut() const { return _irCut; }
    void setIrCut(bool val) { _irCut = val; }

private:
    ImagingParam _brightness;
    ImagingParam _contrast;
    bool         _irCut;

};

class ClientImaging: public ClientDevice {
public:
    ClientImaging();
    ClientImaging(std::string url, std::string user, std::string password, bool showCapabilities);
    ~ClientImaging();

public:
    void getCurrentPreset();
    int GetImagingSettings(std::string const & videoToken, ImagingParams &img_params);
    void GetMoveOptions();
    int getOptions(std::string const & videoToken, ImagingParams &img_params);
    void getPresets();
    void getServiceCapabilities();
    void getStatus();
    void move();
    void setCurrentPreset();
    int setImagingSettings(std::string const & videoToken, float brightness, float contrast, float irCut);
    void stop();

protected:
    ImagingBindingProxy proxyImaging;
};


#endif //CPPDETECTOR_CLIENTIMAGING_H
