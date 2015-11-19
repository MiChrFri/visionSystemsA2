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

double angleBetween2Lines(std::vector<cv::Point> line1, std::vector<cv::Point> line2);
double getAngle(cv::Point base, cv::Point neighbour);
double* getAngles(cv::Point pA, cv::Point pB, cv::Point pC);
cv::Mat* mapInRect(cv::Mat srcImage, cv::Point2f* sourcePoints);
double randInDegree(double rand);
bool closeToCorner(cv::Point pA, cv::Point corner);
double getDistance(cv::Point pA, cv::Point pB);
int getSimpleDist(cv::Point a, cv::Point b);

#endif /* GeometricUtils_hpp */

