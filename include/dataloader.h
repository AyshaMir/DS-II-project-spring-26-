#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <vector>
#include <string>

using namespace std;

struct Entry {
    string region;
    int day;
    int cases;
    int population;
};

class DataLoader {
public:
    vector<Entry> loadCSV(const string& filename);
};

#endif