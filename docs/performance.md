# Performance Benchmarks

FuzzyBunny is designed for speed, utilizing an optimized C++ core with bit-parallel algorithms and multi-threading support.

## Core Matching Performance

The core Levenshtein implementation uses **Myers' Bit-Parallel algorithm** for strings up to 64 characters. This allows for $O(N)$ matching with very low constant overhead.

For longer strings, it falls back to an optimized $O(NM)$ dynamic programming approach.

## Multi-threading with OpenMP

`batch_match` is parallelized using **OpenMP**. This allows you to process large numbers of queries across all available CPU cores without being bottlenecked by the Python Global Interpreter Lock (GIL).

## In-place Normalization

By pre-normalizing candidates and queries in `batch_match`, we avoid redundant computations, making it significantly faster for bulk operations compared to repetitive calls to individual matchers.

## Benchmarking on Your Machine

You can run the built-in benchmarking tool to see how it performs on your specific hardware and data:

```python
import fuzzybunny

perf = fuzzybunny.benchmark("query", ["candidate"] * 10000)
print(perf)
```
