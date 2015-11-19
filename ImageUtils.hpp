//
//  ImageUtils.hpp
//  Assignment2VS
//
//  Created by Michael Frick on 10/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef imageUtils_hpp
#define imageUtils_hpp

cv::Mat* loadImages(int number_of_images, std::string image_files[], std::string file_location);
cv::Mat* getPages();
cv::Mat* getPhotos();
cv::Mat* getChamferImg(cv::Mat img);

#endif