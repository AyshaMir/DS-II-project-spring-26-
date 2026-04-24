#include <iostream>
#include "heap.h"

using namespace std;

int main() {
    // Insert sample regions
    insert("Pakistan", 4.77, 4.91);
    insert("India", 15.86, 5.00);
    insert("US", 82.10, 303.03);
    insert("Brazil", 40.00, 15.00);

    cout << " HEAP CONTENTS " << endl;
    display();

    cout << "\n SHOW MAX " << endl;
    showMax();

    cout << "\n HEAP SIZE " << endl;
    cout << "Size: " << size() << endl;

    cout << "\n TOP 3 REGIONS " << endl;
    showTopN(3);

    cout << "\n EXTRACT MAX " << endl;
    Region maxRegion = extractMax();
    cout << "Extracted: "
         << maxRegion.region
         << " | Growth: " << maxRegion.growth
         << " | CPM: " << maxRegion.cpm << endl;

    cout << "\n HEAP AFTER EXTRACTION " << endl;
    display();

    cout << "\n EMPTY CHECK " << endl;
    cout << (isEmpty() ? "Heap is empty." : "Heap is not empty.") << endl;

    return 0;
}