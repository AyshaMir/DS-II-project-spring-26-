#include "DashboardData.h"
#include <algorithm>
#include <cctype>

sf::Color statusColor(const string& status) {
    if (status == "SAFE") return sf::Color(52, 168, 100);
    if (status == "OUTBREAK") return sf::Color(196, 50, 65);
    return sf::Color(215, 155, 35);
}

string lowerText(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return (char)tolower(c);
    });
    return value;
}

string classifyVisualStatus(float growth, float cpm) {
    if ((growth > 30.0f) || (cpm > 0.5f)) return "OUTBREAK";
    if ((growth > 15.0f) || (cpm > 0.1f)) return "WARNING";
    return "SAFE";
}
