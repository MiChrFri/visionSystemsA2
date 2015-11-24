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
    // vars
    int TP = 0; // a page is visible and recognised correctly
    int FP = 0; // an incorrectly recognised page, where EITHER no page  was visible OR a different page was visible
    int TN = 0; // no page visible and no page identified
    
    //FN will never happen because it is using the matching propability
    int FN = 0; // a page is visible but no page was found
    
    
    for(int i = 0; i < constant::numberOfPhotos; i++) {
            if(resultValues[i] >= 0) {
                if(resultValues[i] == correctMatches[i]) {
                    TP++;
                }
                else {
                    FP++;
                }
            }
            else {
                if(resultValues[i] == -1) {
                    TN++;
                }
            }
    }
    cout << "\n\n/******* statistics *******/\n/**************************/\n";
    
    cout << "TP: " << TP << endl;
    cout << "TN: " << TN << endl;
    cout << "FP: " << FP << endl;
    cout << "FN: " << FN << endl;
    
    double precision    = ((double)TP/(double)(TP+FP));
    if(TP == 0 && FP == 0) {
        precision = 1;
    }
    double recall       = ((double)TP/(double)(TP+FN));
    if(TP == 0 && FN == 0) {
        recall = 1;
    }
    double accuracy     = ((double)(TP+TN)/(double)(TP+TN+FP+FN));
    if(TP+TN == 0 && TP+TN+FP+FN == 0) {
        accuracy = 1;
    }
    double specificity  = ((double)TN/(double)(FP+TN));
    if(TN == 0 && FP == 0) {
        specificity = 1;
    }
    double f1           = 2.0*precision*recall/(precision+recall);
    
    cout << endl;
    cout << "precision:   " << precision << endl;
    cout << "recall:      " << recall << endl;
    cout << "accuracy:    " << accuracy << endl;
    cout << "specificity: " << specificity << endl;
    cout << "f1:          " << f1 << endl;
    cout << endl;
    cout << endl;
}

