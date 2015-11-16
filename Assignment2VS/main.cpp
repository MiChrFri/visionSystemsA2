//
//  main.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 07/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>

#include <cmath>

// custom includes
#include "ImageUtils.hpp"
#include "HistogramUtils.hpp"
#include "GeometricUtils.hpp"
#include "Debug.hpp"

// Struct for sorting
struct {
    bool operator() (Point pt1, Point pt2) {
        return (pt1.y > pt2.y);
    }
} pointSorter;

// function declarations
void displayImage(Mat image);
int* getNeighbours(int* baseIndex, vector<Point2f> *points);
bool inVector(int val, vector<int>vec);
int inSameSide(int val1, int val2,  vector<vector<int>> sides);
vector<vector<Point>> getBlindLines(vector<vector<int>> corners, vector<vector<int>> sides, vector<Point2f>center);
vector<Point2f> findIntersectionPoints(vector<vector<Point>> lines);

vector<vector<int>> getSides(vector<vector<int>> lines);
vector<int> getEdges(int corner, vector<vector<int>> sides);

// ALGOS
Mat thresholdIMG(Mat* image);
Mat adaptiveThresholdImg(Mat image);
Mat backProjection(Mat* sampleHist, Mat* inputImg);

int matchPossibility(Mat pageImg, Mat matchImg);

int identifyPoint(double* angles);
double getAngle(Point base, Point neighbour);

// GLOABAL VARS
enum pointProp {
    UNKNOWN, CORNER, SIDE, WRONGNEIGHBOUR
};

// main
int main(int argc, const char * argv[]) {
    logVersionNumber();
    
    Mat* pages  = getPages();
    Mat* photos = getPhotos();
    
    if(pages->empty() || photos->empty()) {
        cout << "loading images failed!" << endl;
    }
    else {
        // get sample histogram
        Mat sampleHist = getSampleHist();
        
        for(int i = 0; i < 3; i++) {
            // use variable name for readability
            Mat *rgbImg = new Mat;
            rgbImg = &photos[i];
            
            // back projection
            Mat BP = backProjection(&sampleHist, rgbImg);
            
            // back threshold
            Mat treshImg = thresholdIMG(&BP);
            
            //release the backprojected image
            BP.release();
            
            //find contours
            vector<vector<Point> > contours;
            findContours(treshImg, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
            
            //release the treshold image
            treshImg.release();
            
            // find contours
            vector<vector<Point> > contours_poly( contours.size() );
            vector<Rect> boundRect( contours.size() );
            vector<Point2f>center( contours.size() );
            vector<float>radius( contours.size() );
            
            for( int ii = 0; ii < contours.size(); ii++ ){
                approxPolyDP( Mat(contours[ii]), contours_poly[ii], 3, true );
                minEnclosingCircle( (Mat)contours_poly[ii], center[ii], radius[ii] );
            }
            
            vector<vector<int>> lines;
            vector<vector<int>> corners;
            
            // work with contours
            for( int ii = 0; ii < contours.size(); ii++ ) {
                Scalar color = Scalar( 0, 255, 0);
                
                int startPoint = ii;
                
                // get the two closest points
                int *nbees = getNeighbours(&startPoint, &center);
                
                // the three angles of the triable with the 3 points
                double *angles = getAngles(center[startPoint], center[nbees[0]], center[nbees[1]]);
                
                //Debug output
                //cout << "POINTS: " << startPoint << "|" << nbees[0] << "|" << nbees[1] << endl;
                //cout << "ANGLEEEEs: " << angles[0] << " | " << angles[1] << " | " << angles[2] << " | " << endl;
                putText(*rgbImg, to_string(ii), cvPoint(center[ii].x, center[ii].y+13), FONT_HERSHEY_DUPLEX, 0.3, Scalar(0,0,255), 1, CV_AA);
                
                // try to identifty whether a point is at a corner or in a line
                int pointProp = identifyPoint(angles);
                
                switch(pointProp){
                    case CORNER:
                        if(corners.size() > 0) {
                            for(int iii = 0; iii < corners.size(); iii++) {
                                if(!closeToCorner(center[startPoint], center[corners[iii][0]])) {
                                    corners.push_back({startPoint, nbees[0], nbees[1]});
                                    color = Scalar( 0, 0, 0);
                                }
                            }
                        }
                        else {
                            corners.push_back({startPoint, nbees[0], nbees[1]});
                            color = Scalar( 0, 0, 0);
                        }
                        break;
                    case SIDE:
                        lines.push_back({startPoint, nbees[0], nbees[1]});
                        break;
                    case UNKNOWN:
                        color = Scalar( 0, 0, 255);
                        break;
                }
                
                circle(*rgbImg, center[ii], 5, color, 1, 20, 0 );
            }
            
            // GET THE POINTS OF THE 4 SIDES
            vector<vector<int>> sides = getSides(lines);
            vector<Point2f> cornerPoints;
            
            for(int ii = 0; ii < corners.size(); ii++) {
                bool gotIt = false;
               
                for(int iii = 0; iii < cornerPoints.size(); iii++){
                    if(center[corners[ii][0]] == cornerPoints[iii]) {
                        gotIt = true;
                    }
                }
                if(!gotIt) {
                    cornerPoints.push_back(center[corners[ii][0]]);
                }
            }
            
            // CALCULATE MISSING CORNERS
            vector<vector<Point>> blindLines = getBlindLines(corners, sides, center);
            
            for(int ii = 0; ii < blindLines.size(); ii++) {
                line(*rgbImg, blindLines[ii][0], blindLines[ii][1], Scalar(0,0,200), 1, 20, 0 );
            }
        
            if(corners.size() <= 4) {
                vector<Point2f> intersections = findIntersectionPoints(blindLines);
        
                for(int ii = 0; ii < intersections.size(); ii++) {
                    bool valid = true;
                    
                    for(int iii = 0; iii < corners.size(); iii++) {
                        if(closeToCorner(intersections[ii], center[corners[iii][0]])) {
                            valid = false;
                        }
                    }
                    
                    if(valid) {
                        bool equal = false;
                        for(int iii = 0; iii < cornerPoints.size(); iii++) {
                            if(intersections[ii] == cornerPoints[iii]) {
                                equal = true;
                            }
                        }
                        
                        if(!equal) {
                            cornerPoints.push_back(intersections[ii]);
                        }
                    }
                }
            }
            
            // sort the points in the vector
            sort(cornerPoints.begin(), cornerPoints.end(), pointSorter);
            
            for(int x = 0; x < cornerPoints.size(); x++) {
                cout << cornerPoints[x] << endl;
                circle(*rgbImg, cornerPoints[x], 5, Scalar(160, 0, 255), 1, 20, 0 );
            }
            
            imshow("pagePhoto" + to_string(i), *rgbImg);
            moveWindow("pagePhoto" + to_string(i), 300, 0);

            if(cornerPoints.size() < 40) {
                // log out sides
                //logSides(sides);
                
                ////// TRANSFORMATION
                // write corner points to array
                Point2f srcQua[4];
                srcQua[0] = cornerPoints[0];
                srcQua[1] = cornerPoints[1];
                srcQua[2] = cornerPoints[2];
                srcQua[3] = cornerPoints[3];
                
                Mat* transformedImg = new Mat[1];
                transformedImg = mapInRect(*rgbImg, srcQua);
                
                imshow("asd" + to_string(i), transformedImg[0]);
                
                //// EXPERIMENTAL TEMPLATE MATCHING
                int highestMatch[2] = {0, 0};
                
                for(int i = 0; i < 13; i++) {
                    int r = matchPossibility(pages[i], transformedImg[0]);
                    if(r > highestMatch[1]) {
                        highestMatch[0] = i;
                        highestMatch[1] = r;
                    }
                }
                
                // we don't need this image anymore
                transformedImg[0].release();
        
                cout << "highest match = " << highestMatch[1] << " has index: " << highestMatch[0] << endl;
                imshow("match" + to_string(i), pages[highestMatch[0]]);
                moveWindow("match" + to_string(i), 0, 0);
                
                // release images that we don't need anymore
                
                transformedImg->release();
                rgbImg->release();
            }
        }
    }
    
    cout << "press any key to end the programm" << endl;
    waitKey(0);

    return 0;
}

/***** GET THE FOUR SIDES OF THE DOCUMENT *****/
/**********************************************/
vector<vector<int>> getSides(vector<vector<int>> lines) {
    vector<int> side1, side2, side3, side4;
    
    for(int i = 0; i < lines.size(); i++) {
        if(inVector(lines[i][0], side1) || side1.empty()) {
            if(!inVector(lines[i][1], side1)) {
                side1.push_back(lines[i][1]);
            }
            if(!inVector(lines[i][2], side1)) {
                side1.push_back(lines[i][2]);
            }
        }
        else if(inVector(lines[i][0], side2) || side2.empty()) {
            if(!inVector(lines[i][1], side2)) {
                side2.push_back(lines[i][1]);
            }
            if(!inVector(lines[i][2], side2)) {
                side2.push_back(lines[i][2]);
            }
        }
        else if(inVector(lines[i][0], side3) || side3.empty()) {
            if(!inVector(lines[i][1], side3)) {
                side3.push_back(lines[i][1]);
            }
            if(!inVector(lines[i][2], side3)) {
                side3.push_back(lines[i][2]);
            }
        }
        else if(inVector(lines[i][0], side4) || side4.empty()) {
            if(!inVector(lines[i][1], side4)) {
                side4.push_back(lines[i][1]);
            }
            if(!inVector(lines[i][2], side4)) {
                side4.push_back(lines[i][2]);
            }
        }
    }
    
    vector<vector<int>> sides = {side1, side2, side3, side4};
    return sides;
}

/************ GET A MATCHING VALUE ************/
/**********************************************/
int matchPossibility(Mat pageImg, Mat matchImg) {
    Mat uno = getChamferImg(pageImg)[0];
    Mat dos = getChamferImg(matchImg)[0];
    
    Mat unoR;
    Size size(uno.cols/5, uno.rows/5);
    resize(uno, unoR, size);
    
    Mat dosR;
    resize(dos, dosR, size);
    
    Mat result;
    compare(unoR , dosR, result , CMP_EQ );
    int similarPixels  = countNonZero(result);
    
    double dist = norm(unoR,dosR,NORM_L2);
    return abs(similarPixels/dist);
}

bool inVector(int val, vector<int>vec) {
    if (find(vec.begin(), vec.end(),val) != vec.end()) {
        return true;
    }
    else {
        return false;
    }
}

int inSameSide(int val1, int val2,  vector<vector<int>> sides) {
    for(int i = 0; i < sides.size(); i++) {
        if (find(sides[i].begin(), sides[i].end(),val1) != sides[i].end()) {
            if (find(sides[i].begin(), sides[i].end(),val2) != sides[i].end()) {
                return i;
            }
        }
    }
    
    return -1;
}

/************ GET NEIGHBOUR POINTS ************/
/**********************************************/
int* getNeighbours(int* baseIndex, vector<Point2f> *points) {
    
    int neighbours[2][2] = {
        {INT_MAX, INT_MAX},
        {INT_MAX, INT_MAX}
    };
    
    for(int i = 0; i < points->size(); i ++) {
        if(i != *baseIndex) {
            int dist = getSimpleDist((*points)[*baseIndex], (*points)[i]);
            
            if(dist < neighbours[0][0] || dist < neighbours[1][0]) {
                if(neighbours[0][0] <= neighbours[1][0]) {
                    neighbours[1][0] = dist;
                    neighbours[1][1] = i;
                }
                else {
                    neighbours[0][0] = dist;
                    neighbours[0][1] = i;
                }
            }
        }
    }
    
    int* result = new int[2];
    result[0] = neighbours[0][1];
    result[1] = neighbours[1][1];
    
    return result;
}

/******* FIND OUT IF A POINT IS AT THE CORNER OR IN A LINE *******/
/*****************************************************************/
int identifyPoint(double* angles) {
    // set the tolerance angle to detect as straight line angle+-
    int cornerTolerance = 20;
    int sideToleranceS = 10;
    int rightAngle = 90;
    
    if ( angles[1] + angles[2] < sideToleranceS) {
        return SIDE;
    }
    else if ( abs(angles[0] - rightAngle) < cornerTolerance ) {
        return CORNER;
    }
    else {
        return UNKNOWN;
    }
}

/**************** ALGORITHMS ****************/
/********************************************/

/**** BACK PROJECTION ****/
Mat backProjection(Mat* sampleHist, Mat* image) {
    // set the channel range to the full 8 Bit
    float channelRange[] = {0.0, 255.0};
    
    // get the number of channel for this image
    int noOfChannels = image->channels();
    
    // get the channels of the image and set the ranges for each one
    const float* channelRanges[noOfChannels];
    int channels[noOfChannels];
    for (int i = 0; i < image->channels(); i++) {
        channels[i] = i;
        channelRanges[i] = channelRange;
    }
    
    // calculate the backprojection and return the result image
    Mat result;
    calcBackProject(image, 1, channels, *sampleHist, result, channelRanges, 255.0);
    
    return result;
}

/**** THRESHOLD ****/
Mat thresholdIMG(Mat* image) {
    Mat threshImage;
    double maxValue = 255;
    
    //adaptiveThreshold(image, threshImage, maxValue, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
    threshold(*image, threshImage, 200, maxValue, THRESH_BINARY);
    
    return threshImage;
}

vector<vector<Point>> getBlindLines(vector<vector<int>> corners, vector<vector<int>> sides, vector<Point2f>center) {
    vector<vector<Point>> blindLines;
    
    for(int ii = 0; ii < corners.size(); ii++) {
        vector<int> edges = getEdges(corners[ii][0], sides);
        
        int missing = corners[ii][1];
        
        for(int iii = 0; iii < sides.size(); iii++) {
            if(inVector(corners[ii][1], sides[iii])) {
                missing = corners[ii][2];
            }
        }
        
        Point a = center[missing];
        Point b = center[corners[ii][0]];
        double dist = getDistance(a, b);
        Point c;
        
        c.x = b.x + (b.x - a.x) / dist * -(10 * dist);
        c.y = b.y + (b.y - a.y) / dist * -(10 * dist);
        
        blindLines.push_back({c, b});
    }
    return blindLines;
}

vector<int> getEdges(int corner, vector<vector<int>> sides) {
    vector<int> edges;
    for(int i = 0; i < sides.size(); i++) {
        if(inVector(corner, sides[i])) {
            edges.push_back(i);
        }
    }
    return edges;
}

vector<Point2f> findIntersectionPoints(vector<vector<Point>> lines) {
    vector<Point2f> matches;
    
    for(int ii = 0; ii < lines.size(); ii++) {
        for(int iii = 1; iii < lines.size(); iii++) {
            Point p1 = lines[ii][0];
            Point q1 = lines[ii][1];
            Point p2 = lines[iii][0];
            Point q2 = lines[iii][1];
            
            double d1x = q1.x - p1.x;
            double d1y = q1.y - p1.y;
            double m1 = d1y/d1x;
            double c1 = p1.y - m1 * p1.x;
            
            double d2x = q2.x - p2.x;
            double d2y = q2.y - p2.y;
            double m2 = d2y/d2x;
            double c2 = p2.y - m2 * p2.x;
            
            if( (m1 - m2) != 0){
                double intersectionX = (c2 - c1) / (m1 - m2);
                double intersectionY = m1 * intersectionX + c1;
                if (intersectionX >= 0 && intersectionY >= 0) {
                    Point match = Point(intersectionX, intersectionY);
                    matches.push_back(match);
                }
            }
        }
    }
    return matches;
}
