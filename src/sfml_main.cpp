#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

#include "ui.h"
#include "dataloader.h"
#include "sparsematrix.h"
#include "analyzer_decisiontree.h"
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

int main() {
    string filename = "data/mpox_dataset.csv";
    DataLoader loader;
    Analyzer analyzer;
    SparseMatrix sparseMatrix;
    KDTree kdTree;

    vector<Entry> data = loader.loadCSV(filename);

    if (data.empty()) {
        cout << "Error: Dataset could not be loaded." << endl;
        return 1;
    }

    sort(data.begin(), data.end(), [](const Entry& a, const Entry& b) {
        if (a.region == b.region) {
            return a.day < b.day;
        }

        return a.region < b.region;
    });

    for (const auto& e : data) {
        sparseMatrix.insert(e.region, e.day, e.cases);
    }

    vector<string> regions = getAllRegions(data);
    int totalDays = getMaxDay(data);

    vector<RegionPoint> regionPoints;
    vector<RegionTimeline> timelines;

    for (const string& region : regions) {
        vector<int> fullCases = sparseMatrix.extractRegion(region, totalDays);
        vector<int> nonZeroCases = sparseMatrix.extractNonZeroCases(region);
        int population = getPopulation(data, region);

        float avgGrowth = 0.0f;
        float cpm = 0.0f;
        bool trend = false;
        string status = "SAFE";

        if (!nonZeroCases.empty()) {
            avgGrowth = analyzer.calculateAverageGrowth(nonZeroCases);

            int windowSize = min(5, (int)nonZeroCases.size());
            trend = analyzer.isRisingTrend(nonZeroCases, windowSize);

            cpm = analyzer.calculateCasesPerMillion(nonZeroCases, population);

            status = analyzer.classifyRegion(avgGrowth, trend, cpm);

            kdTree.insert(region, avgGrowth, cpm);
        }

        regionPoints.push_back({
            region,
            avgGrowth,
            cpm,
            status
        });

        RegionTimeline timeline;
        timeline.name = region;
        timeline.cases = fullCases;
        timelines.push_back(timeline);
    }

    sf::RenderWindow window(
        sf::VideoMode(WIN_W, WIN_H),
        "Epidemic Outbreak Detector"
    );

    window.setFramerateLimit(60);

    DashboardUI dashboard;

    dashboard.attachKDTree(&kdTree);
    dashboard.attachSparseMatrix(&sparseMatrix, regions, totalDays);

    if (!dashboard.loadFont("data/ARIAL.ttf")) {
        cout << "Error: Could not load font file." << endl;
        cout << "Make sure this exists: data/ARIAL.ttf" << endl;
        return 1;
    }

    dashboard.setBackendData(regionPoints, timelines);

    bool dirty = true;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            dashboard.handleEvent(window, event, dirty);
        }

        dashboard.update(dirty);

        if (dirty) {
            window.clear();
            dashboard.draw(window);
            window.display();
            dirty = false;
        }
    }

    return 0;
}
