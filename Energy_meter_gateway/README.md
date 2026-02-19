# Energy Meter Gateway

A memory-constrained event management system for smart energy meters using a doubly linked list with circular buffer behavior.

## Features

- **Doubly Linked List**: Bidirectional navigation through events
- **Circular Buffer**: Automatic FIFO removal when buffer reaches 20 events
- **Live Display Mode**: Real-time event monitoring
- **Cursor Navigation**: Browse historical events forward/backward
- **Memory Efficient**: Fixed 20-event capacity for embedded systems

## Event Types

- `POWER_CONSUMPTION` - Power usage readings
- `VOLTAGE_LEVEL` - Voltage measurements
- `FREQUENCY_STABILITY` - Frequency deviation alerts
- `FAULT_ALERT` - Critical system faults

## Compilation

```bash
gcc -o gateway energy_meter_gateway.c -Wall -Wextra
```

## Usage

```bash
./gateway
```

### Commands

| Key | Action |
|-----|--------|
| `n` | Navigate to next event (newer) |
| `p` | Navigate to previous event (older) |
| `r` | Start live event display |
| `h` | Pause live display |
| `d` | Display current event |
| `s` | Show system status |
| `c` | Clear all events |
| `x` | Terminate system |
| `?` | Show help |

## System Behavior

- **Buffer Capacity**: Maximum 20 events
- **Overflow Handling**: Oldest event automatically removed when full
- **Cursor Position**: Starts at oldest event (head)
- **Cursor Stability**: Automatically adjusts if pointed event is deleted
- **Event Simulation**: New events generated periodically for demonstration

## Example Output

```
┌─────────────────────────────────────────────────────────
│ Type: POWER_CONSUMPTION      Value: 1250.50
│ Time: 2024-01-15 14:30:22
│ Description: Meter #101: Peak load detected
└─────────────────────────────────────────────────────────
```

## Testing

Run comprehensive test suite:

```bash
gcc -o test test_energy_gateway.c -Wall -Wextra
./test
```

Tests verify:
- System initialization
- Event addition/removal
- Navigation (forward/backward)
- Circular buffer behavior
- Cursor stability during deletions
- Memory management

## Architecture

```
[HEAD] ←→ [Event 1] ←→ [Event 2] ←→ ... ←→ [Event 20] [TAIL]
           ↑
        [CURSOR]
```

- **HEAD**: Oldest event
- **TAIL**: Newest event
- **CURSOR**: Current viewing position

## Use Case

Designed for embedded systems monitoring multiple smart energy meters with limited memory, requiring efficient event storage and retrieval.
