#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <iostream>
#include <string>

using namespace std;

struct Region {
    string region;
    float growth;
    float cpm;
};

// Global heap vector
extern vector<Region> heap;

// Core heap functions
void heapifyUp(int index);
void heapifyDown(int index);
void insert(string region, float growth, float cpm);
Region extractMax();

// Utility/display functions
void showMax();
void showTopN(int n);
int size();
bool isEmpty();
void display();

#endif