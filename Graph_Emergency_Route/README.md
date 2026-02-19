# Graph Emergency Route Finder

Network routing system using Dijkstra's algorithm to find optimal paths with minimum latency in emergency scenarios.

## Features

- **Dijkstra's Algorithm**: Finds shortest path in weighted graph
- **Bidirectional Links**: Undirected network topology
- **Latency Optimization**: Minimizes total path latency
- **Interactive Input**: User-specified source and destination nodes

## Network Topology

```
        S1 ----8---- S2 ----7---- S3
        |            |            |
       20            3           12
        |            |            |
        S4 ----4---- S5           S6
                     |            |
                     +-----6------+
                     
        X (intermediate node)
        S2 --3-- X --5-- S5
```

**Nodes**: S1, S2, S3, S4, S5, S6, X  
**Edges**: Weighted by latency (bidirectional)

## Compilation

```bash
gcc -o route main.c graph.c dijkstra.c utils.c -Wall -Wextra
```

## Usage

```bash
./route
```

### Example Session

```
Enter start node (e.g., S1): S1
Enter destination node (e.g., S6): S6

Optimal routing path: S1 -> S2 -> X -> S5 -> S6
Total latency: 22
```

## Algorithm

**Dijkstra's Shortest Path**:
- Time Complexity: O(N²) where N = 7 nodes
- Finds minimum latency path between any two nodes
- Handles disconnected nodes gracefully

## Project Structure

```
Graph_Emergency_Route/
├── main.c       # User interface and path display
├── graph.c/h    # Graph representation and topology
├── dijkstra.c/h # Shortest path algorithm
└── utils.c/h    # Input validation utilities
```

## Valid Nodes

- **S1-S6**: Server/switch nodes
- **X**: Intermediate routing node

## Use Case

Emergency network routing system that calculates optimal evacuation or data transmission paths with minimum latency in critical infrastructure networks.
