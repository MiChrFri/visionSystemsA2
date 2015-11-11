//
//  ImageUtils.hpp
//  Assignment2VS
//
//  Created by Michael Frick on 10/10/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef imageUtils_hpp
#define imageUtils_hpp

#endif

using namespace std;
using namespace cv;
// constants
namespace constant {
    const string directory = "/Users/michael/Desktop/Trinity/VisionSystems/Assignment2/";
}

Mat* loadImages(int number_of_images, string image_files[], string file_location);
Mat* getPages();
Mat* getPhotos();

