//
//  HistogramUtils.hpp
//  Assignment2VS
//
//  Created by Michael Frick on 08/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef HistogramUtils_hpp
#define HistogramUtils_hpp

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

void showHistogram(cv::Mat& img);
cv::MatND getHistogram(cv::Mat image);
cv::MatND getSampleHist();

#endif


