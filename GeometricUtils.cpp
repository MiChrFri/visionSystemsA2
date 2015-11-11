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

//using namespace std;
using namespace cv;

// function declarations
double getDistance(Point pA, Point pB);
double getAngle(Point base, Point neighbour);
double* getAngles(Point pA, Point pB, Point pC);
double randInDegree(double rand);

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