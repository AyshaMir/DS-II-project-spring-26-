#include <iostream>
#include <vector>
#include <algorithm>
#include "dataloader.h"
#include "analyzer_decisiontree.h"

using namespace std;

// Extract cases for a region
vector<int> getCasesForRegion(vector<Entry>& data, string region) {
    vector<int> cases;

    for (auto& e : data) {
        if (e.region == region) {
            cases.push_back(e.cases);
        }
    }

    return cases;
}

// Get population for a region
int getPopulation(vector<Entry>& data, string region) {
    for (auto& e : data) {
        if (e.region == region) {
            return e.population;
        }
    }
    return 0;
}

int main() {
    DataLoader loader;
    Analyzer analyzer;

    vector<Entry> data = loader.loadCSV("data/dataset.csv");

    // ADD THIS HERE
    sort(data.begin(), data.end(), [](Entry a, Entry b) {
        if (a.region == b.region)
            return a.day < b.day;
        return a.region < b.region;
    });

    string region = "Canada";  // change this to test

    vector<int> cases = getCasesForRegion(data, region);
    int population = getPopulation(data, region);
    if (cases.empty()) {
        cout << "No data found for region: " << region << endl;
        return 0;
    }

    cout << "Cases for " << region << ": ";
    for (int c : cases) cout << c << " ";
    cout << endl;

    // New improved analysis
    float avgGrowth = analyzer.calculateAverageGrowth(cases);
    bool trend = analyzer.isRisingTrend(cases, 5);   // window size = 5
    float risk = analyzer.calculateCasesPerMillion(cases, population);

    string status = analyzer.classifyRegion(avgGrowth, trend, risk);

    cout << "Average Growth: " << avgGrowth << "%" << endl;
    cout << "Cases per million: " << risk << endl;

    if (trend) {
        cout << "Rising Trend Detected" << endl;
    }

    cout << "Final Status: " << status << endl;

    return 0;
}