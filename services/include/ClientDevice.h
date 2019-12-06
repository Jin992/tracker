//
// Created by jin on 11/22/19.
//

#ifndef CPPDETECTOR_CLIENTDEVICE_H
#define CPPDETECTOR_CLIENTDEVICE_H
#include <iostream>
#include "stdio.h"
#include "wsseapi.h"
#include <openssl/rsa.h>

#include "onvif/soapDeviceBindingProxy.h"

class ClientDevice {

public:
	ClientDevice();
	ClientDevice(std::string url, std::string user, std::string password, bool showCapabilities);
	~ClientDevice();

public:
	bool _hasMedia;
	bool _hasPTZ;

protected:
	std::string			_strUrl;
	std::string			_user;
	std::string			_password;
	struct soap			*soap;
	DeviceBindingProxy	proxyDevice;
};


#endif //CPPDETECTOR_CLIENTDEVICE_H
