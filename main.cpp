#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <zbar.h>
#include <iostream>
#include <chrono>
#include <bits/stdc++.h>

#define MEASURE_TIME
#define SHOW_VIDEO

using namespace std;

int detect_codes(zbar::ImageScanner &scanner, cv::Mat &frame, unordered_set <string> &strings, double timestamp) {

  cv::Mat grey;
  cv::cvtColor(frame, grey, cv::COLOR_BGR2GRAY);

  zbar::Image image(grey.cols,              // width
                    grey.rows,              // height
                    "Y800",                 // format
                    (uchar *)grey.data,     // data
                    grey.cols * grey.rows); // length

  int n = scanner.scan(image);

  for (auto code = image.symbol_begin(); code != image.symbol_end(); ++code) {
    if (strings.find(code->get_data()) == strings.end()) {
      cout << "QR code detected at " << timestamp << "ms    >> \"" << code->get_data() << "\"" << endl;
      strings.insert(code->get_data());
    }
  }

  return n;
}

int main() {
  // How often the video should be sampled for a QR code, in seconds, increase
  // this number for higher performance, but lower accuracy
  int sample_period = 0.5;

  cout << "Starting capture" << endl;

  cv::VideoCapture capture("test-1.mp4");
  if (!capture.isOpened()) return -1;

  zbar::ImageScanner scanner;
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);  // disable all symbologies
  scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);  // enable QR Code only

  unordered_set <string> strings;

  double fps = capture.get(cv::CAP_PROP_FPS);

  cout << fps << " FPS" << endl;

  int tick = 0;

  while(capture.isOpened()) {
      tick++;
      cv::Mat frame;
      capture >> frame;

      if (tick > (sample_period * fps)) {
        tick = 0;

        if (!frame.empty()) {
            double timestamp = capture.get(cv::CAP_PROP_POS_MSEC);

            detect_codes(scanner, frame, strings, timestamp);
        } else {
            break;
        }
      }
  }

  capture.release();

  return 0;
}
