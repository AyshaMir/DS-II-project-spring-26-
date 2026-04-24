#include "dataloader.h"
#include <fstream>
#include <sstream>
#include <iostream>

vector<Entry> DataLoader::loadCSV(const string& filename) {
    vector<Entry> data;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return data;
    }

    string line;
    
    // Skip header
    getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string temp;
        Entry entry;
        getline(ss, entry.region, ',');
    
        getline(ss, temp, ',');
        entry.day = stoi(temp);

        getline(ss, temp, ',');
        entry.cases = stoi(temp);

        getline(ss, temp, ',');
        entry.population = stoi(temp);

        data.push_back(entry);
    }

    file.close();
    return data;
}