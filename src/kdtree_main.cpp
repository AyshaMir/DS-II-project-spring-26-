#include <iostream>
#include <vector>
#include "kdtree.h"

using namespace std;

int main() {
    KDTree tree;

    // INSERT
    tree.insert("Pakistan", 4.7, 5.0);
    tree.insert("India", 15.8, 6.4);
    tree.insert("US", 73.3, 303.0);
    tree.insert("Brazil", 40.0, 15.0);

    cout << " KD-TREE CONTENTS AFTER INSERT " << endl;
    tree.display();

    // SEARCH
    cout << "\n SEARCH FUNCTION " << endl;

    KDNode* found = tree.searchRegion("India");
    if (found) {
        cout << "Found: " << found->region
             << " | Growth: " << found->growth
             << " | CPM: " << found->cpm << endl;
    } else {
        cout << "India not found." << endl;
    }

    KDNode* notFound = tree.searchRegion("China");
    if (notFound) {
        cout << "Found: " << notFound->region
             << " | Growth: " << notFound->growth
             << " | CPM: " << notFound->cpm << endl;
    } else {
        cout << "China not found." << endl;
    }

    // UPDATE
    cout << "\n UPDATE FUNCTION " << endl;

    bool updated = tree.updateRegion("India", 50.0, 20.0);

    if (updated) {
        cout << "India updated successfully." << endl;
    } else {
        cout << "India not found, update failed." << endl;
    }

    cout << "\n KD-TREE AFTER UPDATE " << endl;
    tree.display();

    // NEAREST NEIGHBOR
    cout << "\n NEAREST NEIGHBOR " << endl;

    KDNode* nearest = tree.nearestNeighbor(20.0, 10.0);
    if (nearest) {
        cout << "Nearest region: " << nearest->region
             << " | Growth: " << nearest->growth
             << " | CPM: " << nearest->cpm << endl;
    }

    // FIND MINIMUM GROWTH
    cout << "\n FIND MINIMUM GROWTH " << endl;

    KDNode* minGrowth = tree.findMin(0);
    if (minGrowth) {
        cout << "Min Growth Region: " << minGrowth->region
             << " | Growth: " << minGrowth->growth
             << " | CPM: " << minGrowth->cpm << endl;
    }

    // FIND MINIMUM CPM
    cout << "\n FIND MINIMUM CPM " << endl;

    KDNode* minCpm = tree.findMin(1);
    if (minCpm) {
        cout << "Min CPM Region: " << minCpm->region
             << " | Growth: " << minCpm->growth
             << " | CPM: " << minCpm->cpm << endl;
    }

    // RANGE SEARCH
    cout << "\n RANGE SEARCH " << endl;

    vector<KDNode*> results = tree.rangeSearch(10.0, 60.0, 5.0, 25.0);

    if (results.empty()) {
        cout << "No regions found in range." << endl;
    } else {
        for (KDNode* node : results) {
            cout << "Region: " << node->region
                 << " | Growth: " << node->growth
                 << " | CPM: " << node->cpm << endl;
        }
    }

    // DELETE
    cout << "\n DELETE FUNCTION " << endl;

    tree.deleteNode("India");

    cout << "India deleted." << endl;

    cout << "\n KD-TREE AFTER DELETE " << endl;
    tree.display();

    // SEARCH AFTER DELETE
    cout << "\n SEARCH AFTER DELETE " << endl;

    KDNode* afterDelete = tree.searchRegion("India");
    if (afterDelete) {
        cout << "India still found." << endl;
    } else {
        cout << "India not found after delete." << endl;
    }

    return 0;
}