#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

#include "DashboardData.h"
#include "kdtree.h"

using namespace std;

class GraphRenderer {
public:
    static void drawCanvasTitle(sf::RenderWindow& window, sf::Font& font, const string& title);

    static void drawScatterPlot(sf::RenderWindow& window, sf::Font& font,
                                const vector<RegionPoint>& regions,
                                const vector<bool>& highlight,
                                bool dimOthers,
                                const string& title,
                                bool showTarget = false,
                                sf::Vector2f target = {-1.0f, -1.0f});

    static void drawSparseHeatmap(sf::RenderWindow& window, sf::Font& font,
                                  const vector<RegionTimeline>& timelines,
                                  const string& title);

    static void drawTrendGraph(sf::RenderWindow& window, sf::Font& font,
                               const vector<TrendPoint>& trend,
                               const string& regionName,
                               const string& title);

    static void drawTopRiskBars(sf::RenderWindow& window, sf::Font& font,
                                const vector<RegionPoint>& topRegions,
                                const string& title);

    static void drawKDTreeView(sf::RenderWindow& window, sf::Font& font,
                               KDNode* root,
                               const vector<RegionPoint>& regions,
                               const string& title);

    // Public because KD-tree helper functions in GraphRenderer.cpp use them
    static sf::Vector2f mapToScreen(float x, float y,
                                    const GraphBounds& bounds,
                                    const GraphArea& area);

    static void drawRect(sf::RenderWindow& window, float x, float y, float w, float h,
                         sf::Color fill, sf::Color outline = sf::Color::Transparent,
                         float thickness = 0.0f);

    static void drawText(sf::RenderWindow& window, sf::Font& font, const string& text,
                         float x, float y, int size, sf::Color color,
                         float rotation = 0.0f);

    static void drawLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b,
                         sf::Color color, float thickness = 1.5f);

    static void drawDot(sf::RenderWindow& window, sf::Vector2f pos, sf::Color color,
                        float radius = 7.0f, bool outline = true);

private:
    static GraphArea plotArea();
    static GraphBounds calculateBounds(const vector<RegionPoint>& regions);

    static void drawAxes(sf::RenderWindow& window, sf::Font& font,
                         const GraphBounds& bounds, const GraphArea& area,
                         const string& xLabel, const string& yLabel);
};

#endif