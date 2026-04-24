#include "heap.h"

vector<Region> heap;

// Move element up to maintain max-heap property
void heapifyUp(int index) {
    while (index > 0 && heap[(index - 1) / 2].growth < heap[index].growth) {
        swap(heap[index], heap[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

// Move element down to maintain max-heap property
void heapifyDown(int index) {
    int left = 2 * index + 1;
    int right = 2 * index + 2;
    int largest = index;

    if (left < (int)heap.size() && heap[left].growth > heap[largest].growth)
        largest = left;

    if (right < (int)heap.size() && heap[right].growth > heap[largest].growth)
        largest = right;

    if (largest != index) {
        swap(heap[index], heap[largest]);
        heapifyDown(largest);
    }
}

// Insert a new region into heap
void insert(string region, float growth, float cpm) {
    heap.push_back({region, growth, cpm});
    heapifyUp(heap.size() - 1);
}

// Show max element without removing
void showMax() {
    if (!heap.empty()) {
        cout << heap[0].region
             << " | Growth: " << heap[0].growth
             << " | CPM: " << heap[0].cpm << endl;
    } else {
        cout << "Heap is empty." << endl;
    }
}

// Remove and return max element
Region extractMax() {
    if (heap.empty()) {
        return {"", -1, -1};
    }
    Region maxRegion = heap[0];
    heap[0] = heap[heap.size() - 1];
    heap.pop_back();

    if (!heap.empty()) {
        heapifyDown(0);
    }

    return maxRegion;
}

// Show top N regions by growth
void showTopN(int n) {
    vector<Region> temp = heap;

    for (int i = 0; i < n && !temp.empty(); i++) {
        Region maxRegion = temp[0];

        temp[0] = temp[temp.size() - 1];
        temp.pop_back();

        int index = 0;
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < (int)temp.size() && temp[left].growth > temp[largest].growth)
                largest = left;

            if (right < (int)temp.size() && temp[right].growth > temp[largest].growth)
                largest = right;

            if (largest != index) {
                swap(temp[index], temp[largest]);
                index = largest;
            } else {
                break;
            }
        }

        cout << i + 1 << ". "
             << maxRegion.region
             << " | Growth: " << maxRegion.growth
             << " | CPM: " << maxRegion.cpm << endl;
    }
}

// Return heap size
int size() {
    return heap.size();
}

// Check if heap is empty
bool isEmpty() {
    return heap.empty();
}

// Display all heap elements
void display() {
    if (heap.empty()) {
        cout << "Heap is empty." << endl;
        return;
    }

    for (int i = 0; i < (int)heap.size(); i++) {
        cout << "Index " << i << ": "
             << heap[i].region
             << " | Growth: " << heap[i].growth
             << " | CPM: " << heap[i].cpm << endl;
    }
}