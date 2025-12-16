# Multithreaded RPC-Based Distributed Task Processing System

**Course**: CSE 303 Fundamentals of Operating Systems  
**Assignment**: Multithreaded RPC Task Processing System

## Overview

This project implements a distributed task processing system using Remote Procedure Calls (RPC) and multithreading. Clients submit tasks to a server via RPC, and the server processes them concurrently using a pool of worker threads.

## System Architecture

### Components

1. **RPC Interface** (`task.x`): Defines the communication protocol between client and server
2. **Task Queue** (`task_queue.c/h`): Thread-safe FIFO queue for pending tasks
3. **Result Store** (`result_store.c/h`): Thread-safe storage for completed task results
4. **Task Processor** (`task_processor.c/h`): Implementation of three task types
5. **Server** (`server.c`): RPC server with worker thread pool
6. **Client** (`client.c`): Interactive client for task submission and result retrieval

### Supported Task Types

| Type | Description | Example Input | Expected Output |
|------|-------------|---------------|-----------------|
| 1 | Reverse String | `"hello world"` | `"dlrow olleh"` |
| 2 | Sum of Integers | `"5 7 1 12 4"` | `"29"` |
| 3 | Fibonacci Number | `"20"` | `"6765"` |

### Thread Synchronization

- **Task Queue**: Uses `pthread_mutex_t` for mutual exclusion and `pthread_cond_t` for efficient worker wake-up
- **Result Store**: Uses `pthread_mutex_t` to protect concurrent access to results
- **Task ID Generation**: Protected by a separate mutex to ensure unique IDs
- **Worker Threads**: 3 threads continuously process tasks from the shared queue

## Compilation Instructions

### Prerequisites
- GCC compiler
- RPC tools (`rpcgen`)
- pthread library
- Linux/Unix environment (tested on Ubuntu 24.04)

### Build Steps

```bash
# Build the entire project
make

# This will:
# 1. Generate RPC stubs from task.x using rpcgen
# 2. Compile server and client programs
# 3. Link with pthread library
```

### Clean Build

```bash
# Remove all generated files and binaries
make clean
```

## Execution Instructions

### Starting the Server

```bash
# Run the server (default port)
./server
```

The server will:
- Initialize the task queue and result store
- Spawn 3 worker threads
- Start listening for RPC connections
- Display status messages for submitted and processed tasks

### Running the Client

```bash
# Connect to server (replace 'localhost' with server hostname if remote)
./client localhost
```

The client provides an interactive menu:

```
=== Task Processing Client ===
1. Submit Task (Type 1: Reverse String)
2. Submit Task (Type 2: Sum Integers)
3. Submit Task (Type 3: Fibonacci)
4. Get Result
5. Exit
```

### Example Usage Session

1. **Submit a reverse string task**:
   - Choose option `1`
   - Enter: `hello world`
   - Note the returned task ID (e.g., Task ID: 1)

2. **Submit a sum task**:
   - Choose option `2`
   - Enter: `5 7 1 12 4`
   - Note the returned task ID (e.g., Task ID: 2)

3. **Submit a Fibonacci task**:
   - Choose option `3`
   - Enter: `20`
   - Note the returned task ID (e.g., Task ID: 3)

4. **Retrieve results**:
   - Choose option `4`
   - Enter task ID: `1`
   - Result: `dlrow olleh`

## Design Description

### Workflow

1. **Client submits task** → Calls `SUBMIT_TASK` RPC with task type and payload
2. **Server receives task** → Generates unique ID, pushes to thread-safe queue
3. **Worker thread picks task** → Blocks on queue until task available
4. **Worker processes task** → Calls appropriate processor function
5. **Worker stores result** → Saves to thread-safe result store
6. **Client requests result** → Calls `GET_RESULT` RPC with task ID
7. **Server returns result** → Returns computed result or "PENDING"

### Thread Safety Guarantees

- **Producer-Consumer Pattern**: Clients (producers) add tasks to queue, workers (consumers) remove them
- **No Race Conditions**: All shared data protected by mutexes
- **Deadlock Prevention**: Proper lock ordering, no nested locks
- **Condition Variables**: Efficient blocking instead of busy-waiting

## Test Plan

### Test 1: Individual Task Types

**Objective**: Verify each task type produces correct results

```bash
# Terminal 1: Start server
./server

# Terminal 2: Test each type
./client localhost
# Option 1, input: "hello world" → expected: "dlrow olleh"
# Option 2, input: "5 7 1 12 4" → expected: "29"
# Option 3, input: "20" → expected: "6765"
```

**Expected**: All three tasks return correct results

### Test 2: Concurrent Submissions

**Objective**: Verify server handles multiple clients simultaneously

```bash
# Terminal 1: Start server
./server

# Terminal 2: Client 1
./client localhost
# Submit multiple tasks

# Terminal 3: Client 2
./client localhost
# Submit multiple tasks concurrently

# Terminal 4: Client 3
./client localhost
# Submit multiple tasks concurrently
```

**Expected**: All tasks processed correctly, no crashes, all results retrievable

### Test 3: PENDING Status

**Objective**: Verify incomplete tasks return "PENDING"

```bash
# Terminal 1: Start server
./server

# Terminal 2: Client
./client localhost
# Submit task (e.g., Fibonacci 20)
# Immediately request result (should be "PENDING")
# Wait a moment, request again (should show result)
```

**Expected**: 
- First retrieval: "PENDING"
- Second retrieval: Computed result (e.g., "6765")

### Test 4: High Load

**Objective**: Test system under load

```bash
# Submit 50+ tasks rapidly from multiple clients
# Verify all tasks eventually complete
# Check for memory leaks using valgrind (optional)
```

**Expected**: All tasks processed, no crashes, stable performance

## File Structure

```
.
├── task.x                  # RPC interface definition
├── task_queue.h            # Task queue interface
├── task_queue.c            # Task queue implementation
├── result_store.h          # Result store interface
├── result_store.c          # Result store implementation
├── task_processor.h        # Task processor interface
├── task_processor.c        # Task processor implementation
├── server.c                # RPC server with worker threads
├── client.c                # Interactive RPC client
├── Makefile                # Build system
└── README.md               # This file
```

## Implementation Notes

- **Task ID Assignment**: Auto-incremented starting from 1
- **Worker Threads**: Fixed pool of 3 threads (configurable via `NUM_WORKERS`)
- **Queue Size**: Unlimited (dynamic linked list)
- **Result Storage**: Supports up to 1000 results (configurable via `MAX_RESULTS`)
- **RPC Protocol**: Uses TCP for reliable communication


## Author

CSE 303 
Burak Yalçın 20220808069
Fall 2025
