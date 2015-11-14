//
//  GeometricUtils.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 11/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include "GeometricUtils.hpp"

#include <cmath>
//#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// functions
double getDistance(Point pA, Point pB) {
    double c = pow((pA.x - pB.x), 2) + pow((pA.y - pB.y), 2);
    return sqrt(c);
}

double getAngle(Point base, Point neighbour) {
    double a = abs(neighbour.x - base.x);
    double b = abs(neighbour.y - base.y);
    
    double c = sqrt((pow(a, 2) + pow(b, 2)));
    double q = (pow(a, 2))/c;
    double p = c - q;
    double h = sqrt(p * q);
    double alpha = atan(h/p) * 180.0 / M_PI;
    double beta = 90 - alpha;
    
    return beta;
}

double* getAngles(Point pA, Point pB, Point pC) {
    double a = getDistance(pA, pB);
    double b = getDistance(pA, pC);
    double c = getDistance(pB, pC);

    // first angle
    double numerator = pow(a, 2) + pow(b, 2) - pow(c, 2);
    double denumerator = 2 * a * b;
    double angle1Rand = acos(numerator/denumerator);
    double angle1 = randInDegree(angle1Rand);
    
    // second angle
    numerator = pow(a, 2) + pow(c, 2) - pow(b, 2);
    denumerator = 2 * a * c;
    double angle2Rand = acos(numerator/denumerator);
    double angle2 = randInDegree(angle2Rand);
    
    // third angle
    double angle3 = 180.0 - angle1 - angle2;

    
    double* angles = new double[3];
    angles[0] = angle1;
    angles[1] = angle2;
    angles[2] = angle3;
    
    return angles;
}

double randInDegree(double rand) {
    double degree =  rand * 180.0/M_PI;
    
    return degree;
}

Mat* mapInRect(Mat srcImage, Point2f *sourcePoints ) {
    Point2f dstQua[4];

    Mat warp_mat( 2, 3, CV_32FC1 );
    Mat warp_dst;
    
    /// Set the dst image the same type and size as src
    int newImgW = 300;
    int newImgH = 448;
    
    warp_dst = Mat::zeros( newImgH, newImgW, srcImage.type() );
    
    dstQua[0] = Point2f( newImgW, newImgH);
    dstQua[1] = Point2f( 0, newImgH);
    dstQua[2] = Point2f( newImgW, 0);
    dstQua[3] = Point2f( 0, 0 );
    
    // Get the Perspective Transform
    warp_mat = getPerspectiveTransform( sourcePoints, dstQua );
    warpPerspective(srcImage, warp_dst, warp_mat, warp_dst.size());

    Mat* rtrnImg = new Mat;
    rtrnImg[0] = warp_dst;
    
    return rtrnImg;
}

bool closeToCorner(Point pA, Point corner) {
    int tolerance = 20;
    
    if(getSimpleDist(pA, corner) < tolerance) {
        return true;
    }
    else {
        return false;
    }
}

int getSimpleDist(Point a, Point b) {
    int xDist = abs(a.x - b.x);
    int yDist = abs(a.y - b.y);
    
    return xDist + yDist;
}
