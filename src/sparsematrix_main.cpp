#include <iostream>
#include <vector>
#include "sparsematrix.h"

using namespace std;

int main() {
    SparseMatrix sm;

    // Insert sample data
    sm.insert("Pakistan", 1, 1000);
    sm.insert("Pakistan", 2, 0);      // should not be stored
    sm.insert("Pakistan", 3, 1100);
    sm.insert("India", 1, 5000);
    sm.insert("India", 2, 6000);
    sm.insert("US", 1, 20000);
    sm.insert("US", 2, 50000);

    cout << " SPARSE MATRIX CONTENTS " << endl;
    sm.print();

    cout << "\n GET FUNCTION " << endl;
    cout << "Pakistan, Day 1: " << sm.get("Pakistan", 1) << endl;
    cout << "Pakistan, Day 2: " << sm.get("Pakistan", 2) << endl; // should return 0
    cout << "India, Day 2: " << sm.get("India", 2) << endl;

    cout << "\n EXTRACT REGION " << endl;
    vector<int> pakistanCases = sm.extractRegion("Pakistan", 5);

    cout << "Pakistan cases: ";
    for (int c : pakistanCases) {
        cout << c << " ";
    }
    cout << endl;

    return 0;
}