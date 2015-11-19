//
//  HistogramUtils.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 08/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include "HistogramUtils.hpp"
#include "Constants.h"

using namespace std;
using namespace cv;

/* get the histogram for an image */
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

/* get the histogram for our sample image for the backprojection */
MatND getSampleHist() {
    // get sample image
    Mat sampleIMG = imread(constant::directory + "/Assignment2VS/BlueBookPixels.png", CV_LOAD_IMAGE_COLOR);
    
    // get normalized histogram for sample image
    MatND histo = getHistogram(sampleIMG);
    
    return histo;
}
