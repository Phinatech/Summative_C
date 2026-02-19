# Graph Social Network

Interaction mapping tool using a directed graph with dynamic adjacency matrix to track user relationships.

## Features

- **Directed Graph**: Tracks one-way user interactions (A → B)
- **Dynamic Adjacency Matrix**: Grows/shrinks as users are added/removed
- **Query System**: View incoming and outgoing interactions per user
- **Interactive Menu**: Add/remove users and interactions in real-time

## Data Structure

**Adjacency Matrix Representation**:
- Rows: "From" users
- Columns: "To" users
- Value 1: Interaction exists
- Value 0: No interaction

```
     U101 U102 U103
U101   0    1    1     (U101 interacts with U102, U103)
U102   0    0    1     (U102 interacts with U103)
U103   0    0    0     (U103 has no outgoing interactions)
```

## Compilation

```bash
gcc -o social main.c graph.c -Wall -Wextra
```

## Usage

```bash
./social
```

### Menu Options

```
1) Query a user (incoming + outgoing)
2) Print adjacency matrix
3) Add user
4) Remove user
5) Add interaction (FROM -> TO)
6) Remove interaction (FROM -> TO)
0) Exit
```

## Example Session

```
Choose an option: 1
Enter user ID to query (e.g., U103): U104

==============================
 Query Result for User: U104
==============================
Outgoing (interacts with):
  -> U105
  -> U106
Incoming (interacted by):
  <- U102

Summary: 2 outgoing, 1 incoming
==============================
```

## Initial Dataset

Pre-loaded with 8 users (U101-U108) and their interactions:
- U101 → U102, U103
- U102 → U104
- U103 → U105
- U104 → U105, U106
- U105 → U107
- U106 → U108

## Operations

- **Add User**: Expands matrix by one row and column
- **Remove User**: Shrinks matrix and removes all related interactions
- **Add Interaction**: Sets adjacency matrix cell to 1
- **Remove Interaction**: Sets adjacency matrix cell to 0
- **Query User**: Shows all incoming and outgoing connections

## Constraints

- User IDs must start with 'U' (e.g., U103)
- Self-interactions not allowed (U101 → U101)
- Duplicate interactions prevented

## Use Case

Social network analysis tool for mapping directed relationships, influence tracking, or communication patterns in user communities.
