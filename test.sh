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

# Create test client input
cat << 'EOF' > test_input_1.txt
1
hello world
4
1
5
EOF

echo "Submitting reverse string task..."
timeout 5 ./client localhost < test_input_1.txt > test_output_1.txt 2>&1
echo "Result:"
cat test_output_1.txt | grep -A 2 "Task ID" || echo "No result captured"

echo ""
echo "=== Test 2: Sum of Integers ==="
echo "Input: '5 7 1 12 4'"
echo "Expected: '29'"
echo ""

cat << 'EOF' > test_input_2.txt
2
5 7 1 12 4
4
2
5
EOF

echo "Submitting sum task..."
timeout 5 ./client localhost < test_input_2.txt > test_output_2.txt 2>&1
echo "Result:"
cat test_output_2.txt | grep -A 2 "Task ID" || echo "No result captured"

echo ""
echo "=== Test 3: Fibonacci ==="
echo "Input: '20'"
echo "Expected: '6765'"
echo ""

cat << 'EOF' > test_input_3.txt
3
20
4
3
5
EOF

echo "Submitting Fibonacci task..."
timeout 5 ./client localhost < test_input_3.txt > test_output_3.txt 2>&1
echo "Result:"
cat test_output_3.txt | grep -A 2 "Task ID" || echo "No result captured"

echo ""
echo "=== Cleaning up ==="
if [ ! -z "$SERVER_PID" ]; then
    echo "Stopping server (PID: $SERVER_PID)..."
    kill $SERVER_PID 2>/dev/null
fi

# Clean up test files
rm -f test_input_*.txt test_output_*.txt

echo ""
echo "=== Test Complete ==="
echo "Check server.log for detailed server output"
