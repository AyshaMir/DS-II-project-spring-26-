# Epidemic Outbreak Detector

A C++ + SFML project made for monitoring rare disease outbreaks using data structures.

The project uses an Mpox dataset and analyzes regions using:

* Growth Rate
* Cases Per Million (CPM)
* Rising Trends

Regions are classified as:

* SAFE
* WARNING
* OUTBREAK

Main data structures used:

* Sparse Matrix
* KD-Tree

The project also includes an SFML dashboard for visualization and backend operations.

IMPORTANT:

* Region names are case-sensitive
* Region names must exactly match the dataset

Example:

* `Pakistan` ✅
* `pakistan` ❌

---

# Run Commands

## KD-Tree Main

```bash
g++ src/kdtree_main.cpp src/kdtree.cpp -Iinclude -o kdtree_app

./kdtree_app
```

## Sparse Matrix Main

```bash
g++ src/sparsematrix_main.cpp src/sparsematrix.cpp -Iinclude -o sparse_app

./sparse_app
```

## Backend Main

```bash
g++ src/main.cpp src/dataloader.cpp src/analyzer_decisiontree.cpp src/sparsematrix.cpp src/kdtree.cpp -Iinclude -o backend_app

./backend_app
```

## SFML UI

```bash
g++ src/sfml_main.cpp src/ui.cpp src/GraphRenderer.cpp src/DashboardData.cpp src/dataloader.cpp src/analyzer_decisiontree.cpp src/sparsematrix.cpp src/kdtree.cpp -Iinclude -lsfml-graphics -lsfml-window -lsfml-system -o epidemic_ui

./epidemic_u
```
