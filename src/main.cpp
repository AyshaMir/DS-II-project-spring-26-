#include <iostream>
#include "dataloader.h"

int main() {
    DataLoader loader;
    vector<Entry> data = loader.loadCSV("data/dataset.csv");

    for (const auto& e : data) {
        cout << e.region << " Day " << e.day
             << " Cases: " << e.cases
             << " Population: " << e.population << endl;
    }

    return 0;
}