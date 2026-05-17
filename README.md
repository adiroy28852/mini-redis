# mini-redis

A Redis-compatible in-memory key-value store built from scratch in C++17.

I Built this to understand how Redis actually works internally — raw TCP sockets, the RESP wire protocol, epoll-based I/O, LRU eviction, and layered middleware via design patterns. And trying to get started with systems programming.


This program is currently small and lightweight, so even on a windows machine, with WSL, it will run easy and fast. [My specs (that i thought would be relevant to share) are here](specs.txt)

This project is divided into 3 phases. 3 because 2 felt overwhelming, and more than 3 could mean me giving up. 

Phase 1 is a simple foundation on redis, currently supports string based KV caching.

Phase 2 will start once I have good foundations on multithreading and concurrency, atomics and high-performance networking
I may have missed some topics. 

Phase 3 will have all the things you will eventually read below. Even I dont know what i will need to learn :) 

---

## Setup (Windows)

### Requirements
- Windows with WSL2 (Ubuntu)
- VS Code with the WSL extension

### Step 1 — Install WSL2

Open PowerShell as Administrator:

```powershell
wsl --install
```

Restart when prompted. After restart, Ubuntu opens and asks for a username and password.

Verify:

```powershell
wsl --list --verbose
# Should show: Ubuntu  Running  2
```

### Step 2 — Install dependencies

Open Ubuntu (search "Ubuntu" in Start, or run `wsl` in any terminal):

```bash
sudo apt update && sudo apt upgrade -y

sudo apt install -y \
  build-essential \
  g++ \
  cmake \
  make \
  gdb \
  git \
  netcat-openbsd
```

Verify:

```bash
g++ --version      # expect 11+
cmake --version    # expect 3.16+
```

### Step 3 — Connect VS Code to WSL

1. Open VS Code
2. Install the **WSL** extension by Microsoft (`Ctrl+Shift+X`)
3. Press `Ctrl+Shift+P` → `WSL: Connect to WSL` → Enter
4. Bottom-left corner should show `WSL: Ubuntu`
5. Install inside WSL when prompted:
   - **C/C++** by Microsoft
   - **CMake Tools** by Microsoft

### Step 4 — Clone and build

```bash
git clone https://github.com/adiroy28852/mini-redis.git
cd mini-redis

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
```

### Step 5 — Run

```bash
./build/mini-redis
# [mini-redis] Listening on port 6380
```

> **Linux users:** Steps 2–5 are identical. Skip Steps 1 and 3.

---

## Testing commands

The server speaks [RESP](https://redis.io/docs/latest/develop/reference/protocol-spec/) over TCP on port 6380. Test with `netcat` — no redis-cli needed.

```bash
# PING
echo -e '*1\r\n$4\r\nPING\r\n' | nc localhost 6380

# SET / GET
echo -e '*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$6\r\nAditya\r\n' | nc localhost 6380
echo -e '*2\r\n$3\r\nGET\r\n$4\r\nname\r\n' | nc localhost 6380

# SET with expiry
echo -e '*5\r\n$3\r\nSET\r\n$7\r\nsession\r\n$3\r\nabc\r\n$2\r\nEX\r\n$2\r\n10\r\n' | nc localhost 6380

# TTL / INCR / DEL / KEYS / DBSIZE / FLUSHALL
echo -e '*2\r\n$3\r\nTTL\r\n$7\r\nsession\r\n' | nc localhost 6380
echo -e '*2\r\n$4\r\nINCR\r\n$7\r\ncounter\r\n' | nc localhost 6380
echo -e '*2\r\n$3\r\nDEL\r\n$4\r\nname\r\n' | nc localhost 6380
echo -e '*2\r\n$4\r\nKEYS\r\n$1\r\n*\r\n' | nc localhost 6380
echo -e '*1\r\n$6\r\nDBSIZE\r\n' | nc localhost 6380
echo -e '*1\r\n$8\r\nFLUSHALL\r\n' | nc localhost 6380
```

If port 6380 is already in use from a previous run:

```bash
fuser -k 6380/tcp
```

---

## Supported Commands — Phase 1 ✅

| Command | Usage | Returns |
|---------|-------|---------|
| `PING` | `PING` or `PING msg` | `+PONG` or echoes msg |
| `SET` | `SET key value [EX secs] [PX ms]` | `+OK` |
| `GET` | `GET key` | bulk string or nil |
| `DEL` | `DEL key [key ...]` | count deleted |
| `EXISTS` | `EXISTS key` | `:1` or `:0` |
| `EXPIRE` | `EXPIRE key seconds` | `:1` |
| `TTL` | `TTL key` | seconds remaining, `-1` no expiry, `-2` missing |
| `INCR` | `INCR key` | new integer value |
| `DECR` | `DECR key` | new integer value |
| `KEYS` | `KEYS pattern` | array of matching keys |
| `DBSIZE` | `DBSIZE` | count of all keys |
| `FLUSHALL` | `FLUSHALL` | `+OK` |

---

## Architecture

```
Client (netcat / redis-cli / future CLI)
    │  TCP on port 6380
    ▼
TcpServer          — epoll event loop, one thread, N clients
    │
RespParser         — stateful parser, handles partial TCP reads
    │  Command { args: ["SET", "foo", "bar"] }
    ▼
CommandHandler     — dispatches all commands
    │
LoggingStore       — decorator: logs every op + GET latency
    │
Store              — unordered_map + shared_mutex
    │
LruPolicy          — O(1) eviction via linked list + hashmap
```

---

## Design Patterns Used

| Pattern | Where | Why |
|---------|-------|-----|
| **Strategy** | `IEvictionPolicy` → `LruPolicy` | Swap eviction algorithm without touching `Store` |
| **Decorator** | `LoggingStore` wrapping `Store` | Add behaviour without modifying core store |

---


## Phase 2 — 


**`MetricsStore` decorator** — tracks hit/miss rate and total op counts. Redis doesn't have a fancy beautiful dashboard, but the ([INFO command](https://redis.io/docs/latest/commands/info/))

**LFU eviction policy** — LFU is LFU. If you don't know LFU, you should do ([this](https://leetcode.com/problems/lfu-cache/))
**More Data Types** — List (and variants), Hashmap, Sets, Sortedsets(used in leaderboards) ...
**HyperLogLog** - probabilistic DS
**Bloom Filter** - also nearly same 
**Write-Ahead Log (WAL)** 

---

## Phase 3 — Future Scope

**Consistent hashing ring** 
**CLI** — Something I want to build separately, or as a part of mini-redis anyways. 
**MULTI / EXEC** — transaction support. Queue commands per client connection, execute the whole batch atomically.
**SUBSCRIBE / PUBLISH** — pub-sub messaging. Observer pattern — one subscriber list per channel, fan-out on publish.
**Streams** — append-only log of entries with consumer groups. How Redis is used as a lightweight message queue in production.