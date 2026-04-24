#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Entry stored in sparse matrix
struct SparseEntry {
    string region;
    int day;
    int cases;
};

class SparseMatrix {
private:
    SparseEntry entries[1000];
    int count;

public:
    SparseMatrix();

    void insert(string region, int day, int cases);
    int get(string region, int day) const;
    void print() const;

    vector<int> extractRegion(string region, int totalDays) const;
    vector<int> extractNonZeroCases(string region) const;

    int getCount() const;
};

#endif