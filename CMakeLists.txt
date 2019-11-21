cmake_minimum_required(VERSION 3.14)
project(CPPDetector)

set(CMAKE_CXX_STANDARD 11)

set( CMAKE_CXX_FLAGS "-Wall -DWITH_OPENSSL -DWITH_DOM -DWITH_ZLIB" )

include_directories(.)
include_directories(gsoap)
include_directories(gsoap/custom)
include_directories(gsoap/plugin)
include_directories(${PROJECT_SOURCE_DIR})

add_executable(CPPDetector
        gsoap/plugin/wsddapi.cpp
        gsoap/plugin/wsseapi.cpp
        gsoap/plugin/wsaapi.cpp
        gsoap/plugin/mecevp.cpp
        gsoap/plugin/smdevp.cpp
        gsoap/dom.cpp
        gsoap/stdsoap2.cpp
        main.cpp
        onvif/soapAdvancedSecurityServiceBindingProxy.cpp
        onvif/soapC.cpp
        onvif/soapDeviceBindingProxy.cpp
        onvif/soapDeviceIOBindingProxy.cpp
        onvif/soapImagingBindingProxy.cpp
        onvif/soapMediaBindingProxy.cpp
        onvif/soapPTZBindingProxy.cpp
        onvif/soapPullPointSubscriptionBindingProxy.cpp
        onvif/soapRemoteDiscoveryBindingProxy.cpp
        onvif/soapwsddProxy.cpp
        onvif/wsddClient.cpp
        onvif/wsddServer.cpp IpCamera.cpp IpCamera.h services/DeviceService.cpp services/DeviceService.h config.h helpers.h helpers.cpp services/Service.cpp services/Service.h services/MediaService.cpp services/MediaService.h)

target_link_libraries(CPPDetector -lcrypto -lssl -lz)
