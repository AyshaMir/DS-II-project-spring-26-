#include "kdtree.h"
#include <algorithm>

KDTree::KDTree() {
    root = nullptr;
}

KDNode* KDTree::insert_recursion(KDNode* node, string region, float growth, float cpm, int depth) {
    if (node == nullptr) {
        return new KDNode(region, growth, cpm);
    }

    int axis = depth % 2;

    if (axis == 0) {
        if (growth < node->growth)
            node->left = insert_recursion(node->left, region, growth, cpm, depth + 1);
        else
            node->right = insert_recursion(node->right, region, growth, cpm, depth + 1);
    } else {
        if (cpm < node->cpm)
            node->left = insert_recursion(node->left, region, growth, cpm, depth + 1);
        else
            node->right = insert_recursion(node->right, region, growth, cpm, depth + 1);
    }

    return node;
}

void KDTree::insert(string region, float growth, float cpm) {
    root = insert_recursion(root, region, growth, cpm, 0);
}

void KDTree::display_recursion(KDNode* node, int depth) {
    if (node == nullptr) return;

    display_recursion(node->left, depth + 1);

    cout << "Region: " << node->region
         << " | Growth: " << node->growth
         << " | CPM: " << node->cpm
         << " | Depth: " << depth << endl;

    display_recursion(node->right, depth + 1);
}

void KDTree::display() {
    if (root == nullptr) {
        cout << "KD-Tree is empty." << endl;
        return;
    }

    display_recursion(root, 0);
}

float KDTree::distanceSq(float growth1, float cpm1, float growth2, float cpm2) {
    float dx = growth1 - growth2;
    float dy = cpm1 - cpm2;
    return dx * dx + dy * dy;
}

void KDTree::nearestRec(KDNode* node, float targetGrowth, float targetCpm, int depth,
                        KDNode*& bestNode, float& bestDist) {
    if (node == nullptr) return;

    float currentDist = distanceSq(node->growth, node->cpm, targetGrowth, targetCpm);

    if (bestNode == nullptr || currentDist < bestDist) {
        bestNode = node;
        bestDist = currentDist;
    }

    int axis = depth % 2;

    KDNode* nextBranch;
    KDNode* otherBranch;

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

KDNode* KDTree::nearestNeighbor(float targetGrowth, float targetCpm) {
    if (root == nullptr) return nullptr;

    KDNode* bestNode = nullptr;
    float bestDist = numeric_limits<float>::max();

    nearestRec(root, targetGrowth, targetCpm, 0, bestNode, bestDist);

    return bestNode;
}

KDNode* KDTree::findMinRec(KDNode* node, int targetDim, int depth) {
    if (node == nullptr) return nullptr;

    int axis = depth % 2;

    if (axis == targetDim) {
        if (node->left == nullptr)
            return node;

        return findMinRec(node->left, targetDim, depth + 1);
    }

    KDNode* leftMin = findMinRec(node->left, targetDim, depth + 1);
    KDNode* rightMin = findMinRec(node->right, targetDim, depth + 1);
    KDNode* minNode = node;

    if (targetDim == 0) {
        if (leftMin && leftMin->growth < minNode->growth)
            minNode = leftMin;

        if (rightMin && rightMin->growth < minNode->growth)
            minNode = rightMin;
    } else {
        if (leftMin && leftMin->cpm < minNode->cpm)
            minNode = leftMin;

        if (rightMin && rightMin->cpm < minNode->cpm)
            minNode = rightMin;
    }

    return minNode;
}

KDNode* KDTree::findMin(int dimension) {
    if (dimension != 0 && dimension != 1) return nullptr;

    return findMinRec(root, dimension, 0);
}

void KDTree::rangeSearchRec(KDNode* node,
                            float minGrowth, float maxGrowth,
                            float minCpm, float maxCpm,
                            int depth,
                            vector<KDNode*>& results) {
    if (node == nullptr) return;

    if (node->growth >= minGrowth && node->growth <= maxGrowth &&
        node->cpm >= minCpm && node->cpm <= maxCpm) {
        results.push_back(node);
    }

    int axis = depth % 2;

    if (axis == 0) {
        if (minGrowth <= node->growth)
            rangeSearchRec(node->left, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);

        if (maxGrowth >= node->growth)
            rangeSearchRec(node->right, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);
    } else {
        if (minCpm <= node->cpm)
            rangeSearchRec(node->left, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);

        if (maxCpm >= node->cpm)
            rangeSearchRec(node->right, minGrowth, maxGrowth, minCpm, maxCpm, depth + 1, results);
    }
}

vector<KDNode*> KDTree::rangeSearch(float minGrowth, float maxGrowth,
                                    float minCpm, float maxCpm) {
    vector<KDNode*> results;

    rangeSearchRec(root, minGrowth, maxGrowth, minCpm, maxCpm, 0, results);

    return results;
}

KDNode* KDTree::searchRec(KDNode* node, string region) {
    if (node == nullptr) return nullptr;

    if (node->region == region) return node;

    KDNode* left = searchRec(node->left, region);

    if (left != nullptr) return left;

    return searchRec(node->right, region);
}

KDNode* KDTree::searchRegion(string region) {
    return searchRec(root, region);
}

bool KDTree::updateRegion(string region, float newGrowth, float newCpm) {
    KDNode* node = searchRegion(region);

    if (node == nullptr) return false;

    deleteNode(region);
    insert(region, newGrowth, newCpm);

    return true;
}

KDNode* KDTree::deleteRec(KDNode* node, string region, int depth) {
    if (node == nullptr) return nullptr;

    int axis = depth % 2;

    if (node->region == region) {
        if (node->right != nullptr) {
            KDNode* minNode = findMinRec(node->right, axis, depth + 1);

            node->region = minNode->region;
            node->growth = minNode->growth;
            node->cpm = minNode->cpm;

            node->right = deleteRec(node->right, minNode->region, depth + 1);
        } else if (node->left != nullptr) {
            KDNode* minNode = findMinRec(node->left, axis, depth + 1);

            node->region = minNode->region;
            node->growth = minNode->growth;
            node->cpm = minNode->cpm;

            node->right = deleteRec(node->left, minNode->region, depth + 1);
            node->left = nullptr;
        } else {
            delete node;
            return nullptr;
        }

        return node;
    }

    node->left = deleteRec(node->left, region, depth + 1);
    node->right = deleteRec(node->right, region, depth + 1);

    return node;
}

void KDTree::deleteNode(string region) {
    root = deleteRec(root, region, 0);
}

void KDTree::collectNodes(KDNode* node, vector<KDNode*>& nodes) {
    if (node == nullptr) return;

    collectNodes(node->left, nodes);
    nodes.push_back(node);
    collectNodes(node->right, nodes);
}

KDNode* KDTree::findMaxGrowth() {
    vector<KDNode*> nodes;
    collectNodes(root, nodes);

    if (nodes.empty()) return nullptr;

    KDNode* maxNode = nodes[0];

    for (KDNode* node : nodes) {
        if (node->growth > maxNode->growth) {
            maxNode = node;
        }
    }

    return maxNode;
}

vector<KDNode*> KDTree::getTopNByGrowth(int n) {
    vector<KDNode*> nodes;
    collectNodes(root, nodes);

    sort(nodes.begin(), nodes.end(), [](KDNode* a, KDNode* b) {
        return a->growth > b->growth;
    });

    if (n < (int)nodes.size()) {
        nodes.resize(n);
    }

    return nodes;
}

KDNode* KDTree::getRoot() {
    return root;
}

vector<KDNode*> KDTree::getAllNodes() {
    vector<KDNode*> nodes;
    collectNodes(root, nodes);
    return nodes;
}
