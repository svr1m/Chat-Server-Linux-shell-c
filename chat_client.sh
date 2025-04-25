#!/bin/bash
if [ $# -ne 2 ]; then
    echo "Usage: $0 <server_address> <port>"
    exit 1
fi
echo "Welcome to chat server."
echo ""
server_address=$1
port=$2

# Create a single TCP connection for reading and writing
exec 3<>/dev/tcp/$server_address/$port

# Function to read from the server and print messages
receive_messages() {
    while true; do
        if read -r line <&3; then
            echo "$line"
        fi
    done
}

# Run receive_messages in the background
receive_messages &

# Continuously read user input and send it to the server
while true; do
    read -p "" message
    if [[ "$message" == "exit" ]]; then
        echo "Client is disconnecting..."
        kill $! # Terminate the background process
        exec 3<&- # Close the input connection
        exec 3>&- # Close the output connection
        exit 0
    fi
    echo "$message" >&3
done

