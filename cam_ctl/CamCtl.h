//
// Created by jin on 12/4/19.
//

#ifndef CPPDETECTOR_CAMCTL_H
#define CPPDETECTOR_CAMCTL_H

#include "services/include/ClientDevice.h"
#include "services/include/ClientMedia.h"
#include "services/include/ClientPTZ.h"
#include "services/include/ClientImaging.h"
#include <opencv4/opencv2/opencv.hpp>
#include "opencv/Overlay.h"


class CamCtl {
public:
	CamCtl(std::string const &ip, std::string const &user, std::string const &pass)
	: _url(ip), _user(user), _pass(pass), _overlay(1920, 1080)
	{
		_media = new ClientMedia(_url, _user, _pass, true);
		_ptz = new ClientPTZ(_url, _user, _pass, true);
		_imaging = new ClientImaging(_url, _user, _pass, true);
		_media->createProfile("PTZ", "PTZ");
		_media->addPTZConfiguration("PTZ", "PTZToken");
		_media->getVideoSourceConfigurations();
		_ptz->getConfiguration("PTZToken");
	}

	ClientImaging *imaging() { return _imaging; }
	ClientPTZ *ptz() { return _ptz; }
	Overlay &overlay(){ return _overlay; }

private:
	std::string		_url;
	std::string		_user;
	std::string		_pass;
	ClientMedia		*_media;
	ClientPTZ		*_ptz;
	ClientImaging	*_imaging;
	Overlay			_overlay;
};


#endif //CPPDETECTOR_CAMCTL_H
