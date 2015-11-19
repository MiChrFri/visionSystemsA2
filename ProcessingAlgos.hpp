//
//  ProcessingAlgos.hpp
//  Assignment2VS
//
//  Created by Michael Frick on 18/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef ProcessingAlgos_hpp
#define ProcessingAlgos_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

cv::Mat backProjection(cv::Mat* sampleHist, cv::Mat* image);
cv::Mat thresholdIMG(cv::Mat image);
std::vector<cv::Point2f> getPoints(cv::Mat* img);

#endif /* ProcessingAlgos_hpp */
