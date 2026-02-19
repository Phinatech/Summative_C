# Binary Tree Command Authorization System

A secure industrial control terminal that uses a Binary Search Tree (BST) for efficient command validation and authorization.

## Features

- **Fast Command Lookup**: O(log n) search using balanced BST
- **Typo Detection**: Levenshtein distance algorithm suggests corrections
- **Security Logging**: Automatically logs unauthorized command attempts
- **Case-Sensitive Matching**: Exact command matching required
- **Interactive Terminal**: User-friendly command-line interface

## Project Structure

```
Binary_Tree_Command/
├── main.c                  # Main program entry and UI
├── bst.c/h                 # Binary Search Tree implementation
├── command_processor.c/h   # Command validation and execution
├── file_ops.c/h            # File I/O operations
├── fuzzy_match.c/h         # Levenshtein distance matching
└── test_main.c             # Unit tests
```

## Compilation

```bash
gcc -o terminal main.c bst.c command_processor.c file_ops.c fuzzy_match.c -Wall -Wextra
```

## Usage

```bash
./terminal
```

### Commands

**Special Commands:**
- `list` - Display all approved commands
- `stats` - Show system statistics
- `help` - Display help menu
- `clear` - Clear the screen
- `exit` - Exit the terminal

**Control Commands:**
Enter any approved command from the loaded command list. Commands are case-sensitive.

## Configuration

The system reads approved commands from `approved_commands.txt`. If the file doesn't exist, a sample file is created automatically with common industrial control commands.

### Sample Commands File Format
```
START_MOTOR
STOP_MOTOR
EMERGENCY_SHUTDOWN
RESET_SYSTEM
```

## Security Features

- All unauthorized commands are logged to `rejected_commands.log`
- Typo suggestions only appear if edit distance ≤ 3
- Commands must match exactly (case-sensitive)
- Session statistics track all command attempts

## Example Session

```
Terminal> START_MOTOR
✓ Command executed: START_MOTOR

Terminal> start_motor
✗ Command not recognized: start_motor
   Did you mean: START_MOTOR?

Terminal> stats
═══════════════════════════════════════════════════════════
                  SYSTEM STATISTICS
═══════════════════════════════════════════════════════════
Total approved commands : 15
Commands executed       : 1
Commands suggested      : 1
Commands rejected       : 0
```

## Testing

Compile and run the test suite:

```bash
gcc -o test test_main.c bst.c command_processor.c file_ops.c fuzzy_match.c -Wall -Wextra
./test
```

## License

Educational project for demonstrating BST data structures and command validation systems.
