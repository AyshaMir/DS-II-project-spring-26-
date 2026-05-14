#include "ui.h"
#include "GraphRenderer.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>

using namespace std;

static const sf::Color COL_BG(240, 234, 220);
static const sf::Color COL_CANVAS(250, 246, 240);
static const sf::Color COL_SIDEBAR(18, 32, 65);
static const sf::Color COL_SIDEBAR_HOVER(28, 50, 98);
static const sf::Color COL_SIDEBAR_ACTIVE(196, 50, 65);
static const sf::Color COL_CRIMSON(196, 50, 65);
static const sf::Color COL_CRIMSON_LIGHT(220, 78, 90);
static const sf::Color COL_PANEL(228, 222, 208);
static const sf::Color COL_PANEL_DARK(212, 205, 190);
static const sf::Color COL_BORDER(185, 177, 160);
static const sf::Color COL_INPUT_BG(255, 252, 247);
static const sf::Color COL_INPUT_ACTIVE(255, 255, 255);
static const sf::Color COL_TEXT_DARK(28, 28, 45);
static const sf::Color COL_TEXT_LIGHT(242, 237, 225);
static const sf::Color COL_TEXT_MID(115, 108, 95);
static const sf::Color COL_NAVY(42, 68, 120);
static const sf::Color COL_SAFE(52, 168, 100);
static const sf::Color COL_WARNING(215, 155, 35);
static const sf::Color COL_OUTBREAK(196, 50, 65);

DashboardUI::DashboardUI() {
    currentScreen = SCR_ALL;
    outputText = "Load a dataset to begin.";
    focusedBox = -1;
    cursorOn = true;
    runHover = false;

    backendTree = nullptr;
    backendSparse = nullptr;
    backendTotalDays = 0;

    searchIndex = -1;
    nearestTarget = {-1.0f, -1.0f};

    minGrowth = 10.0f;
    maxGrowth = 80.0f;
    minCpm = 0.0f;
    maxCpm = 5.0f;
    targetGrowth = 20.0f;
    targetCpm = 10.0f;
    kValue = 5;
    topRiskGraphMode = GRAPH_SCATTER;
}

bool DashboardUI::loadFont(const string& path) {
    if (!font.loadFromFile(path)) {
        return false;
    }

    createSidebar();
    createInputs();
    createRunButton();
    initOutput(SCR_ALL);
    return true;
}

bool regionExists(const vector<string>& regions, const string& name) {
    for (const string& r : regions) {
        if (r == name) return true;
    }
    return false;
}

void DashboardUI::attachKDTree(KDTree* tree) {
    backendTree = tree;
}

void DashboardUI::attachSparseMatrix(SparseMatrix* sparse,
                                     const vector<string>& regionNames,
                                     int totalDays) {
    backendSparse = sparse;
    backendRegionNames = regionNames;
    backendTotalDays = totalDays;
}

void DashboardUI::setBackendData(const vector<RegionPoint>& realRegions,
                                 const vector<RegionTimeline>& allTimelines) {
    regions = realRegions;
    timelines = allTimelines;

    selectedTrend.clear();
    selectedTrendName = "";

    searchIndex = -1;
    rangeHighlight.clear();
    nearestHighlight.clear();
    topRiskIndexes.clear();

    initOutput(currentScreen);
}

void DashboardUI::createSidebar() {
    const string labels[SCR_COUNT] = {
        "All Regions",
        "Sparse Matrix",
        "Sparse Operations",
        "Analysis Report",
        "Manage Region",
        "Range Search",
        "Nearest Region",
        "Top Risk",
        "Trend Graph",
        "KD-Tree View"
    };

    sidebar.clear();

    for (int i = 0; i < SCR_COUNT; i++) {
        Button button;
        button.rect.setSize({(float)SIDEBAR_W, 52.0f});
        button.rect.setPosition(0.0f, 78.0f + i * 56.0f);

        button.text.setFont(font);
        button.text.setCharacterSize(12);
        button.text.setString(labels[i]);
        button.text.setPosition(18.0f, 95.0f + i * 56.0f);

        button.target = (Screen)i;
        button.hover = false;
        button.active = (i == 0);

        sidebar.push_back(button);
    }
}

void DashboardUI::createInputs() {
    const string labels[8] = {
        "Region Name",
        "Min Growth % / Day",
        "Max Growth %",
        "Min CPM / New Cases",
        "Max CPM",
        "Target Growth %",
        "Target CPM",
        "K Value"
    };

    const string hints[8] = {
        "e.g. Pakistan",
        "growth or day",
        "e.g. 80",
        "cpm or cases",
        "e.g. 5",
        "e.g. 20",
        "e.g. 10",
        "e.g. 5"
    };

    inputs.clear();

    for (int i = 0; i < 8; i++) {
        InputBox box;
        box.rect.setSize({250.0f, 34.0f});
        box.rect.setPosition((float)INPUT_X + 24.0f, 78.0f + i * 72.0f);
        box.rect.setFillColor(COL_INPUT_BG);
        box.rect.setOutlineColor(COL_BORDER);
        box.rect.setOutlineThickness(1.0f);

        box.label.setFont(font);
        box.label.setCharacterSize(11);
        box.label.setFillColor(COL_TEXT_MID);
        box.label.setString(labels[i]);
        box.label.setPosition((float)INPUT_X + 24.0f, 58.0f + i * 72.0f);

        box.display.setFont(font);
        box.display.setCharacterSize(12);
        box.display.setFillColor(COL_TEXT_DARK);
        box.display.setPosition((float)INPUT_X + 34.0f, 86.0f + i * 72.0f);

        box.value = "";
        box.hint = hints[i];

        inputs.push_back(box);
    }
}

void DashboardUI::createRunButton() {
    runButton.setSize({250.0f, 44.0f});
    runButton.setPosition((float)INPUT_X + 24.0f, 665.0f);
    runButton.setFillColor(COL_CRIMSON);

    runText.setFont(font);
    runText.setCharacterSize(15);
    runText.setFillColor(COL_TEXT_LIGHT);
    runText.setString("Run");
    runText.setPosition((float)INPUT_X + 130.0f, 676.0f);
}

vector<int> DashboardUI::activeInputs() const {
    switch (currentScreen) {
        case SCR_SPARSE_OPS:
            return {0, 1, 3};
        case SCR_SEARCH:
            return {0, 1, 3};
        case SCR_RANGE:
            return {1, 2, 3, 4};
        case SCR_NEAREST:
            return {5, 6, 7};
        case SCR_TOPRISK:
            return {7};
        case SCR_TREND:
            return {0};
        default:
            return {};
    }
}

void DashboardUI::setOutput(const string& text) {
    outputText = text;
}

float DashboardUI::toFloat(const string& value, float fallback) const {
    try {
        if (value.empty()) return fallback;
        return stof(value);
    } catch (...) {
        return fallback;
    }
}

int DashboardUI::toInt(const string& value, int fallback) const {
    try {
        if (value.empty()) return fallback;
        return stoi(value);
    } catch (...) {
        return fallback;
    }
}

void DashboardUI::initOutput(Screen screen) {
    switch (screen) {
        case SCR_ALL:
            setOutput("Showing all loaded regions as points.\nX-axis = Growth Rate, Y-axis = Cases Per Million.\nAxes automatically scale to the loaded dataset range.\nColors: green = SAFE, orange = WARNING, red = OUTBREAK.");
            break;

        case SCR_SPARSE:
            setOutput("Sparse Matrix stores only non-zero case entries.\nThis screen shows all region timelines from the loaded dataset.\nLight cells are zero; darker cells are stored non-zero values.");
            break;

        case SCR_SPARSE_OPS:
            setOutput("Sparse Matrix Operations use the actual backend SparseMatrix.\nRegion Name = region, Min Growth/Day = day, Min CPM/New Cases = cases.\nRun = search/get, I = insert, U = update, D = delete.");
            break;

        case SCR_ANALYSIS:
            setOutput("Pipeline: CSV -> Sparse Matrix -> Analyzer -> Decision Tree -> KD-tree.\nThe report summarizes region statuses and risk levels.");
            break;

        case SCR_SEARCH:
            setOutput("Manage Region using the actual backend KD-tree.\nRegion Name = region.\nMin Growth/Day = new growth for update.\nMin CPM/New Cases = new CPM for update.\nRun = search, U = update, D = delete.");
            break;

        case SCR_TREND:
            setOutput("Type a region name and click Run to view its 30-day case trend.\nThis timeline comes from SparseMatrix.extractRegion().");
            break;

        case SCR_KDTREE:
            setOutput("KD-tree stores every region as a 2D point: Growth Rate and Cases Per Million.\nThis view shows the actual backend KD-tree links.");
            break;

        case SCR_TOPRISK:
            computeOutput();
            break;

        default:
            setOutput("Enter the required input values and click Run.");
            break;
    }
}

void DashboardUI::computeOutput() {
    ostringstream oss;
    oss << fixed << setprecision(2);

    if (currentScreen == SCR_SPARSE_OPS) {
        if (backendSparse == nullptr) {
            setOutput("Backend SparseMatrix is not connected.");
            return;
        }

        string regionName = inputs[0].value;
        int day = toInt(inputs[1].value, -1);

        if (regionName.empty() || day <= 0) {
            setOutput("Enter Region Name and Day first.");
            return;
        }

        if (!regionExists(backendRegionNames, regionName)) {
            setOutput("Region not found. Region name must match the dataset exactly.");
            return;
        }

        bool found = backendSparse->search(regionName, day);
        int value = backendSparse->get(regionName, day);

        oss << "SparseMatrix Search/Get Result:\n"
            << "Region: " << regionName << "\n"
            << "Day: " << day << "\n"
            << "Found as stored non-zero entry: " << (found ? "Yes" : "No") << "\n"
            << "Cases returned by get(): " << value << "\n\n"
            << "I = insert, U = update, D = delete using New Cases input.";

        setOutput(oss.str());
        return;
    }

    if (regions.empty()) {
        setOutput("No backend data loaded yet.");
        return;
    }

    if (currentScreen == SCR_SEARCH) {
        string query = inputs[0].value;
        searchIndex = -1;

        for (int i = 0; i < (int)regions.size(); i++) {
            if (regions[i].name == query) {
                searchIndex = i;
                break;
            }
        }

        if (searchIndex == -1) {
            setOutput("Region not found. Type an exact region name from the dataset.");
            return;
        }

        const RegionPoint& r = regions[searchIndex];

        oss << "KD-tree Search Result:\n"
            << "Region: " << r.name << "\n"
            << "Growth: " << r.growth << "%\n"
            << "CPM: " << r.cpm << "\n"
            << "Status: " << r.status << "\n\n"
            << "U = update this region in backend KD-tree.\n"
            << "D = delete this region from backend KD-tree.";

        setOutput(oss.str());
        return;
    }

    if (currentScreen == SCR_RANGE) {
        minGrowth = toFloat(inputs[1].value, 10.0f);
        maxGrowth = toFloat(inputs[2].value, 80.0f);
        minCpm = toFloat(inputs[3].value, 0.0f);
        maxCpm = toFloat(inputs[4].value, 5.0f);

        rangeHighlight.assign(regions.size(), false);
        int count = 0;

        oss << "Range Search Result:\nGrowth: " << minGrowth << " to " << maxGrowth
            << " | CPM: " << minCpm << " to " << maxCpm << "\n\n";

        for (int i = 0; i < (int)regions.size(); i++) {
            const RegionPoint& r = regions[i];

            if (r.growth >= minGrowth && r.growth <= maxGrowth &&
                r.cpm >= minCpm && r.cpm <= maxCpm) {
                rangeHighlight[i] = true;
                count++;
                oss << count << ". " << r.name
                    << " | Growth: " << r.growth
                    << "% | CPM: " << r.cpm << "\n";
            }
        }

        if (count == 0) oss << "No regions found in this range.";

        setOutput(oss.str());
        return;
    }

    if (currentScreen == SCR_NEAREST) {
        targetGrowth = toFloat(inputs[5].value, 20.0f);
        targetCpm = toFloat(inputs[6].value, 10.0f);
        kValue = toInt(inputs[7].value, 3);
        kValue = max(1, min(kValue, (int)regions.size()));

        vector<pair<float, int>> distances;

        for (int i = 0; i < (int)regions.size(); i++) {
            float dx = regions[i].growth - targetGrowth;
            float dy = regions[i].cpm - targetCpm;
            distances.push_back({dx * dx + dy * dy, i});
        }

        sort(distances.begin(), distances.end());
        nearestHighlight.assign(regions.size(), false);
        nearestTarget = {targetGrowth, targetCpm};

        oss << "Nearest Region Query:\nTarget: (" << targetGrowth << ", " << targetCpm << ")\n\n";

        for (int i = 0; i < kValue; i++) {
            int idx = distances[i].second;
            nearestHighlight[idx] = true;
            const RegionPoint& r = regions[idx];

            oss << i + 1 << ". " << r.name
                << " | Growth: " << r.growth
                << "% | CPM: " << r.cpm << "\n";
        }

        setOutput(oss.str());
        return;
    }

    if (currentScreen == SCR_TOPRISK) {
        kValue = toInt(inputs[7].value, 5);
        kValue = max(1, min(kValue, (int)regions.size()));

        topRiskIndexes.resize(regions.size());
        iota(topRiskIndexes.begin(), topRiskIndexes.end(), 0);

        sort(topRiskIndexes.begin(), topRiskIndexes.end(), [&](int a, int b) {
            return regions[a].growth > regions[b].growth;
        });

        topRiskIndexes.resize(kValue);

        oss << "Top " << kValue << " High-Risk Regions by Growth Rate:\n\n";

        for (int i = 0; i < kValue; i++) {
            const RegionPoint& r = regions[topRiskIndexes[i]];
            oss << i + 1 << ". " << r.name
                << " | Growth: " << r.growth
                << "% | CPM: " << r.cpm
                << " | " << r.status << "\n";
        }

        oss << "\nPress B for bar view, S for scatter view.";

        setOutput(oss.str());
        return;
    }

    if (currentScreen == SCR_TREND) {
        if (backendSparse == nullptr) {
            setOutput("Backend SparseMatrix is not connected.");
            return;
        }

        string regionName = inputs[0].value;

        if (regionName.empty()) {
            setOutput("Enter a region name to show its trend.");
            return;
        }

        if (!regionExists(backendRegionNames, regionName)) {
            selectedTrend.clear();
            selectedTrendName = "";
            setOutput("Region not found. Region name must match the dataset exactly.");
            return;
        }

        vector<int> cases = backendSparse->extractRegion(regionName, backendTotalDays);

        selectedTrend.clear();

        for (int i = 0; i < (int)cases.size(); i++) {
            selectedTrend.push_back({i + 1, cases[i]});
        }

        selectedTrendName = regionName;

        oss << "Trend loaded for region: " << regionName
            << "\nDays shown: " << backendTotalDays
            << "\nSource: SparseMatrix.extractRegion()";

        setOutput(oss.str());
        return;
    }

    initOutput(currentScreen);
}

vector<RegionPoint> DashboardUI::currentTopRiskRegions() const {
    vector<RegionPoint> result;

    for (int idx : topRiskIndexes) {
        if (idx >= 0 && idx < (int)regions.size()) {
            result.push_back(regions[idx]);
        }
    }

    return result;
}

void DashboardUI::rebuildRegionsFromKDTree() {
    if (backendTree == nullptr) return;

    vector<KDNode*> nodes = backendTree->getAllNodes();

    regions.clear();

    for (KDNode* node : nodes) {
        regions.push_back({
            node->region,
            node->growth,
            node->cpm,
            classifyVisualStatus(node->growth, node->cpm)
        });
    }
}

void DashboardUI::rebuildTimelinesFromSparse() {
    if (backendSparse == nullptr) return;

    timelines.clear();

    for (const string& region : backendRegionNames) {
        RegionTimeline t;
        t.name = region;
        t.cases = backendSparse->extractRegion(region, backendTotalDays);
        timelines.push_back(t);
    }
}

void DashboardUI::updateSelectedRegion() {
    if (backendTree == nullptr) {
        setOutput("Backend KD-tree is not connected.");
        return;
    }

    string regionName = inputs[0].value;

    if (regionName.empty()) {
        setOutput("Enter a region name before updating.");
        return;
    }

    float newGrowth = toFloat(inputs[1].value, -1.0f);
    float newCpm = toFloat(inputs[3].value, -1.0f);

    if (newGrowth < 0.0f || newCpm < 0.0f) {
        setOutput("For KD update: enter Region Name, new Growth, and new CPM.");
        return;
    }

    bool updated = backendTree->updateRegion(regionName, newGrowth, newCpm);

    if (!updated) {
        setOutput("Update failed. Region not found in backend KD-tree.");
        return;
    }

    rebuildRegionsFromKDTree();

    searchIndex = -1;
    rangeHighlight.clear();
    nearestHighlight.clear();
    topRiskIndexes.clear();

    ostringstream oss;
    oss << fixed << setprecision(2);
    oss << "Backend KD-tree updated successfully.\n"
        << "Region: " << regionName << "\n"
        << "New Growth: " << newGrowth << "%\n"
        << "New CPM: " << newCpm;

    setOutput(oss.str());
}

void DashboardUI::deleteSelectedRegion() {
    if (backendTree == nullptr) {
        setOutput("Backend KD-tree is not connected.");
        return;
    }

    string regionName = inputs[0].value;

    if (regionName.empty()) {
        setOutput("Enter a region name before deleting.");
        return;
    }

    KDNode* found = backendTree->searchRegion(regionName);

    if (found == nullptr) {
        setOutput("Delete failed. Region not found in backend KD-tree.");
        return;
    }

    backendTree->deleteNode(regionName);
    rebuildRegionsFromKDTree();

    searchIndex = -1;
    rangeHighlight.clear();
    nearestHighlight.clear();
    topRiskIndexes.clear();

    setOutput("Region deleted from actual backend KD-tree: " + regionName);
}

void DashboardUI::insertSparseEntry() {
    if (backendSparse == nullptr) {
        setOutput("Backend SparseMatrix is not connected.");
        return;
    }

    string regionName = inputs[0].value;
    int day = toInt(inputs[1].value, -1);
    int cases = toInt(inputs[3].value, -1);

    if (regionName.empty() || day <= 0 || cases < 0) {
        setOutput("For sparse insert: enter Region Name, Day, and New Cases.");
        return;
    }

    backendSparse->insert(regionName, day, cases);
    rebuildTimelinesFromSparse();

    setOutput("Inserted into backend SparseMatrix: " + regionName +
              ", Day " + to_string(day) +
              ", Cases " + to_string(cases));
}

void DashboardUI::updateSparseEntry() {
    if (backendSparse == nullptr) {
        setOutput("Backend SparseMatrix is not connected.");
        return;
    }

    string regionName = inputs[0].value;
    int day = toInt(inputs[1].value, -1);
    int cases = toInt(inputs[3].value, -1);

    if (regionName.empty() || day <= 0 || cases < 0) {
        setOutput("For sparse update: enter Region Name, Day, and New Cases.");
        return;
    }

    bool updated = backendSparse->update(regionName, day, cases);

    if (!updated && cases > 0) {
        backendSparse->insert(regionName, day, cases);
    }

    rebuildTimelinesFromSparse();

    setOutput("Updated backend SparseMatrix: " + regionName +
              ", Day " + to_string(day) +
              ", Cases " + to_string(cases));
}

void DashboardUI::deleteSparseEntry() {
    if (backendSparse == nullptr) {
        setOutput("Backend SparseMatrix is not connected.");
        return;
    }

    string regionName = inputs[0].value;
    int day = toInt(inputs[1].value, -1);

    if (regionName.empty() || day <= 0) {
        setOutput("For sparse delete: enter Region Name and Day.");
        return;
    }

    bool removed = backendSparse->remove(regionName, day);
    rebuildTimelinesFromSparse();

    setOutput(removed
        ? "Deleted non-zero entry from backend SparseMatrix: " + regionName + ", Day " + to_string(day)
        : "No stored non-zero entry found to delete for " + regionName + ", Day " + to_string(day));
}

void DashboardUI::drawOutputPanel(sf::RenderWindow& window) {
    GraphRenderer::drawRect(window, (float)CANVAS_X, (float)OUTPUT_Y,
                            (float)CANVAS_W, (float)OUTPUT_H, COL_PANEL);

    GraphRenderer::drawRect(window, (float)CANVAS_X, (float)OUTPUT_Y,
                            (float)CANVAS_W, 3.0f, COL_CRIMSON);

    GraphRenderer::drawText(window, font, "OUTPUT",
                            (float)CANVAS_X + 16.0f,
                            (float)OUTPUT_Y + 10.0f,
                            12,
                            COL_TEXT_MID);

    float y = (float)OUTPUT_Y + 38.0f;

    stringstream ss(outputText);
    string line;

    while (getline(ss, line) && y < WIN_H - 15.0f) {
        GraphRenderer::drawText(window, font, line,
                                (float)CANVAS_X + 16.0f,
                                y,
                                13,
                                COL_TEXT_DARK);
        y += 19.0f;
    }
}

void DashboardUI::drawSidebar(sf::RenderWindow& window) {
    GraphRenderer::drawRect(window, 0.0f, 0.0f,
                            (float)SIDEBAR_W, (float)WIN_H, COL_SIDEBAR);

    GraphRenderer::drawRect(window, 0.0f, 0.0f,
                            (float)SIDEBAR_W, 72.0f, sf::Color(12, 22, 50));

    GraphRenderer::drawText(window, font, "EPIDEMIC",
                            16.0f, 10.0f, 14, COL_CRIMSON);

    GraphRenderer::drawText(window, font, "OUTBREAK",
                            16.0f, 30.0f, 14, COL_TEXT_LIGHT);

    GraphRenderer::drawText(window, font, "DETECTOR v1.0",
                            16.0f, 52.0f, 11, sf::Color(145, 138, 124));

    GraphRenderer::drawRect(window, (float)SIDEBAR_W - 1.0f, 0.0f,
                            3.0f, (float)WIN_H, COL_CRIMSON);

    for (auto& button : sidebar) {
        sf::Color fill = sf::Color::Transparent;

        if (button.active) fill = COL_SIDEBAR_ACTIVE;
        else if (button.hover) fill = COL_SIDEBAR_HOVER;

        button.rect.setFillColor(fill);
        window.draw(button.rect);

        button.text.setFillColor(button.active ? COL_TEXT_LIGHT : sf::Color(170, 162, 148));
        window.draw(button.text);
    }
}

void DashboardUI::drawInputPanel(sf::RenderWindow& window) {
    GraphRenderer::drawRect(window, (float)INPUT_X, 0.0f,
                            (float)INPUT_W, (float)WIN_H, COL_PANEL);

    GraphRenderer::drawRect(window, (float)INPUT_X, 0.0f,
                            2.0f, (float)WIN_H, COL_BORDER);

    GraphRenderer::drawRect(window, (float)INPUT_X, 0.0f,
                            (float)INPUT_W, 46.0f, COL_PANEL_DARK);

    GraphRenderer::drawText(window, font, "INPUT PARAMETERS",
                            (float)INPUT_X + 20.0f,
                            15.0f,
                            12,
                            COL_TEXT_MID);

    vector<int> active = activeInputs();

    for (int i = 0; i < (int)inputs.size(); i++) {
        bool enabled = find(active.begin(), active.end(), i) != active.end();

        InputBox& box = inputs[i];

        box.rect.setFillColor(!enabled ? sf::Color(215, 208, 195)
                                       : (i == focusedBox ? COL_INPUT_ACTIVE : COL_INPUT_BG));

        box.rect.setOutlineColor(i == focusedBox ? COL_CRIMSON : COL_BORDER);
        box.rect.setOutlineThickness(i == focusedBox ? 1.8f : 1.0f);

        window.draw(box.rect);
        window.draw(box.label);

        if (!enabled) {
            box.display.setString("-");
            box.display.setFillColor(sf::Color(175, 167, 152));
        } else if (box.value.empty()) {
            box.display.setString(box.hint);
            box.display.setFillColor(sf::Color(182, 174, 160));
        } else {
            box.display.setString(box.value);
            box.display.setFillColor(COL_TEXT_DARK);
        }

        window.draw(box.display);

        if (i == focusedBox && cursorOn) {
            float cx = box.display.getPosition().x + box.display.getLocalBounds().width + 2.0f;
            GraphRenderer::drawRect(window, cx, box.display.getPosition().y,
                                    1.5f, 18.0f, COL_CRIMSON);
        }
    }

    runButton.setFillColor(runHover ? COL_CRIMSON_LIGHT : COL_CRIMSON);
    window.draw(runButton);
    window.draw(runText);
}

void DashboardUI::drawAllRegions(sf::RenderWindow& window) {
    GraphRenderer::drawScatterPlot(window, font, regions, {}, false,
                                   "All Regions - Growth Rate vs Cases Per Million");
}

void DashboardUI::drawSparseMatrix(sf::RenderWindow& window) {
    GraphRenderer::drawSparseHeatmap(window, font, timelines,
                                     "Sparse Matrix - All Region Timelines");
}

void DashboardUI::drawSparseOperations(sf::RenderWindow& window) {
    GraphRenderer::drawSparseHeatmap(window, font, timelines,
                                     "Sparse Matrix Operations - Backend View");
}

void DashboardUI::drawAnalysis(sf::RenderWindow& window) {
    GraphRenderer::drawCanvasTitle(window, font,
                                   "Analysis Report - Epidemic Status Summary");

    int safe = 0;
    int warning = 0;
    int outbreak = 0;
    float totalGrowth = 0.0f;

    RegionPoint highest = regions.empty()
        ? RegionPoint{"None", 0.0f, 0.0f, "SAFE"}
        : regions[0];

    for (const auto& r : regions) {
        if (r.status == "SAFE") safe++;
        else if (r.status == "OUTBREAK") outbreak++;
        else warning++;

        totalGrowth += r.growth;

        if (r.growth > highest.growth) highest = r;
    }

    float avgGrowth = regions.empty() ? 0.0f : totalGrowth / regions.size();

    ostringstream avg;
    avg << fixed << setprecision(2) << avgGrowth;

    vector<pair<string, sf::Color>> lines = {
        {"EPIDEMIC OUTBREAK DETECTION REPORT", COL_CRIMSON},
        {"", COL_TEXT_DARK},
        {"SUMMARY STATISTICS", COL_NAVY},
        {"Total Regions Monitored: " + to_string(regions.size()), COL_TEXT_DARK},
        {"Average Growth Rate: " + avg.str() + "%", COL_TEXT_DARK},
        {"Highest Growth Region: " + highest.name, COL_TEXT_DARK},
        {"", COL_TEXT_DARK},
        {"CLASSIFICATION BREAKDOWN", COL_NAVY},
        {"SAFE: " + to_string(safe) + " regions", COL_SAFE},
        {"WARNING: " + to_string(warning) + " regions", COL_WARNING},
        {"OUTBREAK: " + to_string(outbreak) + " regions", COL_OUTBREAK},
        {"", COL_TEXT_DARK},
        {"DATA STRUCTURE FLOW", COL_NAVY},
        {"Sparse Matrix stores non-zero case entries only.", COL_TEXT_DARK},
        {"Analyzer calculates growth, CPM, and trend.", COL_TEXT_DARK},
        {"Decision Tree classifies each region.", COL_TEXT_DARK},
        {"KD-tree stores analyzed points for search/range/nearest/top-K queries.", COL_TEXT_DARK}
    };

    float y = 55.0f;

    for (auto& line : lines) {
        if (line.first.empty()) {
            y += 12.0f;
            continue;
        }

        GraphRenderer::drawText(window, font, line.first,
                                (float)CANVAS_X + 34.0f,
                                y,
                                14,
                                line.second);

        y += 28.0f;
    }
}

void DashboardUI::drawManageRegion(sf::RenderWindow& window) {
    vector<bool> hi;

    if (searchIndex >= 0) {
        hi.assign(regions.size(), false);
        hi[searchIndex] = true;
    }

    GraphRenderer::drawScatterPlot(window, font, regions, hi,
                                   searchIndex >= 0,
                                   "Manage Region - Backend KD-tree Search/Update/Delete");
}

void DashboardUI::drawRange(sf::RenderWindow& window) {
    GraphRenderer::drawScatterPlot(window, font, regions, rangeHighlight,
                                   !rangeHighlight.empty(),
                                   "Range Search - Growth and CPM Bounds");
}

void DashboardUI::drawNearest(sf::RenderWindow& window) {
    GraphRenderer::drawScatterPlot(window, font, regions, nearestHighlight,
                                   !nearestHighlight.empty(),
                                   "Nearest Region - KD-tree Nearest Query",
                                   !nearestHighlight.empty(),
                                   nearestTarget);
}

void DashboardUI::drawTopRisk(sf::RenderWindow& window) {
    if (topRiskIndexes.empty()) computeOutput();

    vector<bool> hi(regions.size(), false);

    for (int idx : topRiskIndexes) {
        if (idx >= 0 && idx < (int)hi.size()) hi[idx] = true;
    }

    if (topRiskGraphMode == GRAPH_BAR) {
        GraphRenderer::drawTopRiskBars(window, font, currentTopRiskRegions(),
                                       "Top Risk Regions - Bar Graph View");
    } else {
        GraphRenderer::drawScatterPlot(window, font, regions, hi, true,
                                       "Top Risk Regions - Scatter View");
    }
}

void DashboardUI::drawTrend(sf::RenderWindow& window) {
    GraphRenderer::drawTrendGraph(window, font, selectedTrend,
                                  selectedTrendName,
                                  "Case Trend Graph - Daily Cases Over Time");
}

void DashboardUI::drawKDTree(sf::RenderWindow& window) {
    GraphRenderer::drawKDTreeView(
        window,
        font,
        backendTree == nullptr ? nullptr : backendTree->getRoot(),
        regions,
        "KD-tree View - Actual Backend Split Lines"
    );
}

void DashboardUI::handleEvent(sf::RenderWindow& window,
                              const sf::Event& event,
                              bool& dirty) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mouse((float)event.mouseMove.x, (float)event.mouseMove.y);

        for (auto& button : sidebar) {
            bool hoverNow = button.rect.getGlobalBounds().contains(mouse);

            if (hoverNow != button.hover) {
                button.hover = hoverNow;
                dirty = true;
            }
        }

        bool runHoverNow = runButton.getGlobalBounds().contains(mouse);

        if (runHoverNow != runHover) {
            runHover = runHoverNow;
            dirty = true;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse((float)event.mouseButton.x,
                           (float)event.mouseButton.y);

        for (auto& button : sidebar) {
            if (button.rect.getGlobalBounds().contains(mouse)) {
                currentScreen = button.target;

                for (auto& b : sidebar) b.active = false;
                button.active = true;

                focusedBox = -1;
                searchIndex = -1;
                rangeHighlight.clear();
                nearestHighlight.clear();
                topRiskIndexes.clear();

                initOutput(currentScreen);
                dirty = true;
            }
        }

        int oldFocus = focusedBox;
        focusedBox = -1;

        vector<int> active = activeInputs();

        for (int idx : active) {
            if (inputs[idx].rect.getGlobalBounds().contains(mouse)) {
                focusedBox = idx;
            }
        }

        if (focusedBox != oldFocus) dirty = true;

        if (runButton.getGlobalBounds().contains(mouse)) {
            computeOutput();
            dirty = true;
        }
    }

    if (event.type == sf::Event::TextEntered && focusedBox >= 0) {
        InputBox& box = inputs[focusedBox];

        if (event.text.unicode == 8) {
            if (!box.value.empty()) {
                box.value.pop_back();
                dirty = true;
            }
        } else if (event.text.unicode >= 32 &&
                   event.text.unicode < 127) {
            if (box.value.size() < 25) {
                box.value += (char)event.text.unicode;
                dirty = true;
            }
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        currentScreen == SCR_TOPRISK) {
        if (event.key.code == sf::Keyboard::B) {
            topRiskGraphMode = GRAPH_BAR;
            dirty = true;
        } else if (event.key.code == sf::Keyboard::S) {
            topRiskGraphMode = GRAPH_SCATTER;
            dirty = true;
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        currentScreen == SCR_SEARCH) {
        if (event.key.code == sf::Keyboard::U) {
            updateSelectedRegion();
            dirty = true;
        } else if (event.key.code == sf::Keyboard::D) {
            deleteSelectedRegion();
            dirty = true;
        }
    }

    if (event.type == sf::Event::KeyPressed &&
        currentScreen == SCR_SPARSE_OPS) {
        if (event.key.code == sf::Keyboard::I) {
            insertSparseEntry();
            dirty = true;
        } else if (event.key.code == sf::Keyboard::U) {
            updateSparseEntry();
            dirty = true;
        } else if (event.key.code == sf::Keyboard::D) {
            deleteSparseEntry();
            dirty = true;
        }
    }
}

void DashboardUI::update(bool& dirty) {
    if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
        cursorOn = !cursorOn;
        cursorClock.restart();

        if (focusedBox >= 0) dirty = true;
    }
}

void DashboardUI::draw(sf::RenderWindow& window) {
    GraphRenderer::drawRect(window, 0.0f, 0.0f,
                            (float)WIN_W, (float)WIN_H, COL_BG);

    GraphRenderer::drawRect(window, (float)CANVAS_X, 0.0f,
                            (float)CANVAS_W, (float)CANVAS_H, COL_CANVAS);

    switch (currentScreen) {
        case SCR_ALL:
            drawAllRegions(window);
            break;
        case SCR_SPARSE:
            drawSparseMatrix(window);
            break;
        case SCR_SPARSE_OPS:
            drawSparseOperations(window);
            break;
        case SCR_ANALYSIS:
            drawAnalysis(window);
            break;
        case SCR_SEARCH:
            drawManageRegion(window);
            break;
        case SCR_RANGE:
            drawRange(window);
            break;
        case SCR_NEAREST:
            drawNearest(window);
            break;
        case SCR_TOPRISK:
            drawTopRisk(window);
            break;
        case SCR_TREND:
            drawTrend(window);
            break;
        case SCR_KDTREE:
            drawKDTree(window);
            break;
        default:
            break;
    }

    drawOutputPanel(window);
    drawSidebar(window);
    drawInputPanel(window);
}
