//
//  ImageUtils.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 10/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include <opencv2/opencv.hpp>
#include "ImageUtils.hpp"
#include <opencv2/imgproc.hpp>
#include "Constants.h"
#include "HistogramUtils.hpp"
#include "ProcessingAlgos.hpp"

/*** function declarations ***/
Rect getContentRect(Mat* img);


// IMAGELOADER
Mat* loadImages(int number_of_images, string image_files[], string file_location) {
    Mat* images = new Mat[number_of_images];
    
    for (int file_no=0; (file_no < number_of_images); file_no++) {
        string filename(file_location);
        filename.append(image_files[file_no]);
        
        //read img
        images[file_no] = imread(filename, CV_LOAD_IMAGE_COLOR);
        
        if (images[file_no].empty()) {
            cout << "Could not open " << images[file_no] << endl;
        }
    }
    return images;
}

Mat* getPages() {
    /*********** file location ***********/
    string file_location = constant::directory + "Input/pages/";
    const int numberOfPages = 13;

    /*********** test files ***********/
    string image_files[numberOfPages];
    
    /******** create imageNames *******/
    for(int i = 0; i < numberOfPages; i++) {
        string imgName = "Page";
        
        imgName += i < 9 ? "0":"";
        imgName += to_string(i+1) + ".jpg";
        
        image_files[i] = imgName;
    }
    
    int numberOfImages = sizeof(image_files)/sizeof(image_files[0]);
    
    /*********** load images ***********/
    Mat* myImages = loadImages(numberOfImages, image_files, file_location);
    
    /*********** crop and resize images ***********/
    Mat* croppedImgs = new Mat[numberOfImages];
    
    for(int i = 0; i < numberOfImages; i++) {
        Rect documentRect = getContentRect(&myImages[i]);
        
        resize(myImages[i](documentRect),croppedImgs[i],constant::size);
    }
    
    return croppedImgs;
}

Mat* getPhotos() {
    /*********** file location ***********/
    string photo_location = constant::directory + "Input/photos/";
    const int numberOfPhotos = 25;
    
    /*********** test files ***********/
    string photo_files[numberOfPhotos];
    
    /******** create imageNames *******/
    for(int i = 0; i < numberOfPhotos; i++) {
        string imgName = "BookView";
        
        imgName += i < 9 ? "0":"";
        imgName += to_string(i+1) + ".JPG";
        
        photo_files[i] = imgName;
    }
    
    int numberOfPictures = sizeof(photo_files)/sizeof(photo_files[0]);
    cout << numberOfPictures << " photos loaded" << endl;
    
    /*********** load images ***********/
    Mat* photos = loadImages(numberOfPictures, photo_files, photo_location);
    
    Mat* resizedImage = new Mat[numberOfPictures];

    for(int i = 0; i < numberOfPhotos; i++) {
        Size size(photos[i].cols/2, photos[i].rows/2);
        
        resize(photos[i], resizedImage[i], size);
    }
    
    return resizedImage;
}

Mat* getChamferImg(Mat img) {
    
    // convert to grayscale
    Mat grayImg;
    cvtColor(img,grayImg,CV_RGB2GRAY);
    
    // canny
    Mat edgeImg;
    Canny(grayImg, edgeImg, 100, 200, 5);
    
    // threshold img
    Mat threshImage;
    adaptiveThreshold(edgeImg, threshImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 3, 2);
    
    // chamfer img
    Mat chamfImg;
    distanceTransform(threshImage, chamfImg, CV_DIST_L2, 3);
    
    
    Mat* chmfrImg = new Mat;
    chmfrImg[0] = chamfImg;
    
    return chmfrImg;
}

Rect getContentRect(Mat* img) {
    vector<Point2f> cornerPoints;
    double area = 50.0;
    Size size(area, area);
    
    Mat sampleHist = getSampleHist();
    
    // back projection
    Mat BP = backProjection(&sampleHist, img);
    
    // threshold image
    Mat threshImage;
    double maxValue = 255;
    threshold(BP, threshImage, 50, maxValue, THRESH_BINARY);
    
    // close image
    Mat closed_image;
    
    Mat three_by_three_element( 3, 3, CV_8U, Scalar(1) );
    morphologyEx( threshImage, closed_image, MORPH_OPEN, three_by_three_element);
    
    double imgWidth = closed_image.cols;
    double imgHeight = closed_image.rows;
    
    Rect cornerRects[] = {
        Rect(0               , 0             , area, area),
        Rect(0               , imgWidth-area , area, area),
        Rect(imgHeight-area  , 0             , area, area),
        Rect(imgHeight-area  , imgWidth-area , area, area)
    };

    for(int i = 0; i < 4; i++) {
        Rect rect = cornerRects[0];
        
        Mat part;
        resize(closed_image(rect), part, size);
        
        Point2f returnedPoint = getPoints(&part)[0];
        cornerPoints.push_back(returnedPoint);
    }
    
    //apply positions to the whole image
    int pLUx = round(cornerPoints[0].x);
    int pLUy = round(cornerPoints[0].y);
    
    int pRUx = imgWidth - round(cornerPoints[1].x);
    //int pRUy = round(cornerPoints[1].y);
    
    //int pLDx = round(cornerPoints[2].x);
    int pLDy = imgHeight- round(cornerPoints[2].y);
    
    //int pRDx = imgWidth - round(cornerPoints[3].x);
    //int pRDy = imgHeight - round(cornerPoints[3].y);
    
    
    double rectWidth = pRUx - pLUx;
    double rectHeight = pLDy - pLUy;
    
    return Rect(pLUx, pLUy, rectWidth, rectHeight);
}