//
//  GeometricUtils.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 11/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include <cmath>
#include <opencv2/opencv.hpp>

#include "GeometricUtils.hpp"
#include "Constants.h"

using namespace std;
using namespace cv;

/* return the angle between two lines to eachother */
double angleBetween2Lines(vector<Point> line1, vector<Point> line2) {
    double angle1 = atan2(line1[0].y - line1[1].y, line1[0].x - line1[1].x);
    double angle2 = atan2(line2[0].y - line2[1].y, line2[0].x - line2[1].x);
    return randInDegree(angle1 - angle2);
}

/* return the beta angle between two points using pythagoras */
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

/* returns an array of 3 angles in a triangle of three points */
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

/* return the degree of a given rand */
double randInDegree(double rand) {
    return rand * 180.0/M_PI;;
}

/* uses 4 point in an image to map them into a defined rectange */
Mat* mapInRect(Mat srcImage, Point2f *sourcePoints ) {
    Point2f dstQua[4];

    Mat warp_mat( 2, 3, CV_32FC1 );
    Mat warp_dst;
    
    // Set the dst image the same type and size as src
    int newImgW = constant::size.width;
    int newImgH = constant::size.height;
    
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

/* return true if the distance between two point is smaller than a defined tolerance */
bool closeToCorner(Point pA, Point corner) {
    int tolerance = 20;
    return getSimpleDist(pA, corner) < tolerance? true: false;
}

/* return the distance between two points using pythagoras */
double getDistance(Point pA, Point pB) {
    double c = pow((pA.x - pB.x), 2) + pow((pA.y - pB.y), 2);
    return sqrt(c);
}

/* a fast and easy way to calculate a distance between two points */
int getSimpleDist(Point a, Point b) {
    int xDist = abs(a.x - b.x);
    int yDist = abs(a.y - b.y);
    
    return xDist + yDist;
}
