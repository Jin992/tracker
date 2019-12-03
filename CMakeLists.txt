cmake_minimum_required(VERSION 3.14)
project(CPPDetector)

set(CMAKE_CXX_STANDARD 11)

set( CMAKE_CXX_FLAGS "-Wall -DWITH_OPENSSL -DWITH_DOM -DWITH_ZLIB" )
find_package(Boost 1.58 COMPONENTS system regex  filesystem REQUIRED)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories(.)
include_directories(onvif/gsoap)
include_directories(onvif/gsoap/custom)
include_directories(onvif/gsoap/plugin)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(services/include)
include_directories(web_server)

add_executable(CPPDetector
        onvif/gsoap/plugin/wsddapi.cpp
        onvif/gsoap/plugin/wsseapi.cpp
        onvif/gsoap/plugin/wsaapi.cpp
        onvif/gsoap/plugin/mecevp.cpp
        onvif/gsoap/plugin/smdevp.cpp
        onvif/gsoap/dom.cpp
        onvif/gsoap/stdsoap2.cpp
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
        onvif/wsddServer.cpp
        services/src/ClientDevice.cpp
        services/src/ClientMedia.cpp
        services/src/ClientPTZ.cpp
        services/src/ClientImaging.cpp
        web_server/WebServer.cpp
        web_server/responder/Responder.cpp
        helpers/base64_encode.cpp
        opencv/tracker/Tracker.cpp)

target_link_libraries(CPPDetector -lcrypto -lssl -lz ${Boost_LIBRARIES}
        -pthread ${CMAKE_SOURCE_DIR}/web_server/json11/libjson11.a ${OpenCV_LIBS} )
