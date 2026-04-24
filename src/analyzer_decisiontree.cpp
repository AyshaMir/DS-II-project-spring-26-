#include "analyzer_decisiontree.h"
#include <cmath> 
// 1. Average Growth
float Analyzer::calculateAverageGrowth(const vector<int>& cases) {
    if (cases.size() < 2) return 0;

    float total = 0;
    int count = 0;

    for (int i = 1; i < cases.size(); i++) {
        int previous = cases[i - 1];
        int current = cases[i];

        if (previous > 0 && current > 0) {
            float growth = log((float)current / previous);
            total += growth;
            count++;
        }
    }

    if (count == 0) return 0;

    return (total / count) * 100;  // scale for readability
}

// 2. Cases per million 
float Analyzer::calculateCasesPerMillion(const vector<int>& cases, int population) {
    if (population == 0 || cases.empty()) return 0;
    int n = cases.size();
    int window = min(7, n);  // use 7 days or less if not available
    float sum = 0;
    // take last window days
    for (int i = n - window; i < n; i++) {
        sum += cases[i];
    }
    float avg = sum / window;
    return (avg * 1000000.0) / population;
}

// 3. Sliding Window Trend
bool Analyzer::isRisingTrend(const vector<int>& cases, int windowSize) {
    if (cases.size() < windowSize) return false;
    int increases = 0;
    for (int i = cases.size() - windowSize + 1; i < cases.size(); i++) {
        if (cases[i] > cases[i - 1]) {
            increases++;
        }
    }
    return (increases >= windowSize - 2);
}

//4. Decision Tree
string Analyzer::classifyRegion(float growth, bool trend, float cpm) {

    // OUTBREAK: strong exponential growth + consistent rise
    if (growth > 30 && trend)
        return "OUTBREAK";

    // WARNING: moderate growth or rising pattern
    else if (growth > 15 && trend)
        return "WARNING";

    // SAFE: low or unstable growth
    else
        return "SAFE";
}