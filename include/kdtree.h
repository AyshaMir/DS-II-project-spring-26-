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

    KDNode* insert_recursion(KDNode* node, string region, float growth, float cpm, int depth);
    void display_recursion(KDNode* node, int depth);

    float distanceSq(float growth1, float cpm1, float growth2, float cpm2);

    void nearestRec(KDNode* node, float targetGrowth, float targetCpm, int depth,
                    KDNode*& bestNode, float& bestDist);

    KDNode* findMinRec(KDNode* node, int targetDim, int depth);

    void rangeSearchRec(KDNode* node,
                        float minGrowth, float maxGrowth,
                        float minCpm, float maxCpm,
                        int depth,
                        vector<KDNode*>& results);

    KDNode* searchRec(KDNode* node, string region);
    KDNode* deleteRec(KDNode* node, string region, int depth);

    void collectNodes(KDNode* node, vector<KDNode*>& nodes);

public:
    KDTree();

    void insert(string region, float growth, float cpm);
    void display();

    KDNode* searchRegion(string region);
    KDNode* nearestNeighbor(float targetGrowth, float targetCpm);
    KDNode* findMin(int dimension);

    vector<KDNode*> rangeSearch(float minGrowth, float maxGrowth,
                                float minCpm, float maxCpm);

    bool updateRegion(string region, float newGrowth, float newCpm);
    void deleteNode(string region);

    KDNode* findMaxGrowth();
    vector<KDNode*> getTopNByGrowth(int n);

    KDNode* getRoot();
    vector<KDNode*> getAllNodes();
};

#endif
