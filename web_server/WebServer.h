//
// Created by jin on 11/28/19.
//

#ifndef CPPDETECTOR_WEBSERVER_H
#define CPPDETECTOR_WEBSERVER_H

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <thread>
#include <json11/json11.hpp>
#include "services/include/ClientPTZ.h"
#include <responder/Responder.h>
#include "services/include/ClientImaging.h"

typedef websocketpp::server<websocketpp::config::asio> server;


class Range {
public:
    Range(uint32_t min, uint32_t max):_min(min), _max(max)
    {}
    uint32_t min() const { return _min; }
    uint32_t max() const { return _max; }

private:
    uint32_t  _min;
    uint32_t  _max;
};

class CameraConfig {
public:
    CameraConfig(Range pan, Range tilt, Range zoom) : _pan(pan), _tilt(tilt), _zoom(zoom)
    {}
private:
    Range _pan;
    Range _tilt;
    Range _zoom;
};

class PTZ {
public:
    PTZ(float pan, float tilt, float zoom) {

    }

    float pan() const { return _pan; }
    float tilt() const { return _tilt; }
    float zoom() const { return _zoom; }

private:
    float _pan;
    float _tilt;
    float _zoom;
};

class WebServer {
public:
    WebServer(ClientPTZ *PTZ, ClientImaging * imaging) : _ptz(PTZ), _imaging(imaging), _respond(_ptz, _imaging) {
        // set logging settings
        _endpoint.set_error_channels(websocketpp::log::elevel::all);
        _endpoint.set_access_channels(websocketpp::log::alevel::all);
        // Turn on reuse addr
        _endpoint.set_reuse_addr(true);
        // Initialize Asio
        _endpoint.init_asio();

        // Set the default message handler to the echo handler
        _endpoint.set_message_handler([this](websocketpp::connection_hdl hdl, server::message_ptr msg) {_msg_handler(hdl, msg);});
    }

    void run() {
        // Start socket listener in parallel thread
        std::thread thread(&WebServer::_start_server, this);
        // Detach thread we don't care about it anymore
        if (thread.joinable())
            thread.detach();
    }

private:
    void _msg_handler(websocketpp::connection_hdl hdl, server::message_ptr msg) {
        json11::Json respond = _respond.process_request(msg->get_payload());
        //_json_parser(hdl, msg->get_payload());
        _endpoint.send(hdl, respond.dump(), websocketpp::frame::opcode::text);

    }
    void _start_server(){
        // Listen on port 11111 and use only IpV4 address family
        _endpoint.listen(boost::asio::ip::tcp::v4(),11111);

        // Queues a connection accept operation
        _endpoint.start_accept();
        // Start the Asio io_service run loop
        _endpoint.run();
    }

private:
    server _endpoint;
    ClientPTZ *_ptz;
    ClientImaging * _imaging;
    Responder _respond;
};


#endif //CPPDETECTOR_WEBSERVER_H
