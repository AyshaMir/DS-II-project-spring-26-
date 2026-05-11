#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <iomanip>
#include <limits>

#include "dataloader.h"
#include "analyzer_decisiontree.h"
#include "sparsematrix.h"
#include "kdtree.h"

using namespace std;

vector<string> getAllRegions(const vector<Entry>& data) {
    set<string> uniqueRegions;

    for (const auto& e : data) {
        uniqueRegions.insert(e.region);
    }

    return vector<string>(uniqueRegions.begin(), uniqueRegions.end());
}

int getPopulation(const vector<Entry>& data, const string& region) {
    for (const auto& e : data) {
        if (e.region == region) {
            return e.population;
        }
    }

    return 0;
}

int getMaxDay(const vector<Entry>& data) {
    int maxDay = 0;

    for (const auto& e : data) {
        if (e.day > maxDay) {
            maxDay = e.day;
        }
    }

    return maxDay;
}

string chooseDataset() {
    return "data/mpox_dataset.csv";
}

int main() {
    DataLoader loader;
    Analyzer analyzer;
    SparseMatrix sparseMatrix;
    KDTree kdTree;

    cout << endl;
    cout << "        EPIDEMIC OUTBREAK DETECTOR" << endl;
    cout << endl;

    string filename = chooseDataset();

    cout << "\n[1] Loading dataset: " << filename << endl;

    vector<Entry> data = loader.loadCSV(filename);

    if (data.empty()) {
        cout << "No data loaded from CSV." << endl;
        return 0;
    }

    cout << "Dataset loaded successfully." << endl;

    sort(data.begin(), data.end(), [](const Entry& a, const Entry& b) {
        if (a.region == b.region) {
            return a.day < b.day;
        }

        return a.region < b.region;
    });

    cout << "\n[2] Storing disease cases in Sparse Matrix..." << endl;

    for (const auto& e : data) {
        sparseMatrix.insert(e.region, e.day, e.cases);
    }

    cout << "Sparse Matrix created successfully." << endl;

    cout << "\n========== SPARSE MATRIX CONTENTS ==========" << endl;
    sparseMatrix.print();

    vector<string> regions = getAllRegions(data);
    int totalDays = getMaxDay(data);

    cout << "\n[3] Analyzing each region..." << endl;
    cout << "\n========== EPIDEMIC ANALYSIS REPORT ==========" << endl;

    for (const string& region : regions) {
        vector<int> fullCases = sparseMatrix.extractRegion(region, totalDays);
        vector<int> nonZeroCases = sparseMatrix.extractNonZeroCases(region);
        int population = getPopulation(data, region);

        if (nonZeroCases.empty()) {
            cout << "\nRegion: " << region << endl;
            cout << "No non-zero case data found." << endl;
            cout << "Final Status: SAFE" << endl;
            cout << "---------------------------------------------" << endl;
            continue;
        }

        float avgGrowth = analyzer.calculateAverageGrowth(nonZeroCases);
        int windowSize = min(5, (int)nonZeroCases.size());
        bool trend = analyzer.isRisingTrend(nonZeroCases, windowSize);
        float cpm = analyzer.calculateCasesPerMillion(nonZeroCases, population);

        string status = analyzer.classifyRegion(avgGrowth, trend, cpm);

        kdTree.insert(region, avgGrowth, cpm);

        cout << "\nRegion: " << region << endl;

        cout << "Full Cases Timeline: ";
        for (int cases : fullCases) {
            cout << cases << " ";
        }
        cout << endl;

        cout << "Non-zero Cases Used for Analysis: ";
        for (int cases : nonZeroCases) {
            cout << cases << " ";
        }
        cout << endl;

        cout << fixed << setprecision(2);
        cout << "Average Growth: " << avgGrowth << "%" << endl;
        cout << "Cases Per Million: " << cpm << endl;
        cout << "Trend: " << (trend ? "Rising" : "Not Rising") << endl;
        cout << "Final Status: " << status << endl;
        cout << "---------------------------------------------" << endl;
    }

    cout << "\n[4] Storing analyzed region metrics in KD-tree..." << endl;

    cout << "\n========== KD-TREE CONTENTS ==========" << endl;
    kdTree.display();

    cout << "\n========== PRIORITY ANALYSIS USING KD-TREE ==========" << endl;

    KDNode* highestGrowth = kdTree.findMaxGrowth();

    if (highestGrowth != nullptr) {
        cout << "\nHighest Growth Region:" << endl;
        cout << highestGrowth->region
             << " | Growth: " << highestGrowth->growth
             << " | CPM: " << highestGrowth->cpm << endl;
    }

    cout << "\nTop 5 Regions By Growth:" << endl;

    vector<KDNode*> topRegions = kdTree.getTopNByGrowth(5);

    if (topRegions.empty()) {
        cout << "No regions available." << endl;
    } else {
        for (int i = 0; i < (int)topRegions.size(); i++) {
            cout << i + 1 << ". "
                 << topRegions[i]->region
                 << " | Growth: " << topRegions[i]->growth
                 << " | CPM: " << topRegions[i]->cpm << endl;
        }
    }

    cout << "\n========== KD-TREE QUERY OPERATIONS ==========" << endl;

    float targetGrowth = 20.0f;
    float targetCpm = 10.0f;

    KDNode* nearest = kdTree.nearestNeighbor(targetGrowth, targetCpm);

    if (nearest != nullptr) {
        cout << "\nNearest Region to Target Point ("
             << targetGrowth << ", " << targetCpm << "):" << endl;

        cout << nearest->region
             << " | Growth: " << nearest->growth
             << " | CPM: " << nearest->cpm << endl;
    }

    KDNode* minGrowth = kdTree.findMin(0);

    if (minGrowth != nullptr) {
        cout << "\nRegion With Minimum Growth:" << endl;
        cout << minGrowth->region
             << " | Growth: " << minGrowth->growth
             << " | CPM: " << minGrowth->cpm << endl;
    }

    KDNode* minCpm = kdTree.findMin(1);

    if (minCpm != nullptr) {
        cout << "\nRegion With Minimum CPM:" << endl;
        cout << minCpm->region
             << " | Growth: " << minCpm->growth
             << " | CPM: " << minCpm->cpm << endl;
    }

    cout << "\n========== OUTBREAK RISK RANGE SEARCH ==========" << endl;

    float minGrowthRange = 10.0f;
    float maxGrowthRange = 80.0f;
    float minCpmRange = 0.0f;
    float maxCpmRange = 5.0f;

    vector<KDNode*> rangeResults = kdTree.rangeSearch(
        minGrowthRange,
        maxGrowthRange,
        minCpmRange,
        maxCpmRange
    );

    cout << "Searching for regions in range:" << endl;
    cout << "Growth: " << minGrowthRange << " to " << maxGrowthRange << endl;
    cout << "CPM: " << minCpmRange << " to " << maxCpmRange << endl;

    if (rangeResults.empty()) {
        cout << "No regions found in this range." << endl;
    } else {
        cout << "\nRegions Found:" << endl;

        for (KDNode* node : rangeResults) {
            cout << node->region
                 << " | Growth: " << node->growth
                 << " | CPM: " << node->cpm << endl;
        }
    }

    cout << "\n========== KD-TREE CORE FUNCTION DEMO ==========" << endl;

    string demoRegion = "Pakistan";

    KDNode* found = kdTree.searchRegion(demoRegion);

    cout << "\nSearch Region: " << demoRegion << endl;

    if (found != nullptr) {
        cout << "Found: " << found->region
             << " | Growth: " << found->growth
             << " | CPM: " << found->cpm << endl;
    } else {
        cout << demoRegion << " not found." << endl;
    }

    cout << "\nUpdating " << demoRegion << " to Growth = 60.00 and CPM = 20.00..." << endl;

    bool updated = kdTree.updateRegion(demoRegion, 60.00f, 20.00f);

    if (updated) {
        cout << demoRegion << " updated successfully." << endl;
    } else {
        cout << demoRegion << " not found. Update failed." << endl;
    }

    cout << "\nKD-tree after update:" << endl;
    kdTree.display();

    cout << "\nDeleting " << demoRegion << " from KD-tree..." << endl;
    kdTree.deleteNode(demoRegion);

    KDNode* afterDelete = kdTree.searchRegion(demoRegion);

    if (afterDelete == nullptr) {
        cout << demoRegion << " deleted successfully." << endl;
    } else {
        cout << demoRegion << " still exists." << endl;
    }

    cout << "\nKD-tree after delete:" << endl;
    kdTree.display();

    cout << endl;
    cout << "        ANALYSIS COMPLETE" << endl;
    cout << endl;

    return 0;
}