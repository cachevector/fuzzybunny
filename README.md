<p align="center">
  <img src="./docs/assets/fuzzybunny.png" alt="FuzzyBunny Logo" width="150" />
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
- **Multiple Scorers**: Support for Levenshtein, Jaccard, and Token Sort ratios.
- **Partial Matching**: Find the best substring matches.
- **Hybrid Scoring**: Combine multiple scorers with custom weights.
- **Pandas & NumPy Integration**: Native support for Series and Arrays.
- **Batch Processing**: Parallelized matching for large datasets using OpenMP.
- **Unicode Support**: Handles international characters and normalization.
- **Benchmarking Tools**: Built-in utilities to measure performance.

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
Combine different algorithms to get better results:

```python
results = fuzzybunny.rank(
    "apple banana", 
    ["banana apple"], 
    scorer="hybrid", 
    weights={"levenshtein": 0.3, "token_sort": 0.7}
)
```

### Pandas Integration
Use the specialized accessor for clean code:

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