#include <iostream>
#include "heap.h"

using namespace std;

int main() {
    // INSERT
    insert("Pakistan", 4.77, 4.91);
    insert("India", 15.86, 5.00);
    insert("US", 82.10, 303.03);
    insert("Brazil", 40.00, 15.00);

    cout << " HEAP CONTENTS AFTER INSERT " << endl;
    display();

    // SEARCH
    cout << "\n SEARCH FUNCTION " << endl;
    cout << "India: "
         << (searchRegion("India") ? "Found" : "Not Found") << endl;

    cout << "China: "
         << (searchRegion("China") ? "Found" : "Not Found") << endl;

    // SHOW MAX
    cout << "\n SHOW MAX " << endl;
    showMax();

    // UPDATE
    cout << "\n UPDATE FUNCTION " << endl;
    updateRegion("Pakistan", 90.00, 12.50);

    cout << "After updating Pakistan growth to 90:" << endl;
    display();

    cout << "\nNew max after update:" << endl;
    showMax();

    // DELETE
    cout << "\n DELETE FUNCTION " << endl;
    deleteRegion("Brazil");

    cout << "After deleting Brazil:" << endl;
    display();

    // TOP N
    cout << "\n TOP 3 REGIONS " << endl;
    showTopN(3);

    // EXTRACT MAX
    cout << "\n EXTRACT MAX " << endl;
    Region maxRegion = extractMax();

    cout << "Extracted: "
         << maxRegion.region
         << " | Growth: " << maxRegion.growth
         << " | CPM: " << maxRegion.cpm << endl;

    cout << "\n HEAP AFTER EXTRACTION " << endl;
    display();

    // SIZE / EMPTY CHECK
    cout << "\n HEAP SIZE " << endl;
    cout << "Size: " << size() << endl;

    cout << "\n EMPTY CHECK " << endl;
    cout << (isEmpty() ? "Heap is empty." : "Heap is not empty.") << endl;

    return 0;
}