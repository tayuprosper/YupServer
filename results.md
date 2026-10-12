# YupServer Benchmark Results

Date: 2026-10-12

## Goal

Measure how fast YupServer is next to nginx, when both run with the same limits. Then see how much each change improves it.

## Test machine

Fill these in once. They matter if you ever write a paper.

- Laptop: Dell Latitude 5520
- CPU model: 11th Gen Intel(R) Core(TM) i5-1145G7 @ 2.60GHz
- Cores: 8
- RAM: 7.0Gi
- Kernel: run `7.0.0-38-generic`
- OS: Linux (Ubuntu based)
- Charger plugged in: (yes)

## Rules for a fair test

| Setting | Value |
|---|---|
| Threads per server | 1 |
| Connections | 1000 |
| After each reply | close the connection |
| Listen backlog | 1024 |
| Response body | `hello\n` (6 bytes) |
| Duration | 10 seconds per run |
| Runs | 5, report the median |
| Open file limit | `ulimit -n 65535` |

Command:

```
wrk -t2 -c1000 -d10s -H "Connection: close" <url>
```

Script: `./bench.sh <url>` runs this 5 times.

Ports:

- YupServer: 8080
- nginx (1 worker, own config): 8082

## Result 1: YupServer v1 vs nginx

YupServer v1 is the single-thread version. It accepts one connection at a time.

| | YupServer v1 | nginx (1 worker) |
|---|---|---|
| Requests/sec (median) | 36,050 | 50,096 |
| Latency avg (median) | 23.10 ms | 7.93 ms |
| Latency max (median) | 45.37 ms | 32.60 ms |
| Socket errors | 0 | 0 |
| Range of 5 runs (req/s) | 28,066 to 37,056 | 47,893 to 52,087 |

nginx is about 1.4 times faster on throughput. Its average latency is about 3 times lower.

### Raw runs: YupServer v1

```
run 1: 36050.11 req/s | latency 23.10ms max 50.10ms | no errors
run 2: 37056.33 req/s | latency 22.36ms max 40.16ms | no errors
run 3: 36116.11 req/s | latency 22.88ms max 44.94ms | no errors
run 4: 34953.65 req/s | latency 24.19ms max 45.37ms | no errors
run 5: 28066.34 req/s | latency 29.03ms max 53.44ms | no errors
```

### Raw runs: nginx

```
run 1: 50095.97 req/s | latency 7.93ms max 32.60ms | no errors
run 2: 49849.39 req/s | latency 7.95ms max 30.23ms | no errors
run 3: 47893.26 req/s | latency 8.63ms max 71.91ms | no errors
run 4: 52087.94 req/s | latency 7.22ms max 33.87ms | no errors
run 5: 52038.23 req/s | latency 6.88ms max 25.49ms | no errors
```

Note: run 5 of YupServer (28k) is low. A laptop can slow down from heat or background work. The median ignores it.

## Change log

Each row is one change and what it did.

| Date | Change | Req/sec | Avg latency | Max latency | Errors |
|---|---|---|---|---|---|
| 2026-10-12 | Baseline, hardcoded `Connection: close` missing | 22,860 (invalid) | 27.02 ms | 1.67 s | 228,878 read errors |
| 2026-10-12 | Fixed a bug that sent empty replies | 28,122 | 19.30 ms | 1.73 s | 7 timeouts |
| 2026-10-12 | Raised listen backlog from 10 to 1024 (100 connections) | 38,016 | 1.83 ms | 10.91 ms | 0 |
| 2026-10-12 | Fair test setup, 1000 connections, v1 vs nginx | 36,050 | 23.10 ms | 45.37 ms | 0 |

## What I learned

1. A listen backlog of 10 caused dropped connections. Clients retried after about 1 second. Fixing it cut the max latency by about 160 times.
2. Check the error counts before you trust a number. The first result (22,860 req/s) had a read error on every request.
3. A fair comparison needs the same thread count, the same connection behavior, and the same response size.
4. nginx wins because it uses epoll. It only serves clients that are ready. YupServer waits on one client at a time.

