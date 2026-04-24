#include "kdtree.h"

// Constructor
KDTree::KDTree() {
    root = nullptr;
}

// Insert helper
KDNode* KDTree::insertRec(KDNode* node, string region, float growth, float cpm, int depth) {
    if (node == nullptr) {
        return new KDNode(region, growth, cpm);
    }
    int axis = depth % 2;
    if (axis == 0) {
        if (growth < node->growth)
            node->left = insertRec(node->left, region, growth, cpm, depth + 1);
        else
            node->right = insertRec(node->right, region, growth, cpm, depth + 1);
    } else {
        if (cpm < node->cpm)
            node->left = insertRec(node->left, region, growth, cpm, depth + 1);
        else
            node->right = insertRec(node->right, region, growth, cpm, depth + 1);
    }
    return node;
}

// Public insert
void KDTree::insert(string region, float growth, float cpm) {
    root = insertRec(root, region, growth, cpm, 0);
}

// Display helper
void KDTree::displayRec(KDNode* node, int depth) {
    if (node == nullptr) return;
    displayRec(node->left, depth + 1);
    cout << "Region: " << node->region
         << " | Growth: " << node->growth
         << " | CPM: " << node->cpm
         << " | Depth: " << depth << endl;

    displayRec(node->right, depth + 1);
}

// Public display
void KDTree::display() {
    if (root == nullptr) {
        cout << "KD-Tree is empty." << endl;
        return;
    }
    displayRec(root, 0);
}

// Squared Euclidean distance
float KDTree::distanceSq(float growth1, float cpm1, float growth2, float cpm2) {
    float dx = growth1 - growth2;
    float dy = cpm1 - cpm2;
    return dx * dx + dy * dy;
}

// Nearest neighbor helper
void KDTree::nearestRec(KDNode* node, float targetGrowth, float targetCpm, int depth,
                        KDNode*& bestNode, float& bestDist) {
    if (node == nullptr) return;

    float currentDist = distanceSq(node->growth, node->cpm, targetGrowth, targetCpm);

    if (bestNode == nullptr || currentDist < bestDist) {
        bestNode = node;
        bestDist = currentDist;
    }

    int axis = depth % 2;
    KDNode* nextBranch = nullptr;
    KDNode* otherBranch = nullptr;

    if (axis == 0) {
        if (targetGrowth < node->growth) {
            nextBranch = node->left;
            otherBranch = node->right;
        } else {
            nextBranch = node->right;
            otherBranch = node->left;
        }
    } else {
        if (targetCpm < node->cpm) {
            nextBranch = node->left;
            otherBranch = node->right;
        } else {
            nextBranch = node->right;
            otherBranch = node->left;
        }
    }

    nearestRec(nextBranch, targetGrowth, targetCpm, depth + 1, bestNode, bestDist);

    float axisDist;
    if (axis == 0)
        axisDist = (targetGrowth - node->growth) * (targetGrowth - node->growth);
    else
        axisDist = (targetCpm - node->cpm) * (targetCpm - node->cpm);

    if (axisDist < bestDist) {
        nearestRec(otherBranch, targetGrowth, targetCpm, depth + 1, bestNode, bestDist);
    }
}

// Public nearest neighbor
KDNode* KDTree::nearestNeighbor(float targetGrowth, float targetCpm) {
    if (root == nullptr) return nullptr;

    KDNode* bestNode = nullptr;
    float bestDist = numeric_limits<float>::max();

    nearestRec(root, targetGrowth, targetCpm, 0, bestNode, bestDist);
    return bestNode;
}

// Find minimum helper
KDNode* KDTree::findMinRec(KDNode* node, int targetDim, int depth) {
    if (node == nullptr) return nullptr;

    int axis = depth % 2;

    // If current axis matches target dimension, minimum must be in left subtree or current node
    if (axis == targetDim) {
        if (node->left == nullptr)
            return node;
        return findMinRec(node->left, targetDim, depth + 1);
    }

    // Otherwise, minimum could be anywhere
    KDNode* leftMin = findMinRec(node->left, targetDim, depth + 1);
    KDNode* rightMin = findMinRec(node->right, targetDim, depth + 1);
    KDNode* minNode = node;

    if (targetDim == 0) {
        if (leftMin && leftMin->growth < minNode->growth) minNode = leftMin;
        if (rightMin && rightMin->growth < minNode->growth) minNode = rightMin;
    } else {
        if (leftMin && leftMin->cpm < minNode->cpm) minNode = leftMin;
        if (rightMin && rightMin->cpm < minNode->cpm) minNode = rightMin;
    }

    return minNode;
}

// Public findMin
KDNode* KDTree::findMin(int dimension) {
    if (dimension != 0 && dimension != 1) return nullptr;
    return findMinRec(root, dimension, 0);
}

// Range search helper
void KDTree::rangeSearchRec(KDNode* node,
                            float minGrowth, float maxGrowth,
                            float minCpm, float maxCpm,
                            int depth,
                            vector<KDNode*>& results) {
    if (node == nullptr) return;

    // Check if current node lies in range
    if (node->growth >= minGrowth && node->growth <= maxGrowth &&
        node->cpm >= minCpm && node->cpm <= maxCpm) {
        results.push_back(node);
    }

    int axis = depth % 2;

    if (axis == 0) {
        // Growth split
        if (minGrowth <= node->growth)
            rangeSearchRec(node->left, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);
        if (maxGrowth >= node->growth)
            rangeSearchRec(node->right, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);
    } else {
        // CPM split
        if (minCpm <= node->cpm)
            rangeSearchRec(node->left, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);
        if (maxCpm >= node->cpm)
            rangeSearchRec(node->right, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);
    }
}

// Public range search
vector<KDNode*> KDTree::rangeSearch(float minGrowth, float maxGrowth,
                                    float minCpm, float maxCpm) {
    vector<KDNode*> results;
    rangeSearchRec(root, minGrowth, maxGrowth, minCpm, maxCpm, 0, results);
    return results;
}