# IRC-Server

An IRC server built in C++ as a group project at Hive Helsinki.

The goal of this project was to recreate the core behavior of a real IRC server using low-level network programming and non-blocking I/O. The server communicates through TCP/IP and is capable of handling multiple clients simultaneously without blocking or using threads/processes for each connection.

Our implementation supports user authentication, nickname and username registration, channel management, private messaging, operator privileges, and several channel modes. We used `epoll` to efficiently manage all socket events and handle incoming/outgoing communication between connected clients.

This IRC server was developed collaboratively by a team of 3 students at Hive Helsinki: @username1, @username2, and @yourusername.

---

## Features

- Multiple client connections
- Authentication with password
- Nickname and username registration
- Channel creation and joining
- Private messaging
- Channel operators
- Non-blocking sockets with `epoll`

### Supported operator commands

- `KICK`
- `INVITE`
- `TOPIC`
- `MODE`

### Supported channel modes

- `i` — invite only
- `t` — topic restricted to operators
- `k` — channel password
- `o` — operator privileges
- `l` — user limit

---

## Compilation

```bash
make
```

Run the server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 password
```

---

## Tested with

- `irssi`
- `nc` (`netcat`)

---

## Example

Connect with netcat:

```bash
nc -C 127.0.0.1 6667
```

Or with `irssi`:

```bash
/connect 127.0.0.1 6667 password
```

Join a channel:

```bash
/join #general
```

---

## Project Structure

```text
.
├── includes/
├── src/
├── Makefile
└── README.md
```
