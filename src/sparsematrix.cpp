#include "sparsematrix.h"

// Constructor
SparseMatrix::SparseMatrix() {
    count = 0;
}

// Insert only non-zero entries
void SparseMatrix::insert(string region, int day, int cases) {
    if (cases > 0 && count < 1000) {
        entries[count].region = region;
        entries[count].day = day;
        entries[count].cases = cases;
        count++;
    }
}

// Get cases for a given region and day
int SparseMatrix::get(string region, int day) const {
    for (int i = 0; i < count; i++) {
        if (entries[i].region == region && entries[i].day == day) {
            return entries[i].cases;
        }
    }
    return 0; // return 0 if not found
}

// Print all stored non-zero entries
void SparseMatrix::print() const {
    cout << "Stored entries:" << endl;
    for (int i = 0; i < count; i++) {
        cout << "(" << entries[i].region
             << ", Day " << entries[i].day
             << ", Cases: " << entries[i].cases
             << ")" << endl;
    }
    cout << "Total stored: " << count << " entries" << endl;
}

// Extract all days of a region into a vector
vector<int> SparseMatrix::extractRegion(string region, int totalDays) const {
    vector<int> cases(totalDays);

    for (int i = 0; i < totalDays; i++) {
        cases[i] = get(region, i + 1);
    }

    return cases;
}

// Return total stored entries
int SparseMatrix::getCount() const {
    return count;
}

vector<int> SparseMatrix::extractNonZeroCases(string region) const {
    vector<int> cases;

    for (int i = 0; i < count; i++) {
        if (entries[i].region == region) {
            cases.push_back(entries[i].cases);
        }
    }

    return cases;
}