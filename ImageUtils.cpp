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
        Rect myROI(10, 10, myImages[i].cols - 20, myImages[i].rows - 20);
        
        Size size(300,448);
        Mat resizedImage;
        resize(myImages[i](myROI),croppedImgs[i],size);
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
        
        cout << imgName << endl;
        
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