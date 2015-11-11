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

// IMAGELOADER
Mat* loadImages(int number_of_images, string image_files[], string file_location) {
    
    // Load images
    Mat* images = new Mat[number_of_images];
    
    for (int file_no=0; (file_no < number_of_images); file_no++) {
        string filename(file_location);
        filename.append(image_files[file_no]);

        //COLOR
        images[file_no] = imread(filename, CV_LOAD_IMAGE_COLOR);
        
        if (images[file_no].empty()) {
            cout << "Could not open " << images[file_no] << endl;
        }
    }
    return images;
}

Mat* getPages() {
    /************* GET PAGES *************/
    /*************************************/
    
    /*********** file location ***********/
    string file_location = constant::directory + "Input/pages/";
    const int numberOfPages = 13;
    //const int numberOfPages = 1;
    
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

    return myImages;
}

Mat* getPhotos() {
    /************* GET PHOTOS *************/
    /*************************************/
    string photo_location = constant::directory + "Input/photos/";
    const int numberOfPhotos = 50;
    
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