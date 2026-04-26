#include <iostream>
#include <vector>
#include "sparsematrix.h"

using namespace std;

int main() {
    SparseMatrix sm;

    // INSERT
    sm.insert("Pakistan", 1, 1000);
    sm.insert("Pakistan", 2, 0);      // should not be stored
    sm.insert("Pakistan", 3, 1100);
    sm.insert("India", 1, 5000);
    sm.insert("India", 2, 6000);
    sm.insert("US", 1, 20000);
    sm.insert("US", 2, 50000);

    cout << " SPARSE MATRIX CONTENTS " << endl;
    sm.print();

    // SEARCH
    cout << "\n SEARCH FUNCTION " << endl;
    cout << "Pakistan Day 1: " 
         << (sm.search("Pakistan", 1) ? "Found" : "Not Found") << endl;

    cout << "Pakistan Day 2: " 
         << (sm.search("Pakistan", 2) ? "Found" : "Not Found") << endl;

    // GET (VALUE)
    cout << "\n GET FUNCTION " << endl;
    cout << "Pakistan, Day 1: " << sm.get("Pakistan", 1) << endl;
    cout << "Pakistan, Day 2: " << sm.get("Pakistan", 2) << endl; // 0
    cout << "India, Day 2: " << sm.get("India", 2) << endl;

    // UPDATE
    cout << "\n UPDATE FUNCTION " << endl;
    sm.update("Pakistan", 1, 2000); // update existing
    sm.update("India", 2, 0);       // should DELETE (sparse rule)

    cout << "After update:\n";
    sm.print();

    // DELETE
    cout << "\n DELETE FUNCTION " << endl;
    sm.remove("US", 1);

    cout << "After delete:\n";
    sm.print();

    // EXTRACT REGION
    cout << "\n EXTRACT REGION " << endl;
    vector<int> pakistanCases = sm.extractRegion("Pakistan", 5);

    cout << "Pakistan cases: ";
    for (int c : pakistanCases) {
        cout << c << " ";
    }
    cout << endl;

    return 0;
}