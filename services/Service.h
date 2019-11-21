//
// Created by jin on 11/21/19.
//

#ifndef CPPDETECTOR_SERVICE_H
#define CPPDETECTOR_SERVICE_H
#include "stdsoap2.h"
#include "gsoap/plugin/wsseapi.h"
#include "config.h"

class Service {
public:
    Service(struct soap* soap)
    : _soap(soap)
    {}
    struct soap* soap() { return _soap; }

protected:
    void __set_credentials(char*username, char*pass) {
        soap_wsse_delete_Security(_soap);
        if (soap_wsse_add_Timestamp(_soap, "Time", 10)
            || soap_wsse_add_UsernameTokenDigest(_soap, "Auth", username, pass)) {
            __report_error();
        }
    }

    void __report_error()
    {
        std::cerr << "Oops, something went wrong:" << std::endl;
        soap_stream_fault(_soap, std::cerr);
        exit(EXIT_FAILURE);
    }

private:


private:
    struct soap* _soap;
};


#endif //CPPDETECTOR_SERVICE_H
