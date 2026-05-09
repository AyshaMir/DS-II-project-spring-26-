#include <iostream>
#include <vector>
#include "kdtree.h"

using namespace std;

int main() {
    KDTree tree;

    cout << "========== KD-TREE TEST MAIN ==========" << endl;

    // INSERT
    cout << "\n[1] INSERT FUNCTION" << endl;

    tree.insert("Pakistan", 4.7, 5.0);
    tree.insert("India", 15.8, 6.4);
    tree.insert("US", 73.3, 303.0);
    tree.insert("Brazil", 40.0, 15.0);
    tree.insert("DRC", 55.5, 30.0);

    cout << "KD-tree after insertions:" << endl;
    tree.display();

    // SEARCH
    cout << "\n[2] SEARCH FUNCTION" << endl;

    KDNode* found = tree.searchRegion("India");

    if (found != nullptr) {
        cout << "Found: " << found->region
             << " | Growth: " << found->growth
             << " | CPM: " << found->cpm << endl;
    } else {
        cout << "India not found." << endl;
    }

    KDNode* notFound = tree.searchRegion("China");

    if (notFound != nullptr) {
        cout << "Found: " << notFound->region
             << " | Growth: " << notFound->growth
             << " | CPM: " << notFound->cpm << endl;
    } else {
        cout << "China not found." << endl;
    }

    // MAX GROWTH
    cout << "\n[3] FIND MAX GROWTH FUNCTION" << endl;

    KDNode* maxGrowth = tree.findMaxGrowth();

    if (maxGrowth != nullptr) {
        cout << "Highest Growth Region: "
             << maxGrowth->region
             << " | Growth: " << maxGrowth->growth
             << " | CPM: " << maxGrowth->cpm << endl;
    }

    // TOP N
    cout << "\n[4] TOP N BY GROWTH FUNCTION" << endl;

    vector<KDNode*> topRegions = tree.getTopNByGrowth(3);

    for (int i = 0; i < (int)topRegions.size(); i++) {
        cout << i + 1 << ". "
             << topRegions[i]->region
             << " | Growth: " << topRegions[i]->growth
             << " | CPM: " << topRegions[i]->cpm << endl;
    }

    // UPDATE
    cout << "\n[5] UPDATE FUNCTION" << endl;

    bool updated = tree.updateRegion("Pakistan", 90.0, 12.5);

    if (updated) {
        cout << "Pakistan updated successfully." << endl;
    } else {
        cout << "Pakistan not found. Update failed." << endl;
    }

    cout << "\nKD-tree after update:" << endl;
    tree.display();

    cout << "\nNew max after update:" << endl;

    maxGrowth = tree.findMaxGrowth();

    if (maxGrowth != nullptr) {
        cout << "Highest Growth Region: "
             << maxGrowth->region
             << " | Growth: " << maxGrowth->growth
             << " | CPM: " << maxGrowth->cpm << endl;
    }

    // NEAREST NEIGHBOR
    cout << "\n[6] NEAREST NEIGHBOR FUNCTION" << endl;

    KDNode* nearest = tree.nearestNeighbor(20.0, 10.0);

    if (nearest != nullptr) {
        cout << "Nearest region to target (20, 10): "
             << nearest->region
             << " | Growth: " << nearest->growth
             << " | CPM: " << nearest->cpm << endl;
    }

    // FIND MINIMUM GROWTH
    cout << "\n[7] FIND MINIMUM GROWTH FUNCTION" << endl;

    KDNode* minGrowth = tree.findMin(0);

    if (minGrowth != nullptr) {
        cout << "Min Growth Region: "
             << minGrowth->region
             << " | Growth: " << minGrowth->growth
             << " | CPM: " << minGrowth->cpm << endl;
    }

    // FIND MINIMUM CPM
    cout << "\n[8] FIND MINIMUM CPM FUNCTION" << endl;

    KDNode* minCpm = tree.findMin(1);

    if (minCpm != nullptr) {
        cout << "Min CPM Region: "
             << minCpm->region
             << " | Growth: " << minCpm->growth
             << " | CPM: " << minCpm->cpm << endl;
    }

    // RANGE SEARCH
    cout << "\n[9] RANGE SEARCH FUNCTION" << endl;

    vector<KDNode*> results = tree.rangeSearch(10.0, 60.0, 5.0, 25.0);

    if (results.empty()) {
        cout << "No regions found in range." << endl;
    } else {
        cout << "Regions found in range [Growth: 10-60, CPM: 5-25]:" << endl;

        for (KDNode* node : results) {
            cout << node->region
                 << " | Growth: " << node->growth
                 << " | CPM: " << node->cpm << endl;
        }
    }

    // DELETE
    cout << "\n[10] DELETE FUNCTION" << endl;

    tree.deleteNode("Brazil");

    cout << "Brazil deleted." << endl;

    cout << "\nKD-tree after delete:" << endl;
    tree.display();

    // SEARCH AFTER DELETE
    cout << "\n[11] SEARCH AFTER DELETE" << endl;

    KDNode* afterDelete = tree.searchRegion("Brazil");

    if (afterDelete != nullptr) {
        cout << "Brazil still found." << endl;
    } else {
        cout << "Brazil not found after delete." << endl;
    }

    cout << "\n========== KD-TREE TEST COMPLETE ==========" << endl;

    return 0;
}