#include <iostream>
#include <vector>
#include "kdtree.h"

using namespace std;

int main() {
    KDTree tree;

    tree.insert("Pakistan", 4.7, 5.0);
    tree.insert("India", 15.8, 6.4);
    tree.insert("US", 73.3, 303.0);
    tree.insert("Brazil", 40.0, 15.0);

    cout << " KD-Tree Display " << endl;
    tree.display();

    cout << "\n Nearest Neighbor " << endl;
    KDNode* nearest = tree.nearestNeighbor(20.0, 10.0);
    if (nearest) {
        cout << "Nearest region: " << nearest->region
             << " | Growth: " << nearest->growth
             << " | CPM: " << nearest->cpm << endl;
    }

    cout << "\n Find Minimum Growth " << endl;
    KDNode* minGrowth = tree.findMin(0);
    if (minGrowth) {
        cout << "Min Growth Region: " << minGrowth->region
             << " | Growth: " << minGrowth->growth
             << " | CPM: " << minGrowth->cpm << endl;
    }

    cout << "\n Find Minimum CPM " << endl;
    KDNode* minCpm = tree.findMin(1);
    if (minCpm) {
        cout << "Min CPM Region: " << minCpm->region
             << " | Growth: " << minCpm->growth
             << " | CPM: " << minCpm->cpm << endl;
    }

    cout << "\n Range Search " << endl;
    vector<KDNode*> results = tree.rangeSearch(10.0, 50.0, 5.0, 20.0);

    if (results.empty()) {
        cout << "No regions found in range." << endl;
    } else {
        for (KDNode* node : results) {
            cout << "Region: " << node->region
                 << " | Growth: " << node->growth
                 << " | CPM: " << node->cpm << endl;
        }
    }

    return 0;
}