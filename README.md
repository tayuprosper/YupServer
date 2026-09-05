# YupServer

YupServer is a small HTTP server written in C++17 using POSIX TCP sockets. It
is an upgrade of [http-on-tcp](https://github.com/tayuprosper/http-on-tcp),
continuing the original project's goal of making HTTP and socket programming
easy to inspect and extend.

The project is intentionally lightweight. It has no external runtime
dependencies or web framework: it creates a TCP socket, accepts connections,
parses a basic HTTP request, and sends an HTTP response.

## Current Status

This is an early development version. The server currently:

- Listens on port `8080`.
- Accepts one connection at a time.
- Parses the request method, path, and a `Content-Length` request body.
- Recognizes `GET`, `POST`, `PUT`, `PATCH`, `DELETE`, `HEAD`, and `OPTIONS`.
- Returns fixed responses for `GET`, `POST`, and `PUT`.
- Rejects unsupported methods with `405 Method Not Allowed`.
- Initializes a document root and includes URL decoding and path-traversal
  checks for the planned static-file serving flow.

Static files are not served yet. The requested path is currently parsed but is
not used to select a file from the document root.

## Requirements

- Linux, macOS, WSL, or another Unix-like operating system
- `g++` with C++17 support
- `make`

On Debian or Ubuntu, install the build tools with:

```bash
sudo apt update
sudo apt install build-essential
```

## Build

Clone the repository and build the executable:

```bash
git clone https://github.com/tayuprosper/YupServer.git
cd YupServer
make
```

The build creates an executable named `app` in the project root.

## Run

Start the server by passing the directory that will act as its document root:

```bash
./app ./public
```

The directory must already exist. The server will listen on:

```text
http://localhost:8080
```

You can test it with `curl`:

```bash
curl -i http://localhost:8080/
curl -i -X POST -d 'hello YupServer' http://localhost:8080/
curl -i -X PUT -d 'updated content' http://localhost:8080/
```

Expected response behavior in the current version:

| Request method | Status | Response |
| --- | --- | --- |
| `GET` | `200 OK` | `Hello world` |
| `POST` | `201 Created` | `Resource Created successfully` |
| `PUT` | `200 OK` | `Resource Updated successfully` |
| Other methods | `405 Method Not Allowed` | `Method not supported` |

Stop the server with `Ctrl+C`.

## Make Targets

```bash
make        # Build the app executable
make run    # Build and run ./app; currently requires a valid directory argument
make clean  # Remove the executable and object files
```

## Project Structure

```text
.
├── main.cpp          # Application entry point and connection loop
├── makefile          # Build and cleanup targets
└── server/
    ├── server.h      # HttpServer interface
    └── server.cpp    # Socket setup, request parsing, and path utilities
```

## Design Notes

YupServer is designed as a learning project and a base for further HTTP
features. The main areas intended for future work are:

- Serve files from the configured document root.
- Add proper `404`, `400`, and other error responses.
- Handle connections concurrently.
- Improve HTTP header parsing and request-body handling.
- Add configurable ports and command-line validation.
- Add automated tests for parsing and path security.

## Relationship to http-on-tcp

YupServer is the next iteration of
[tayuprosper/http-on-tcp](https://github.com/tayuprosper/http-on-tcp). It keeps
the predecessor's minimal TCP and HTTP approach while beginning to separate
server responsibilities and address document-root and path-safety concerns.
