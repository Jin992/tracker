//
// Created by jin on 12/3/19.
//

#ifndef CPPDETECTOR_TRACKER_H
#define CPPDETECTOR_TRACKER_H

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

class Tracker {
public:
    Tracker(std::string url): _url(url), _cap(url) {
        if (!_cap.isOpened())
            throw std::string("Can't open device " + _url);
    }
    ~Tracker() {
        _cap.release();
    }

    void start(){
        while (true) {
            cv::Mat frame;
            _cap >> frame;
            cv::Mat gray;
            frame.copyTo(gray);
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            //cv2.threshold(grayscale, 128, 255, cv2.THRESH_BINARY)
            cv::Mat blackAndWhite;
            gray.copyTo(blackAndWhite);
            cv::threshold(gray, blackAndWhite, 128, 255, cv::THRESH_BINARY);
            cv::imshow("Video", frame);
            cv::imshow("VideoGray", gray);
            cv::imshow("Binnary", blackAndWhite);
            if (frame.empty())
                break;
            int key =cv::waitKey(1);
            if(key == 27){
                cv::destroyAllWindows();
                break;
            }
        }
    }

private:
    std::string _url;
    cv::VideoCapture _cap;



};


#endif //CPPDETECTOR_TRACKER_H
