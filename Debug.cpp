//
//  Debug.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 12/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include "Debug.hpp"
#include "Constants.h"

using namespace std;

// correct matches
int correctMatches[25] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 2, 3, 5, 4, 7, 9, 8, 7, 11, 13, 12, 2};

void logVersionNumber() {
    cout << "/********** OpenCV version: " << CV_VERSION << " **********/" << endl;
    cout << "/*******************************************/" << endl;
}

void logSides(vector<vector<int>> sides) {
    cout << "side1: ";
    for(int i = 0; i < sides[0].size(); i++) {
        cout << sides[0][i] << ", ";
    }
    cout << endl << "side2: ";
    for(int i = 0; i < sides[1].size(); i++) {
        cout << sides[1][i] << ", ";
    }
    cout << endl << "side3: ";
    for(int i = 0; i < sides[2].size(); i++) {
        cout << sides[2][i] << ", ";
    }
    cout << endl << "side4: ";
    for(int i = 0; i < sides[3].size(); i++) {
        cout << sides[3][i] << ", ";
    }
    cout << endl;
}

bool checkMatch(int photoIndex, int pageIndex) {
    return correctMatches[photoIndex] == pageIndex? true : false;
}


void printStatistics(int* resultValues) {
//    // vars
//    int TP = 0; // pageNumber == correct    &   detected result == correct
//    int FP = 0; // pageNumber == wrong   &   detected result == true
//    int TN = 0; // pageNumber == false   &   detected result == false
//    int FN = 0; // pageNumber == true    &   detected result == false
//    
//    for(int i = 0; i < constant::numberOfPhotos; i++) {
//            if(resultValues[i] == correctMatches[i]) {
//                if(resultValues[i]) {
//                    TP++;
//                }
//                else {
//                    TN++;
//                }
//            }
//            else {
//                if(resultValues[i]) {
//                    FN++;
//                }
//                else {
//                    FP++;
//                }
//            }
//    }
//    cout << "\n\n/******* statistics *******/\n/**************************/\n";
//    
//    cout << "TP: " << TP << endl;
//    cout << "TN: " << TN << endl;
//    cout << "FP: " << FP << endl;
//    cout << "FN: " << FN << endl;
//    
//    double precision    = ((double)TP/(double)(TP+FP));
//    double recall       = ((double)TP/(double)(TP+FN));
//    double accuracy     = ((double)(TP+TN)/(double)(TP+TN+FP+FN));
//    double specificity  = ((double)TN/(double)(FP+TN));
//    double f1           = 2.0*precision*recall/(precision+recall);
//    
//    cout << endl;
//    cout << "precision:   " << precision << endl;
//    cout << "recall:      " << recall << endl;
//    cout << "accuracy:    " << accuracy << endl;
//    cout << "specificity: " << specificity << endl;
//    cout << "f1:          " << f1 << endl;
//    cout << endl;
//    cout << endl;
}

