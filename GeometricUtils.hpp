//
//  GeometricUtils.hpp
//  Assignment2VS
//
//  Created by Michael Frick on 11/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef GeometricUtils_hpp
#define GeometricUtils_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

double getDistance(cv::Point pA, cv::Point pB);
double getAngle(cv::Point base, cv::Point neighbour);
double* getAngles(cv::Point pA, cv::Point pB, cv::Point pC);

#endif /* GeometricUtils_hpp */

