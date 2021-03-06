//
//  Debug.hpp
//  Assignment2VS
//
//  Created by Michael Frick on 12/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#ifndef Debug_hpp
#define Debug_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>

void logVersionNumber();
void logSides(std::vector<std::vector<int>> sides);
bool checkMatch(int photoIndex, int pageIndex);
void printStatistics(int* resultValues);

#endif /* Debug_hpp */
