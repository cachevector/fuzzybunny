<p align="center">
  <img src="https://raw.githubusercontent.com/cachevector/fuzzybunny/master/docs/assets/fuzzybunny.png" alt="FuzzyBunny Logo" width="150" />
</p>

<h1 align="center">FuzzyBunny</h1>

<p align="center">
  <b> A high-performance, lightweight Python library for fuzzy string matching and ranking, implemented in C++ with Pybind11. </b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/License-MIT-green" />
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C" />
  <img src="https://img.shields.io/badge/Bindings-Pybind11-blue" />
</p>

## Features

- **Blazing Fast**: C++ core for 2-5x speed improvement over pure Python alternatives.
- **Multiple Scorers**: Support for Levenshtein, Jaccard, Token Sort, Token Set, QRatio, WRatio, and Partial Ratio.
- **Partial Matching**: Find the best substring matches using `mode="partial"`.
- **Hybrid Scoring**: Combine multiple scorers with custom weights for complex matching tasks.
- **Pandas & NumPy Integration**: Native support for Series and Arrays via a dedicated accessor.
- **Batch Processing**: Parallelized matching for large datasets using OpenMP.
- **Unicode Support**: Handles international characters and basic normalization.
- **Benchmarking Tools**: Built-in utilities to measure and compare performance.
- **Thread Safe**: Releases the GIL in C++ for optimal multi-threaded performance.
- **Type Safe**: Includes PEP 561 type stubs for full IDE and MyPy support.

## Installation

```bash
pip install fuzzybunny
```

## Quick Start

```python
import fuzzybunny

# Basic matching
score = fuzzybunny.levenshtein("kitten", "sitting")
print(f"Similarity: {score:.2f}")

# Ranking candidates
candidates = ["apple", "apricot", "banana", "cherry"]
results = fuzzybunny.rank("app", candidates, top_n=2)
# [('apple', 0.6), ('apricot', 0.42)]
```

## Advanced Usage

### Hybrid Scorer
Combine different algorithms using custom weights:

```python
results = fuzzybunny.rank(
    "apple banana", 
    ["banana apple"], 
    scorer="hybrid", 
    weights={"levenshtein": 0.3, "token_sort": 0.7}
)
```

### Partial Matching
Find the best substring match:

```python
score = fuzzybunny.partial_ratio("apple", "apple pie") # 1.0

# Using rank with partial mode
results = fuzzybunny.rank("apple", ["apple pie", "banana"], mode="partial")
# [('apple pie', 1.0), ('banana', 0.18)]
```

### Pandas Integration
Use the specialized `fuzzy` accessor:

```python
import pandas as pd
import fuzzybunny

df = pd.DataFrame({"names": ["apple pie", "banana bread", "cherry tart"]})
results = df["names"].fuzzy.match("apple", mode="partial")
```

### Benchmarking
Compare performance on your specific data:

```python
perf = fuzzybunny.benchmark("query", candidates)
print(f"Levenshtein mean time: {perf['levenshtein']['mean']:.6f}s")
```

## License
MIT