#ifndef KDTREE_H
#define KDTREE_H

#include <string>
#include <iostream>
#include <cmath>
#include <limits>
#include <vector>

using namespace std;

struct KDNode {
    string region;
    float growth;
    float cpm;
    KDNode* left;
    KDNode* right;

    KDNode(string r, float g, float c)
        : region(r), growth(g), cpm(c), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
    KDNode* root;

    KDNode* insertRec(KDNode* node, string region, float growth, float cpm, int depth);
    void displayRec(KDNode* node, int depth);
    float distanceSq(float growth1, float cpm1, float growth2, float cpm2);
    void nearestRec(KDNode* node, float targetGrowth, float targetCpm, int depth,
                    KDNode*& bestNode, float& bestDist);

    KDNode* findMinRec(KDNode* node, int targetDim, int depth);
    void rangeSearchRec(KDNode* node,
                        float minGrowth, float maxGrowth,
                        float minCpm, float maxCpm,
                        int depth,
                        vector<KDNode*>& results);

public:
    KDTree();
    void insert(string region, float growth, float cpm);
    void display();
    KDNode* nearestNeighbor(float targetGrowth, float targetCpm);

    KDNode* findMin(int dimension); // 0 = growth, 1 = cpm
    vector<KDNode*> rangeSearch(float minGrowth, float maxGrowth,
                                float minCpm, float maxCpm);
};

#endif