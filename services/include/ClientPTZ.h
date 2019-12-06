//
// Created by jin on 11/22/19.
//

#ifndef CPPDETECTOR_CLIENTPTZ_H
#define CPPDETECTOR_CLIENTPTZ_H


#include "ClientDevice.h"
#include "onvif/soapPTZBindingProxy.h"

class StatusPTZ{
public:
	StatusPTZ():_pan(0), _tilt(0), _zoom(0){}
	StatusPTZ(float pan, float tilt, float zoom): _pan(pan), _tilt(tilt), _zoom(zoom){}

	float pan() const { return _pan;}
	float tilt() const { return _tilt;}
	float zoom() const { return _zoom;}

private:
	float _pan;
	float _tilt;
	float _zoom;
};

class ClientPTZ : public ClientDevice{
public:
	ClientPTZ(std::string url, std::string user, std::string password, bool showCapabilities);
	~ClientPTZ();

public:
	//get all PTZ configurations
	int getPTZConfigurations();
	//get and print PTZ Status
	int getStatus(std::string profileToken, StatusPTZ &status);
	//absolute move
	int absoluteMove(std::string profileToken, float pan, float panSpeed, float tilt, float tiltSpeed, float zoom, float zoomSpeed);
	//relative move
	int relativeMove(std::string profileToken, float pan, float panSpeed, float tilt, float tiltSpeed, float zoom, float zoomSpeed);
	//continuous move
	int continuousMove(std::string profileToken, float panSpeed, float tiltSpeed, float zoomSpeed);
	//stop movement
	int stop(std::string profileToken, bool panTilt, bool zoom);
	//setHomePosition
	int setHomePosition(std::string profileToken);
	//goToHomePosition
	int goToHomePosition(std::string profileToken);
	//Get configuration and print it
	int getConfiguration(std::string configurationToken);


	//pan to the left n Degress
	int panLeft(std::string profileToken, int nDegrees);
	//pan to the right n Degrees
	int panRight(std::string profileToken, int nDegrees);

	//tilt down
	int tiltDown(std::string profileToken, int nDegrees);
	//tilt up
	int tiltUp(std::string profileToken, int nDegrees);

    //zoom in
    int zoomIn(std::string profileToken);
    //zoom out
    int zoomOut(std::string profileToken);

private:
    PTZBindingProxy proxyPTZ;
};

#endif //CPPDETECTOR_CLIENTPTZ_H
