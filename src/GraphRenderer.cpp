#include "GraphRenderer.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

static const sf::Color COL_CANVAS(250, 246, 240);
static const sf::Color COL_NAVY(42, 68, 120);
static const sf::Color COL_CRIMSON(196, 50, 65);
static const sf::Color COL_TEXT_DARK(28, 28, 45);
static const sf::Color COL_TEXT_LIGHT(242, 237, 225);
static const sf::Color COL_TEXT_MID(115, 108, 95);
static const sf::Color COL_BORDER(185, 177, 160);

void GraphRenderer::drawRect(sf::RenderWindow& window, float x, float y, float w, float h,
                             sf::Color fill, sf::Color outline, float thickness) {
    sf::RectangleShape rect({w, h});
    rect.setPosition(x, y);
    rect.setFillColor(fill);
    if (thickness > 0.0f) {
        rect.setOutlineColor(outline);
        rect.setOutlineThickness(thickness);
    }
    window.draw(rect);
}

void GraphRenderer::drawText(sf::RenderWindow& window, sf::Font& font, const string& text,
                             float x, float y, int size, sf::Color color,
                             float rotation) {
    sf::Text drawable;
    drawable.setFont(font);
    drawable.setString(text);
    drawable.setCharacterSize(size);
    drawable.setFillColor(color);
    drawable.setPosition(x, y);
    drawable.setRotation(rotation);
    window.draw(drawable);
}

void GraphRenderer::drawLine(sf::RenderWindow& window, sf::Vector2f a, sf::Vector2f b,
                             sf::Color color, float thickness) {
    float length = hypotf(b.x - a.x, b.y - a.y);
    if (length < 0.01f) return;

    float angle = atan2f(b.y - a.y, b.x - a.x) * 180.0f / 3.14159265f;
    sf::RectangleShape line({length, thickness});
    line.setPosition(a);
    line.setRotation(angle);
    line.setOrigin(0.0f, thickness / 2.0f);
    line.setFillColor(color);
    window.draw(line);
}

void GraphRenderer::drawDot(sf::RenderWindow& window, sf::Vector2f pos, sf::Color color,
                            float radius, bool outline) {
    sf::CircleShape dot(radius);
    dot.setOrigin(radius, radius);
    dot.setPosition(pos);
    dot.setFillColor(color);

    if (outline) {
        dot.setOutlineColor(sf::Color(255, 255, 255, 210));
        dot.setOutlineThickness(1.6f);
    }

    window.draw(dot);
}

void GraphRenderer::drawCanvasTitle(sf::RenderWindow& window, sf::Font& font,
                                    const string& title) {
    drawRect(window, (float)CANVAS_X, 0.0f, (float)CANVAS_W, 38.0f, COL_NAVY);
    drawText(window, font, title, (float)CANVAS_X + 18.0f, 10.0f, 14, COL_TEXT_LIGHT);
}

GraphArea GraphRenderer::plotArea() {
    return {
        (float)CANVAS_X + 76.0f,
        58.0f,
        (float)CANVAS_W - 120.0f,
        (float)CANVAS_H - 135.0f
    };
}

GraphBounds GraphRenderer::calculateBounds(const vector<RegionPoint>& regions) {
    if (regions.empty()) return {0, 1, 0, 1};

    float minX = regions[0].growth;
    float maxX = regions[0].growth;
    float minY = regions[0].cpm;
    float maxY = regions[0].cpm;

    for (const RegionPoint& r : regions) {
        minX = min(minX, r.growth);
        maxX = max(maxX, r.growth);
        minY = min(minY, r.cpm);
        maxY = max(maxY, r.cpm);
    }

    float rangeX = maxX - minX;
    float rangeY = maxY - minY;

    float padX = max(0.35f, rangeX * 0.04f);
    float padY = max(0.02f, rangeY * 0.05f);

    minX = max(0.0f, minX - padX);
    maxX = maxX + padX;
    minY = max(0.0f, minY - padY);
    maxY = maxY + padY;

    if (fabs(maxX - minX) < 1.0f) maxX = minX + 1.0f;
    if (fabs(maxY - minY) < 0.1f) maxY = minY + 0.1f;

    return {minX, maxX, minY, maxY};
}

sf::Vector2f GraphRenderer::mapToScreen(float x, float y, const GraphBounds& bounds, const GraphArea& area) {
    float px = area.x + ((x - bounds.minX) / (bounds.maxX - bounds.minX)) * area.w;

    float normalizedY = (y - bounds.minY) / (bounds.maxY - bounds.minY);
    normalizedY = max(0.0f, min(1.0f, normalizedY));

    // compressed scale so low CPM regions are more visible
    normalizedY = sqrt(normalizedY);

    float py = area.y + area.h - normalizedY * area.h;

    return {px, py};
}

void GraphRenderer::drawAxes(sf::RenderWindow& window, sf::Font& font,
                             const GraphBounds& bounds, const GraphArea& area,
                             const string& xLabel, const string& yLabel) {
    sf::Color grid(185, 177, 160, 90);

    drawLine(window, {area.x, area.y}, {area.x, area.y + area.h}, COL_TEXT_DARK, 2.0f);
    drawLine(window, {area.x, area.y + area.h}, {area.x + area.w, area.y + area.h}, COL_TEXT_DARK, 2.0f);

    int ticks = 5;

    for (int i = 0; i <= ticks; i++) {
        float xUnit = (float)i / ticks;
        float gx = area.x + area.w * xUnit;

        float xValue = bounds.minX + (bounds.maxX - bounds.minX) * xUnit;

        drawLine(window, {gx, area.y}, {gx, area.y + area.h}, grid, 1.0f);

        ostringstream xs;
        xs << fixed << setprecision(1) << xValue;
        drawText(window, font, xs.str(), gx - 16.0f, area.y + area.h + 10.0f, 10, COL_TEXT_MID);

        float yUnit = (float)i / ticks;
        float gy = area.y + area.h - yUnit * area.h;

        // inverse of sqrt scale
        float realY = bounds.minY + (yUnit * yUnit) * (bounds.maxY - bounds.minY);

        drawLine(window, {area.x, gy}, {area.x + area.w, gy}, grid, 1.0f);

        ostringstream ys;
        ys << fixed << setprecision(2) << realY;
        drawText(window, font, ys.str(), area.x - 58.0f, gy - 7.0f, 10, COL_TEXT_MID);
    }

    drawText(window, font, xLabel, area.x + area.w / 2.0f - 55.0f, area.y + area.h + 36.0f, 12, COL_TEXT_MID);
    drawText(window, font, yLabel, area.x - 60.0f, area.y + area.h / 2.0f + 70.0f, 12, COL_TEXT_MID, -90.0f);
}

void GraphRenderer::drawScatterPlot(sf::RenderWindow& window, sf::Font& font,
                                    const vector<RegionPoint>& regions,
                                    const vector<bool>& highlight,
                                    bool dimOthers,
                                    const string& title,
                                    bool showTarget,
                                    sf::Vector2f target) {
    drawCanvasTitle(window, font, title);

    if (regions.empty()) {
        drawText(window, font, "No region data loaded.",
                 (float)CANVAS_X + 40.0f, 80.0f, 16, COL_TEXT_DARK);
        return;
    }

    GraphArea area = plotArea();
    GraphBounds bounds = calculateBounds(regions);

    if (showTarget) {
        bounds.minX = min(bounds.minX, target.x);
        bounds.maxX = max(bounds.maxX, target.x);
        bounds.minY = min(bounds.minY, target.y);
        bounds.maxY = max(bounds.maxY, target.y);
    }

    drawAxes(window, font, bounds, area, "Growth Rate (%)", "Cases Per Million");

    for (int i = 0; i < (int)regions.size(); i++) {
        bool selected = (!highlight.empty() && i < (int)highlight.size() && highlight[i]);

        sf::Color color = statusColor(regions[i].status);

        if (dimOthers && !selected) {
            color = sf::Color(color.r, color.g, color.b, 55);
        }

        sf::Vector2f pos = mapToScreen(regions[i].growth, regions[i].cpm, bounds, area);

        if (selected) {
            drawDot(window, pos, sf::Color(color.r, color.g, color.b, 55), 13.0f, false);
        }

        drawDot(window, pos, color, selected ? 8.5f : 6.0f);

        if (selected || !dimOthers) {
            drawText(window, font, regions[i].name, pos.x + 9.0f, pos.y - 8.0f, 10,
                     selected ? COL_TEXT_DARK : COL_TEXT_MID);
        }
    }

    if (showTarget) {
        sf::Vector2f pos = mapToScreen(target.x, target.y, bounds, area);
        drawLine(window, {pos.x - 14.0f, pos.y}, {pos.x + 14.0f, pos.y}, COL_CRIMSON, 2.0f);
        drawLine(window, {pos.x, pos.y - 14.0f}, {pos.x, pos.y + 14.0f}, COL_CRIMSON, 2.0f);
        drawDot(window, pos, COL_CRIMSON, 5.0f);
        drawText(window, font, "Target", pos.x + 10.0f, pos.y - 22.0f, 11, COL_CRIMSON);
    }
}

void GraphRenderer::drawSparseHeatmap(sf::RenderWindow& window, sf::Font& font,
                                      const vector<RegionTimeline>& timelines,
                                      const string& title) {
    drawCanvasTitle(window, font, title);

    if (timelines.empty()) {
        drawText(window, font, "No timelines loaded.",
                 (float)CANVAS_X + 40.0f, 80.0f, 16, COL_TEXT_DARK);
        return;
    }

    int rows = (int)timelines.size();
    int maxDays = 0;
    int maxCases = 1;

    for (const auto& t : timelines) {
        maxDays = max(maxDays, (int)t.cases.size());
        for (int c : t.cases) maxCases = max(maxCases, c);
    }

    float startX = (float)CANVAS_X + 160.0f;
    float startY = 72.0f;
    float usableW = (float)CANVAS_W - 240.0f;
    float usableH = (float)CANVAS_H - 110.0f;
    float cellW = usableW / max(1, maxDays);
    float cellH = min(22.0f, usableH / max(1, rows));

    for (int d = 0; d < maxDays; d += 5) {
        drawText(window, font, "D" + to_string(d + 1),
                 startX + d * cellW, startY - 16.0f, 9, COL_TEXT_MID);
    }

    for (int r = 0; r < rows; r++) {
        const RegionTimeline& timeline = timelines[r];

        drawText(window, font, timeline.name,
                 (float)CANVAS_X + 18.0f,
                 startY + r * cellH + 4.0f,
                 9,
                 COL_TEXT_DARK);

        for (int d = 0; d < maxDays; d++) {
            int val = d < (int)timeline.cases.size() ? timeline.cases[d] : 0;
            float norm = maxCases == 0 ? 0.0f : (float)val / maxCases;

            sf::Color color = val == 0
                ? sf::Color(236, 230, 216)
                : sf::Color((sf::Uint8)(85 + 125 * norm),
                            (sf::Uint8)(55 + 15 * norm),
                            (sf::Uint8)(95 + 40 * norm));

            drawRect(window,
                     startX + d * cellW,
                     startY + r * cellH,
                     cellW - 1.2f,
                     cellH - 1.2f,
                     color,
                     COL_BORDER,
                     0.4f);
        }
    }
}

void GraphRenderer::drawTrendGraph(sf::RenderWindow& window, sf::Font& font,
                                   const vector<TrendPoint>& trend,
                                   const string& regionName,
                                   const string& title) {
    drawCanvasTitle(window, font, title);

    if (trend.empty()) {
        drawText(window, font, "Type a region name and click Run.",
                 (float)CANVAS_X + 40.0f, 80.0f, 16, COL_TEXT_DARK);
        return;
    }

    GraphArea area = plotArea();

    int maxDay = 1;
    int maxCases = 1;

    for (const auto& p : trend) {
        maxDay = max(maxDay, p.day);
        maxCases = max(maxCases, p.cases);
    }

    GraphBounds bounds = {1.0f, (float)maxDay, 0.0f, (float)maxCases * 1.15f};
    drawAxes(window, font, bounds, area, "Day", "Cases");

    sf::Color lineColor(42, 100, 180);

    for (int i = 1; i < (int)trend.size(); i++) {
        sf::Vector2f a = mapToScreen((float)trend[i - 1].day, (float)trend[i - 1].cases, bounds, area);
        sf::Vector2f b = mapToScreen((float)trend[i].day, (float)trend[i].cases, bounds, area);
        drawLine(window, a, b, lineColor, 2.5f);
    }

    for (const TrendPoint& p : trend) {
        drawDot(window, mapToScreen((float)p.day, (float)p.cases, bounds, area), lineColor, 4.0f);
    }

    drawText(window, font, "Region: " + regionName,
             area.x + area.w - 170.0f, area.y + 14.0f, 13, COL_TEXT_DARK);
}

void GraphRenderer::drawTopRiskBars(sf::RenderWindow& window, sf::Font& font,
                                    const vector<RegionPoint>& topRegions,
                                    const string& title) {
    drawCanvasTitle(window, font, title);

    if (topRegions.empty()) {
        drawText(window, font, "No top-risk data loaded.",
                 (float)CANVAS_X + 40.0f, 80.0f, 16, COL_TEXT_DARK);
        return;
    }

    float startX = (float)CANVAS_X + 180.0f;
    float startY = 78.0f;
    float maxBarW = (float)CANVAS_W - 280.0f;
    float barH = 30.0f;
    float gap = 16.0f;

    float maxGrowth = 1.0f;
    for (const auto& r : topRegions) maxGrowth = max(maxGrowth, r.growth);

    for (int i = 0; i < (int)topRegions.size(); i++) {
        const RegionPoint& r = topRegions[i];
        float w = (r.growth / maxGrowth) * maxBarW;
        float y = startY + i * (barH + gap);

        drawText(window, font, to_string(i + 1) + ". " + r.name,
                 (float)CANVAS_X + 28.0f, y + 6.0f, 12, COL_TEXT_DARK);

        drawRect(window, startX, y, w, barH, statusColor(r.status), COL_BORDER, 1.0f);

        ostringstream oss;
        oss << fixed << setprecision(2) << r.growth << "%";
        drawText(window, font, oss.str(), startX + w + 10.0f, y + 6.0f, 12, COL_TEXT_DARK);
    }
}

static sf::Color kdColorFromValues(float growth, float cpm) {
    if (growth > 30.0f || cpm > 0.5f) return sf::Color(196, 50, 65);
    if (growth > 15.0f || cpm > 0.1f) return sf::Color(215, 155, 35);
    return sf::Color(52, 168, 100);
}

static void drawActualKDTreeRecursive(sf::RenderWindow& window,
                                      sf::Font& font,
                                      KDNode* node,
                                      float x,
                                      float y,
                                      float xGap,
                                      int depth) {
    if (node == nullptr) return;

    float boxW = 116.0f;
    float boxH = 43.0f;

    sf::Color border = kdColorFromValues(node->growth, node->cpm);

    GraphRenderer::drawRect(window,
                            x - boxW / 2.0f,
                            y,
                            boxW,
                            boxH,
                            sf::Color(250, 246, 240),
                            border,
                            2.0f);

    GraphRenderer::drawText(window, font, node->region,
                            x - boxW / 2.0f + 7.0f,
                            y + 7.0f,
                            8,
                            sf::Color(28, 28, 45));

    string metric = "G:" + to_string((int)node->growth) +
                    " C:" + to_string((int)node->cpm);

    GraphRenderer::drawText(window, font, metric,
                            x - boxW / 2.0f + 7.0f,
                            y + 25.0f,
                            8,
                            sf::Color(115, 108, 95));

    float childY = y + 62.0f;
    float nextGap = max(26.0f, xGap * 0.55f);

    if (node->left != nullptr) {
        float childX = x - xGap;
        GraphRenderer::drawLine(window, {x, y + boxH}, {childX, childY},
                                sf::Color(95, 95, 95), 1.1f);
        drawActualKDTreeRecursive(window, font, node->left, childX, childY, nextGap, depth + 1);
    }

    if (node->right != nullptr) {
        float childX = x + xGap;
        GraphRenderer::drawLine(window, {x, y + boxH}, {childX, childY},
                                sf::Color(95, 95, 95), 1.1f);
        drawActualKDTreeRecursive(window, font, node->right, childX, childY, nextGap, depth + 1);
    }
}

static void drawKDTreeSplits(
    sf::RenderWindow& window,
    KDNode* node,
    int depth,
    const GraphBounds& bounds,
    const GraphArea& area,
    float minX,
    float maxX,
    float minY,
    float maxY
) {
    if (node == nullptr) return;

    int axis = depth % 2;

    if (axis == 0) {
        float splitX = node->growth;

        sf::Vector2f a = GraphRenderer::mapToScreen(splitX, minY, bounds, area);
        sf::Vector2f b = GraphRenderer::mapToScreen(splitX, maxY, bounds, area);

        GraphRenderer::drawLine(window, a, b, sf::Color(42, 68, 120, 160), 1.2f);

        drawKDTreeSplits(window, node->left, depth + 1, bounds, area, minX, splitX, minY, maxY);
        drawKDTreeSplits(window, node->right, depth + 1, bounds, area, splitX, maxX, minY, maxY);
    } else {
        float splitY = node->cpm;

        sf::Vector2f a = GraphRenderer::mapToScreen(minX, splitY, bounds, area);
        sf::Vector2f b = GraphRenderer::mapToScreen(maxX, splitY, bounds, area);

        GraphRenderer::drawLine(window, a, b, sf::Color(196, 50, 65, 150), 1.2f);

        drawKDTreeSplits(window, node->left, depth + 1, bounds, area, minX, maxX, minY, splitY);
        drawKDTreeSplits(window, node->right, depth + 1, bounds, area, minX, maxX, splitY, maxY);
    }
}

void GraphRenderer::drawKDTreeView(
    sf::RenderWindow& window,
    sf::Font& font,
    KDNode* root,
    const vector<RegionPoint>& regions,
    const string& title
) {
    drawCanvasTitle(window, font, title);

    if (root == nullptr || regions.empty()) {
        drawText(window, font, "KD-tree is empty.", (float)CANVAS_X + 40.0f, 80.0f, 16, COL_TEXT_DARK);
        return;
    }

    GraphArea area = plotArea();
    GraphBounds bounds = calculateBounds(regions);

    drawAxes(window, font, bounds, area, "Growth Rate (%)", "Cases Per Million");

    drawKDTreeSplits(
        window,
        root,
        0,
        bounds,
        area,
        bounds.minX,
        bounds.maxX,
        bounds.minY,
        bounds.maxY
    );

    for (const RegionPoint& r : regions) {
        sf::Color color = statusColor(r.status);
        sf::Vector2f pos = mapToScreen(r.growth, r.cpm, bounds, area);

        drawDot(window, pos, color, 6.5f);

        if (r.status == "OUTBREAK") {
            drawText(window, font, r.name, pos.x + 8.0f, pos.y - 8.0f, 10, COL_TEXT_DARK);
        }
    }

    drawText(
        window,
        font,
        "Actual backend KD-tree split view: blue = growth split, red = CPM split.",
        (float)CANVAS_X + 30.0f,
        (float)CANVAS_H - 38.0f,
        12,
        COL_TEXT_DARK
    );
}