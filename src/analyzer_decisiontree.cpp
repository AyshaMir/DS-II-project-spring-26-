#include "analyzer_decisiontree.h"

// 1. Average Growth
float Analyzer::calculateAverageGrowth(const vector<int>& cases) {
    if (cases.size() < 2) return 0;

    float totalGrowth = 0;
    int count = 0;

    for (int i = 1; i < cases.size(); i++) {
        if (cases[i-1] > 0) {
            float growth = ((cases[i] - cases[i-1]) * 100.0) / cases[i-1];
            totalGrowth += growth;
            count++;
        }
    }

    if (count == 0) return 0;
    return totalGrowth / count;
}

// 2. Cases per million (population factor)
float Analyzer::calculateCasesPerMillion(const vector<int>& cases, int population) {
    if (population == 0 || cases.empty()) return 0;

    int n = cases.size();
    int window = min(7, n);  // use 7 days or less if not available

    float sum = 0;

    // take last "window" days
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

    return (increases >= windowSize - 2); // allow slight noise
}

// 4. Decision Tree
string Analyzer::classifyRegion(float growth, bool trend, float cpm) {

    // OUTBREAK: fast spread + large impact + increasing
    if ((growth > 30 && trend) || (cpm > 500 && trend))
        return "OUTBREAK";

    // WARNING: any concerning signal
    else if (growth > 10 || trend || cpm > 100)
        return "WARNING";

    // SAFE: everything under control
    else
        return "SAFE";
}