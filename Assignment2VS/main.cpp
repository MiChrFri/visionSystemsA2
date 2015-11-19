//
//  main.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 07/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

//#include <iostream>
//#include <fstream>
#include <opencv2/opencv.hpp>
#include <cmath>

#include "ImageUtils.hpp"
#include "HistogramUtils.hpp"
#include "GeometricUtils.hpp"
#include "Debug.hpp"
#include "ProcessingAlgos.hpp"

/**** Struct for sorting ****/
struct {
    bool operator() (Point pt1, Point pt2) {
        return (pt1.y > pt2.y);
    }
} pointSorter;

/**** function declarations ****/
void displayImage(Mat image);
int* getNeighbours(int baseIndex, vector<Point2f> points);
bool inVector(int val, vector<int>vec);
int inSameSide(int val1, int val2,  vector<vector<int>> sides);
vector<vector<Point>> getBlindLines(vector<vector<int>> corners, vector<vector<int>> sides, vector<Point2f>center);
vector<Point2f> findIntersectionPoints(vector<vector<Point>> lines);

vector<vector<int>> getSides(vector<vector<int>> lines);
vector<int> getEdges(int corner, vector<vector<int>> sides);

int matchPossibility(Mat pageImg, Mat matchImg);

int identifyPoint(double* angles);
double getAngle(Point base, Point neighbour);

/**** ENUMS ****/
enum pointProp {
    UNKNOWN, CORNER, SIDE, WRONGNEIGHBOUR
};

/**************** main ******************/
/****************************************/
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
        
        //24 crashes
        for(int i = 21; i < 22 ; i++) {
            // use variable name for readability
            Mat rgbImg = photos[i];
            
            // back projection
            Mat BP = backProjection(&sampleHist, &rgbImg);
            
            // back threshold
            Mat img = thresholdIMG(BP);
            
            vector<Point2f> center = getPoints(&img);
            
            vector<vector<int>> lines;
            vector<vector<int>> corners;

            /// loop through points
            for( int ii = 0; ii < center.size(); ii++ ) {
                Scalar color = Scalar( 0, 255, 0);
                
                int startPoint = ii;
                int *nbees = getNeighbours(startPoint, center);
                
                double *angles = getAngles(center[startPoint], center[nbees[0]], center[nbees[1]]);
                
                //Debug output
                //cout << "POINTS: " << startPoint << "|" << nbees[0] << "|" << nbees[1] << endl;
                //cout << "ANGLEEEEs: " << angles[0] << " | " << angles[1] << " | " << angles[2] << " | " << endl;
                //putText(photos[i], to_string(ii), cvPoint(center[ii].x, center[ii].y+13), FONT_HERSHEY_DUPLEX, 0.3, Scalar(0,0,255), 1, CV_AA);
                
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
                
                //circle(rgbImg, center[ii], 5, color, 1, 20, 0 );
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
            
            
            // ADD lines for side lines
            for(int ii = 0; ii < lines.size(); ii++) {
                Point a = center[lines[ii][1]];
                Point b = center[lines[ii][2]];

                double dist = getDistance(a, b);
                Point d1;
                Point d2;
                
                d1.x = b.x + (b.x - a.x) / dist * -(10 * dist);
                d1.y = b.y + (b.y - a.y) / dist * -(10 * dist);
                
                d2.x = b.x + (b.x - a.x) / dist * +(10 * dist);
                d2.y = b.y + (b.y - a.y) / dist * +(10 * dist);
                
                blindLines.push_back({d1, d2});
            }
            
            
            for(int ii = 0; ii < blindLines.size(); ii++) {
                line(rgbImg, blindLines[ii][0], blindLines[ii][1], Scalar(0,0,200), 1, 20, 0 );
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
            
            for(int ii = 0; ii < cornerPoints.size(); ii++) {
                
                for(int iii = 0; iii < cornerPoints.size(); iii++) {
                    
                                    cout << "---> " << cornerPoints[ii] << " COMPARE WITH: " << cornerPoints[iii] << endl;
                    
                    if(getDistance(cornerPoints[ii], cornerPoints[iii]) < 5 && ii != iii) {
                        int averageX = 0.5*(cornerPoints[ii].x + cornerPoints[iii].x);
                        int averageY = 0.5*(cornerPoints[ii].y + cornerPoints[iii].y);
                        
                        cornerPoints[ii] = Point2f(averageX, averageY);
                        cornerPoints.erase(cornerPoints.begin()+iii);
                        iii = 0;
                    }
                }
            }

            sort(cornerPoints.begin(), cornerPoints.end(), pointSorter);
            
            for(int ii = 0; ii < cornerPoints.size(); ii++) {
                circle(rgbImg, cornerPoints[ii], 5, Scalar(160, 0, 255), 1, 20, 0 );
            }
            
            
            // show the orignal photo
            
            
            imshow("pagePhoto" + to_string(i), rgbImg);
            moveWindow("pagePhoto" + to_string(i), 300, 0);
            
            
            // LAST ATTEMPT TO GET POINTS EXPERIMENTAL
            if(cornerPoints.size() < 4) {
                // try to find line via A4 aspect ratio
            }
            ////////////////////
                
            
            if(cornerPoints.size() >= 4) {
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
                transformedImg = mapInRect(rgbImg, srcQua);
                
                #if DEBUG
                imshow("asd" + to_string(i), transformedImg[0]);
                #endif
              
                
                //// EXPERIMENTAL TEMPLATE MATCHING
                int highestMatch[2] = {0, 0};
                
                for(int i = 0; i < 13; i++) {
                    int r = matchPossibility(pages[i], transformedImg[0]);
                    if(r > highestMatch[1]) {
                        highestMatch[0] = i;
                        highestMatch[1] = r;
                    }
                }
                
                // result
                cout << "highest match = " << highestMatch[1] << " has index: " << highestMatch[0] << endl;
               
                
                imshow("match" + to_string(i), pages[highestMatch[0]]);
                moveWindow("match" + to_string(i), 0, 0);
                
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
    Size size(200, 297);
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
int* getNeighbours(int baseIndex, vector<Point2f> points) {
    
    int neighbours[2][2] = {
        {INT_MAX, INT_MAX},
        {INT_MAX, INT_MAX}
    };
    
    for(int i = 0; i < points.size(); i ++) {
        if(i != baseIndex) {
            int dist = getSimpleDist(points[baseIndex], points[i]);
            
            if(dist > 50) {
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


vector<vector<Point>> getBlindLines(vector<vector<int>> corners, vector<vector<int>> sides, vector<Point2f>center) {
    vector<vector<Point>> blindLines;
    
    for(int i = 0; i < corners.size(); i++) {
        vector<int> edges = getEdges(corners[i][0], sides);
        vector<int> missing = {};
        
        for(int ii = 0; ii < sides.size(); ii++) {
            if(!inVector(corners[i][1], sides[ii])) {
                missing.push_back(corners[i][1]);
            }
            if(!inVector(corners[i][2], sides[ii])) {
                missing.push_back(corners[i][2]);
            }
        }
        
        for(int ii = 0; ii < missing.size()/3; ii++) {
            Point a = center[missing[ii]];
            Point b = center[corners[i][0]];
            double dist = getDistance(a, b);
            Point c;
            
            c.x = b.x + (b.x - a.x) / dist * -(10 * dist);
            c.y = b.y + (b.y - a.y) / dist * -(10 * dist);
            
            blindLines.push_back({c, b});
        }
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
    
    for(int i = 0; i < lines.size(); i++) {
        for(int ii = 0; ii < lines.size(); ii++) {
            double angle = abs(angleBetween2Lines(lines[i], lines[ii]));
            int tolerance = 20;

            if( abs(angle - 90) < tolerance ||   abs(angle - 270) < tolerance) {
                Point p1 = lines[i][0];
                Point q1 = lines[i][1];
                Point p2 = lines[ii][0];
                Point q2 = lines[ii][1];
                
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
    }
    return matches;
}
