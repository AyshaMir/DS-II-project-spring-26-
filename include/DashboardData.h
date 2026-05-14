#ifndef DASHBOARD_DATA_H
#define DASHBOARD_DATA_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std;

const int WIN_W = 1600;
const int WIN_H = 900;
const int SIDEBAR_W = 220;
const int INPUT_W = 300;
const int CANVAS_X = SIDEBAR_W;
const int CANVAS_W = WIN_W - SIDEBAR_W - INPUT_W;
const int CANVAS_H = 620;
const int OUTPUT_Y = CANVAS_H;
const int OUTPUT_H = WIN_H - CANVAS_H;
const int INPUT_X = WIN_W - INPUT_W;

enum Screen {
    SCR_ALL = 0,
    SCR_SPARSE,
    SCR_SPARSE_OPS,
    SCR_ANALYSIS,
    SCR_SEARCH,
    SCR_RANGE,
    SCR_NEAREST,
    SCR_TOPRISK,
    SCR_TREND,
    SCR_KDTREE,
    SCR_COUNT
};

enum GraphMode {
    GRAPH_SCATTER = 0,
    GRAPH_LINE,
    GRAPH_BAR
};

struct RegionPoint {
    string name;
    float growth;
    float cpm;
    string status;
};

struct TrendPoint {
    int day;
    int cases;
};

struct RegionTimeline {
    string name;
    vector<int> cases;
};

struct Button {
    sf::RectangleShape rect;
    sf::Text text;
    Screen target;
    bool hover;
    bool active;
};

struct InputBox {
    sf::RectangleShape rect;
    sf::Text label;
    sf::Text display;
    string value;
    string hint;
};

struct GraphArea {
    float x;
    float y;
    float w;
    float h;
};

struct GraphBounds {
    float minX;
    float maxX;
    float minY;
    float maxY;
};

sf::Color statusColor(const string& status);
string classifyVisualStatus(float growth, float cpm);

#endif
