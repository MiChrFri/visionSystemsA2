//
//  ProcessingAlgos.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 18/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include "ProcessingAlgos.hpp"

using namespace std;
using namespace cv;

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
Mat thresholdIMG(Mat image) {
    Mat threshImage;
    double maxValue = 255;
    
    //adaptiveThreshold(image, threshImage, maxValue, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 11, 2);
    threshold(image, threshImage, 200, maxValue, THRESH_BINARY);
    
    return threshImage;
}

vector<Point2f> getPoints(Mat* img) {
    //find contours
    vector<vector<Point> > contours;
    //Mat contourOutput = img.clone();
    
    findContours(img[0], contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    
    //for( int ii = 0; ii < contours.size(); ii++ ){
    for( int ii = 0; ii < contours.size(); ii++ ){
        approxPolyDP( Mat(contours[ii]), contours_poly[ii], 3, true );
        minEnclosingCircle( (Mat)contours_poly[ii], center[ii], radius[ii] );
    }
    
    return center;
}
