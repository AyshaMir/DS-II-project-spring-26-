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

bool SparseMatrix::search(string region, int day) const {
    for (int i = 0; i < count; i++) {
        if (entries[i].region == region && entries[i].day == day) {
            return true;
        }
    }
    return false;
}

bool SparseMatrix::update(string region, int day, int newCases) {
    for (int i = 0; i < count; i++) {
        if (entries[i].region == region && entries[i].day == day) {
            if (newCases > 0) {
                entries[i].cases = newCases;
            } else {
                // if updated to 0 → remove entry (sparse logic)
                remove(region, day);
            }
            return true;
        }
    }
    return false;
}

bool SparseMatrix::remove(string region, int day) {
    for (int i = 0; i < count; i++) {
        if (entries[i].region == region && entries[i].day == day) {

            // shift left to fill gap
            for (int j = i; j < count - 1; j++) {
                entries[j] = entries[j + 1];
            }

            count--;
            return true;
        }
    }
    return false;
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