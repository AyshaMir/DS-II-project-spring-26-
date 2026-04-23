#ifndef ANALYZER_DECISIONTREE_H
#define ANALYZER_DECISIONTREE_H

#include <vector>
#include <string>

using namespace std;

class Analyzer {
public:
    string classifyRegion(float growth, bool trend, float cpm);
    float calculateAverageGrowth(const vector<int>& cases);
    float calculateCasesPerMillion(const vector<int>& cases, int population);
    bool isRisingTrend(const vector<int>& cases, int windowSize);
};

#endif