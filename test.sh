#!/bin/bash

# Test script for RPC Task Processing System

echo "=== RPC Task Processing System - Automated Test ==="
echo ""

# Check if server is running
if ! pgrep -f "./server" > /dev/null; then
    echo "Starting server..."
    ./server > server.log 2>&1 &
    SERVER_PID=$!
    sleep 2
    
    if ! ps -p $SERVER_PID > /dev/null; then
        echo "ERROR: Server failed to start. Check server.log"
        cat server.log
        exit 1
    fi
    
    echo "Server started with PID: $SERVER_PID"
else
    echo "Server already running"
fi

echo ""
echo "=== Test 1: Reverse String ==="
echo "Input: 'hello world'"
echo "Expected: 'dlrow olleh'"
echo ""

# Submit task and capture output
SUBMIT_OUTPUT=$(echo -e "1\nhello world\n5" | timeout 5 ./client localhost 2>&1)
TASK_ID=$(echo "$SUBMIT_OUTPUT" | grep -oP 'Task ID: \K[0-9]+')

if [ -z "$TASK_ID" ]; then
    echo "ERROR: Could not get task ID"
    echo "$SUBMIT_OUTPUT"
else
    echo "Submitted task with ID: $TASK_ID"
    sleep 1
    # Get result using captured ID
    RESULT_OUTPUT=$(echo -e "4\n$TASK_ID\n5" | timeout 5 ./client localhost 2>&1)
    RESULT=$(echo "$RESULT_OUTPUT" | grep -oP "Task $TASK_ID result: \K.*")
    echo "Result: $RESULT"
    if [ "$RESULT" == "dlrow olleh" ]; then
        echo "✓ Test 1 PASSED"
    else
        echo "✗ Test 1 FAILED (expected 'dlrow olleh')"
    fi
fi

echo ""
echo "=== Test 2: Sum of Integers ==="
echo "Input: '5 7 1 12 4'"
echo "Expected: '29'"
echo ""

SUBMIT_OUTPUT=$(echo -e "2\n5 7 1 12 4\n5" | timeout 5 ./client localhost 2>&1)
TASK_ID=$(echo "$SUBMIT_OUTPUT" | grep -oP 'Task ID: \K[0-9]+')

if [ -z "$TASK_ID" ]; then
    echo "ERROR: Could not get task ID"
    echo "$SUBMIT_OUTPUT"
else
    echo "Submitted task with ID: $TASK_ID"
    sleep 1
    RESULT_OUTPUT=$(echo -e "4\n$TASK_ID\n5" | timeout 5 ./client localhost 2>&1)
    RESULT=$(echo "$RESULT_OUTPUT" | grep -oP "Task $TASK_ID result: \K.*")
    echo "Result: $RESULT"
    if [ "$RESULT" == "29" ]; then
        echo "✓ Test 2 PASSED"
    else
        echo "✗ Test 2 FAILED (expected '29')"
    fi
fi

echo ""
echo "=== Test 3: Fibonacci ==="
echo "Input: '20'"
echo "Expected: '6765'"
echo ""

SUBMIT_OUTPUT=$(echo -e "3\n20\n5" | timeout 5 ./client localhost 2>&1)
TASK_ID=$(echo "$SUBMIT_OUTPUT" | grep -oP 'Task ID: \K[0-9]+')

if [ -z "$TASK_ID" ]; then
    echo "ERROR: Could not get task ID"
    echo "$SUBMIT_OUTPUT"
else
    echo "Submitted task with ID: $TASK_ID"
    sleep 1
    RESULT_OUTPUT=$(echo -e "4\n$TASK_ID\n5" | timeout 5 ./client localhost 2>&1)
    RESULT=$(echo "$RESULT_OUTPUT" | grep -oP "Task $TASK_ID result: \K.*")
    echo "Result: $RESULT"
    if [ "$RESULT" == "6765" ]; then
        echo "✓ Test 3 PASSED"
    else
        echo "✗ Test 3 FAILED (expected '6765')"
    fi
fi

echo ""
echo "=== Test 4: PENDING Status ==="
echo "Testing immediate result request..."
echo ""

# Submit Fibonacci(40) which takes longer, then immediately request result
SUBMIT_OUTPUT=$(echo -e "3\n40\n5" | timeout 5 ./client localhost 2>&1)
TASK_ID=$(echo "$SUBMIT_OUTPUT" | grep -oP 'Task ID: \K[0-9]+')

if [ -z "$TASK_ID" ]; then
    echo "Skipping PENDING test (could not get task ID)"
else
    echo "Submitted task with ID: $TASK_ID"
    # Immediately request result (should be PENDING or computed)
    RESULT_OUTPUT=$(echo -e "4\n$TASK_ID\n5" | timeout 5 ./client localhost 2>&1)
    RESULT=$(echo "$RESULT_OUTPUT" | grep -oP "Task $TASK_ID result: \K.*")
    echo "Immediate result: $RESULT"
    if [ "$RESULT" == "PENDING" ]; then
        echo "✓ PENDING status working correctly"
        sleep 2
        RESULT_OUTPUT=$(echo -e "4\n$TASK_ID\n5" | timeout 5 ./client localhost 2>&1)
        RESULT=$(echo "$RESULT_OUTPUT" | grep -oP "Task $TASK_ID result: \K.*")
        echo "After wait result: $RESULT"
    else
        echo "Note: Task completed quickly (result: $RESULT)"
    fi
fi

echo ""
echo "=== Cleaning up ==="
if [ ! -z "$SERVER_PID" ]; then
    echo "Stopping server (PID: $SERVER_PID)..."
    kill $SERVER_PID 2>/dev/null
fi

echo ""
echo "=== Test Complete ==="
echo "Check server.log for detailed server output"
