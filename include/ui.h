#ifndef UI_H
#define UI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "DashboardData.h"
#include "kdtree.h"
#include "sparsematrix.h"

using namespace std;

class DashboardUI {
private:
    sf::Font font;
    vector<Button> sidebar;
    vector<InputBox> inputs;

    Screen currentScreen;
    string outputText;
    int focusedBox;
    bool cursorOn;
    bool runHover;
    sf::Clock cursorClock;

    sf::RectangleShape runButton;
    sf::Text runText;

    vector<RegionPoint> regions;
    vector<RegionTimeline> timelines;
    vector<TrendPoint> selectedTrend;
    string selectedTrendName;

    KDTree* backendTree;
    SparseMatrix* backendSparse;
    vector<string> backendRegionNames;
    int backendTotalDays;

    int searchIndex;
    vector<bool> rangeHighlight;
    vector<bool> nearestHighlight;
    vector<int> topRiskIndexes;
    sf::Vector2f nearestTarget;

    float minGrowth, maxGrowth, minCpm, maxCpm;
    float targetGrowth, targetCpm;
    int kValue;
    GraphMode topRiskGraphMode;

    void createSidebar();
    void createInputs();
    void createRunButton();

    vector<int> activeInputs() const;
    void setOutput(const string& text);
    void initOutput(Screen screen);
    void computeOutput();

    float toFloat(const string& value, float fallback) const;
    int toInt(const string& value, int fallback) const;

    vector<RegionPoint> currentTopRiskRegions() const;

    void rebuildRegionsFromKDTree();
    void rebuildTimelinesFromSparse();

    void updateSelectedRegion();
    void deleteSelectedRegion();

    void insertSparseEntry();
    void updateSparseEntry();
    void deleteSparseEntry();

    void drawOutputPanel(sf::RenderWindow& window);
    void drawSidebar(sf::RenderWindow& window);
    void drawInputPanel(sf::RenderWindow& window);

    void drawAllRegions(sf::RenderWindow& window);
    void drawSparseMatrix(sf::RenderWindow& window);
    void drawSparseOperations(sf::RenderWindow& window);
    void drawAnalysis(sf::RenderWindow& window);
    void drawManageRegion(sf::RenderWindow& window);
    void drawRange(sf::RenderWindow& window);
    void drawNearest(sf::RenderWindow& window);
    void drawTopRisk(sf::RenderWindow& window);
    void drawTrend(sf::RenderWindow& window);
    void drawKDTree(sf::RenderWindow& window);

public:
    DashboardUI();
    bool loadFont(const string& path);

    void attachKDTree(KDTree* tree);
    void attachSparseMatrix(SparseMatrix* sparse,
                            const vector<string>& regionNames,
                            int totalDays);

    void setBackendData(const vector<RegionPoint>& realRegions,
                        const vector<RegionTimeline>& allTimelines);

    void handleEvent(sf::RenderWindow& window, const sf::Event& event, bool& dirty);
    void update(bool& dirty);
    void draw(sf::RenderWindow& window);
};

#endif
