//
//  Debug.cpp
//  Assignment2VS
//
//  Created by Michael Frick on 12/11/15.
//  Copyright © 2015 FricknMike. All rights reserved.
//

#include "Debug.hpp"

using namespace std;

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
