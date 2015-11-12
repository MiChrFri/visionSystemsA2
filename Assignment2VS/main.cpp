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

// function declarations
void displayImage(Mat image);
int* getNeighbours(int baseIndex, vector<Point2f> points);
bool inVector(int val, vector<int>vec);
vector<vector<int>> getSides(vector<vector<int>> lines);

// ALGOS
Mat thresholdIMG(Mat image);
Mat adaptiveThresholdImg(Mat image);

Mat kMeansImg(Mat image);
Mat backProjection(Mat sampleHist, Mat inputImg);
MatND getSampleHist();
MatND getHistogram(Mat image);
int matchPossibility(Mat pageImg, Mat matchImg);

int identifyPoint(double* angles);
double getAngle(Point base, Point neighbour);

// GLOABAL VARS
enum pointProp {
    UNKNOWN, CORNER, SIDE, WRONGNEIGHBOUR
};

// main
int main(int argc, const char * argv[]) {
    
    /************ openCV VERSION ************/
    cout << "openCV version: " << CV_VERSION << "\n\n";
    
    Mat* pages  = getPages();
    Mat* photos = getPhotos();
    
    if(pages->empty()) {
        cout << "loading images failed!" << endl;
    }
    else {
        for(int i = 0; i < 13; i++) {
            //string wi = "page_" + to_string(i);
            //imshow(wi, pages[i]);
        }
    }
    
    if(photos->empty()) {
        cout << "loading images failed!" << endl;
    }
    else {
        for(int i = 0; i < 3; i++) {
            // convert image to HSL
            Mat hlsImg;
            //cvtColor(photos[i], hlsImg, CV_BGR2HLS);
            cvtColor(photos[i], hlsImg, CV_BGR2RGB);
            
            Mat origImage = photos[i];
            
            Mat sampleHist = getSampleHist();
            
            // back projection
            Mat BP = backProjection(sampleHist, hlsImg);
            
            // back threshold
            Mat img = thresholdIMG(BP);
            
            Mat output;
            connectedComponents(img, output);
            
            //find contours
            vector<vector<Point> > contours;
            Mat contourOutput = img.clone();
            
            findContours( contourOutput, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
            
            ////////
            
            RNG rng(12345);
            /// Approximate contours to polygons + get bounding rects and circles
            vector<vector<Point> > contours_poly( contours.size() );
            vector<Rect> boundRect( contours.size() );
            vector<Point2f>center( contours.size() );
            vector<float>radius( contours.size() );
        
            //for( int ii = 0; ii < contours.size(); ii++ ){
            for( int ii = 0; ii < contours.size(); ii++ ){
                approxPolyDP( Mat(contours[ii]), contours_poly[ii], 3, true );
                minEnclosingCircle( (Mat)contours_poly[ii], center[ii], radius[ii] );
            }
            
            vector<vector<int>> lines;
            vector<int> corners;
        
            /// Draw polygonal contour + bonding rects + circles
            for( int ii = 0; ii < contours.size(); ii++ ) {
                Scalar color = Scalar( 0, 255, 0);
                
                int startPoint = ii;
                int *nbees = getNeighbours(startPoint, center);
                
                double *angles = getAngles(center[startPoint], center[nbees[0]], center[nbees[1]]);
                
                //Debug output
                //cout << "POINTS: " << startPoint << "|" << nbees[0] << "|" << nbees[1] << endl;
                //cout << "ANGLEEEEs: " << angles[0] << " | " << angles[1] << " | " << angles[2] << " | " << endl;
                putText(photos[i], to_string(ii), cvPoint(center[ii].x, center[ii].y+13), FONT_HERSHEY_DUPLEX, 0.3, Scalar(0,0,255), 1, CV_AA);
            
                int pointProp = identifyPoint(angles);
                
                switch(pointProp){
                    case CORNER:
                        corners.push_back(startPoint);
                        color = Scalar( 0, 0, 0);
                        break;
                    case SIDE:
                        lines.push_back({startPoint, nbees[0], nbees[1]});
                        break;
                    case UNKNOWN:
                        color = Scalar( 0, 0, 255);
                        break;
                }
                
                circle( origImage, center[ii], 5, color, 1, 20, 0 );
            }
            
            imshow("pagePhoto" + to_string(i), photos[i]);
            moveWindow("pagePhoto" + to_string(i), 300, 0);
            
            // GET THE POINTS OF THE 4 SIDES
            vector<vector<int>> sides = getSides(lines);
            
            
            if(corners.size() != 4 || lines.size() != 16) {
                if(lines.size() != 16) {
                    for(int ii = 0; ii < 4; ii++) {
                        if(sides[ii].size() != 5 && sides[ii].size() != 7){
                            cout << sides[ii].size() << " ⛔️ " << endl;
                        }
                    }
                }
                cout << "---" << endl;
            }
            
            
            if(corners.size() == 4 && lines.size() == 16) {
                cout << "point: " << i << " has " << lines.size() << " lines & " << corners.size() << " corners -> ✅" << endl;
                
                // log out sides
                logSides(sides);
                
                ////// TRANSFORMATION
                // write corner points to array
                Point2f srcQua[4];
                srcQua[0] = center[corners[0]];
                srcQua[1] = center[corners[1]];
                srcQua[2] = center[corners[2]];
                srcQua[3] = center[corners[3]];
                
                Mat* transformedImg = new Mat[1];
                transformedImg = mapInRect(origImage, srcQua);
                
                //// EXPERIMENTAL TEMPLATE MATCHING
                int highestMatch[2] = {0, 0};
                
                for(int i = 0; i < 13; i++) {
                    int r = matchPossibility(pages[i], transformedImg[0]);
                    if(r > highestMatch[1]) {
                        highestMatch[0] = i;
                        highestMatch[1] = r;
                    }
                }
                
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
    if (find(vec.begin(), vec.end(),val)!=vec.end()) {
        return true;
    }
    else {
        return false;
    }
}

/************ GET NEIGHBOUR POINTS ************/
/**********************************************/
int* getNeighbours(int baseIndex, vector<Point2f> points) {

    int neighbours[2][2] = {
        {INT_MAX, INT_MAX},
        {INT_MAX, INT_MAX}
    };
    
    int baseX = points[baseIndex].x;
    int baseY =points[baseIndex].y;

    for(int i = 0; i < points.size(); i ++) {
        if(i != baseIndex) {
            int xDist = abs(baseX - points[i].x);
            int yDist = abs(baseY - points[i].y);
            int dist = xDist + yDist;
            
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


/******* FIND OUT IF A POINT IS AT THE CRONER OR IN A LINE *******/
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
Mat backProjection(Mat sampleHist, Mat image) {
    
    float channelRange[] = {0.0, 255.0};
    const float* channelRanges[]    = {channelRange, channelRange, channelRange};
    
    int noOfChannels = image.channels();
    
    int channels[noOfChannels];
    for (int i = 0; i < image.channels(); i++) {
        channels[i]   = i;
    }
    
    Mat result;
    calcBackProject( &image, 1, channels, sampleHist, result, channelRanges, 255.0);
    
    return result;
}

MatND getHistogram(Mat image) {
    MatND histogram;
    
    int channelNumbers[] = {0, 1, 2};
    int* numberBins     = new int[image.channels()];
    
    for (int i = 0; i < image.channels(); i++) {
        numberBins[i]       = 4.0;
    }
    
    float channelRange[]            = {0.0, 255.0};
    const float* channelRanges[]    = {channelRange, channelRange, channelRange};
    
    calcHist( &image, 1, channelNumbers, Mat(), histogram, image.channels(), numberBins, channelRanges );
    
    //normalize histograms
    normalize(histogram, histogram, 1.0);
    
    return histogram;
}

MatND getSampleHist() {
    // get sample image
    Mat sampleIMG = imread(constant::directory + "/Assignment2VS/BlueBookPixels.png", CV_LOAD_IMAGE_COLOR);
    
    // convert image to HSL
    Mat hlsSample;
    //cvtColor(sampleIMG, hlsSample, CV_BGR2HLS);
    cvtColor(sampleIMG, hlsSample, CV_BGR2RGB);
    
    // get normalized histogram for sample image
    MatND histo = getHistogram(hlsSample);
    
    return histo;
}

// THRESHOLD
Mat thresholdIMG(Mat image) {
    Mat threshImage;
    double maxValue = 255;
    
    //adaptiveThreshold(image, threshImage, maxValue, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
    threshold(image, threshImage, 200, maxValue, THRESH_BINARY);
    
    return threshImage;
}

