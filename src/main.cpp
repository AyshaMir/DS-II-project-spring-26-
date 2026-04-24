#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <iomanip>

#include "dataloader.h"
#include "analyzer_decisiontree.h"
#include "sparsematrix.h"
#include "kdtree.h"
#include "heap.h"

using namespace std;

// Get all unique regions
vector<string> getAllRegions(const vector<Entry>& data) {
    set<string> uniqueRegions;

    for (const auto& e : data) {
        uniqueRegions.insert(e.region);
    }

    return vector<string>(uniqueRegions.begin(), uniqueRegions.end());
}

// Get population for a region
int getPopulation(const vector<Entry>& data, const string& region) {
    for (const auto& e : data) {
        if (e.region == region) {
            return e.population;
        }
    }
    return 0;
}

// Get maximum day in dataset
int getMaxDay(const vector<Entry>& data) {
    int maxDay = 0;
    for (const auto& e : data) {
        if (e.day > maxDay) {
            maxDay = e.day;
        }
    }
    return maxDay;
}

int main() {
    heap.clear();
    DataLoader loader;
    Analyzer analyzer;
    SparseMatrix sm;
    KDTree tree;

    // Load CSV
    vector<Entry> data = loader.loadCSV("data/dataset.csv");

    if (data.empty()) {
        cout << "No data loaded from CSV." << endl;
        return 0;
    }

    // Sort by region then day
    sort(data.begin(), data.end(), [](const Entry& a, const Entry& b) {
        if (a.region == b.region)
            return a.day < b.day;
        return a.region < b.region;
    });

    // Insert into sparse matrix
    for (const auto& e : data) {
        sm.insert(e.region, e.day, e.cases);
    }

    vector<string> regions = getAllRegions(data);
    int totalDays = getMaxDay(data);

    cout << " SPARSE MATRIX CONTENTS \n" << endl;
    sm.print();

    cout << "\n EPIDEMIC ANALYSIS REPORT \n" << endl;

    // Process each region
    for (const string& region : regions) {
        vector<int> fullCases = sm.extractRegion(region, totalDays);      // for display
        vector<int> cases = sm.extractNonZeroCases(region);               // for analysis
        int population = getPopulation(data, region);

        if (cases.empty()) {
            cout << "No data found for region: " << region << endl;
            continue;
        }

        // Analyzer
        float avgGrowth = analyzer.calculateAverageGrowth(cases);
        int windowSize = min(5, (int)cases.size());
        bool trend = analyzer.isRisingTrend(cases, windowSize);
        float cpm = analyzer.calculateCasesPerMillion(cases, population);

        // Decision tree classification
        string status = analyzer.classifyRegion(avgGrowth, trend, cpm);

        // Heap insertion (priority queue using heap)
        insert(region, avgGrowth, cpm);

        // KD-tree insertion
        tree.insert(region, avgGrowth, cpm);

        // Region report
        cout << "Region: " << region << endl;

        cout << "Full Cases Timeline: ";
        for (int c : fullCases) {
            cout << c << " ";
        }
        cout << endl;

        cout << "Non-zero Cases Used for Analysis: ";
        for (int c : cases) {
            cout << c << " ";
        }
        cout << endl;

        cout << fixed << setprecision(2);
        cout << "Average Growth: " << avgGrowth << "%" << endl;
        cout << "Cases per Million: " << cpm << endl;
        cout << "Trend: " << (trend ? "Rising" : "Not Rising") << endl;
        cout << "Final Status: " << status << endl;
        cout << "---------------------------------------------" << endl;
    }

    // Heap section
    cout << "\n HEAP / PRIORITY QUEUE \n" << endl;

    cout << "Heap contents (array representation):" << endl;
    display();

    cout << "\nHighest priority region (max growth):" << endl;
    showMax();

    cout << "\nTop 2 regions by growth:" << endl;
    showTopN(2);

    // KD-tree section
    cout << "\n KD-TREE CONTENTS \n" << endl;
    tree.display();

    cout << "\n KD-TREE QUERIES \n" << endl;

    float targetGrowth = 20.0f;
    float targetCpm = 10.0f;

    KDNode* nearest = tree.nearestNeighbor(targetGrowth, targetCpm);
    if (nearest != nullptr) {
        cout << "Nearest region to target (" << targetGrowth << ", " << targetCpm << "): "
             << nearest->region
             << " | Growth: " << nearest->growth
             << " | CPM: " << nearest->cpm << endl;
    }

    KDNode* minGrowth = tree.findMin(0);
    if (minGrowth != nullptr) {
        cout << "Region with Minimum Growth: " << minGrowth->region
             << " | Growth: " << minGrowth->growth
             << " | CPM: " << minGrowth->cpm << endl;
    }

    KDNode* minCpm = tree.findMin(1);
    if (minCpm != nullptr) {
        cout << "Region with Minimum CPM: " << minCpm->region
             << " | Growth: " << minCpm->growth
             << " | CPM: " << minCpm->cpm << endl;
    }

    float minGrowthRange = 10.0f, maxGrowthRange = 50.0f;
    float minCpmRange = 0.0f, maxCpmRange = 0.1f;

    vector<KDNode*> rangeResults = tree.rangeSearch(
        minGrowthRange, maxGrowthRange,
        minCpmRange, maxCpmRange
    );

    cout << "\nRegions in Range [Growth: " << minGrowthRange
         << " to " << maxGrowthRange
         << ", CPM: " << minCpmRange
         << " to " << maxCpmRange << "]:" << endl;

    if (rangeResults.empty()) {
        cout << "No regions found in the specified range." << endl;
    } else {
        for (KDNode* node : rangeResults) {
            cout << "Region: " << node->region
                 << " | Growth: " << node->growth
                 << " | CPM: " << node->cpm << endl;
        }
    }

    return 0;
}