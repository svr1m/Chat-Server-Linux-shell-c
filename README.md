
# Chat Server (C + Shell Scripting)

A simple multi-client chat server project built using C (server-side) and Shell scripting with Netcat (client-side).

## 📌 Table of Contents

- [Introduction](#introduction)
- [Abstract](#abstract)
- [Project Scope](#project-scope)
- [Functionalities](#functionalities)
- [Constraints](#constraints)
- [Workflow](#workflow)
- [How to Run](#how-to-run)

---

## 📖 Introduction

This project is a basic chat application where multiple clients can connect to a server and exchange text messages in real-time. The server is implemented in C using socket programming, and the clients use Shell scripting with `netcat` (nc).

## 🧠 Abstract

The server handles multiple clients concurrently, managing connections, broadcasting messages to all clients, and handling disconnections. It's a basic, command-line based chat system focusing on core networking concepts.

## 📋 Project Scope

- Multi-client handling
- Broadcasting messages to all clients
- Basic error handling
- Implemented using socket programming in C and Shell scripting

## ⚙️ Functionalities

- **Server Initialization:** Socket creation, binding, and listening
- **Client Handling:** Accepting new clients, maintaining connections
- **Message Broadcasting:** Server relays messages from one client to all others
- **Graceful Disconnection:** Detects and handles client disconnections

## ⚠️ Constraints

- Max 10 concurrent clients (due to fixed-size array)
- No GUI, encryption, or authentication
- Only basic message broadcasting

## 🔁 Workflow Diagram

> You can upload a workflow diagram here as an image if available.

## 🚀 How to Run

### Requirements

- Linux environment
- gcc compiler
- `netcat` installed

### Server

```bash
gcc server.c -o server
./server
```

### Client

```bash
nc <server-ip> <port>
```

Each client can now type and see messages from other clients connected to the server.

