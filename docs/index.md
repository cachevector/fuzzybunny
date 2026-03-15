# FuzzyBunny

A high-performance, lightweight Python library for fuzzy string matching and ranking, implemented in C++ with Pybind11.

## Features

- **Blazing Fast**: Optimized C++ core (Myers' Bit-Parallel algorithm) for superior performance.
- **Multiple Scorers**: Support for Levenshtein, Jaccard, Token Sort, Token Set, QRatio, and WRatio.
- **Partial Matching**: Find the best substring matches.
- **Hybrid Scoring**: Combine multiple scorers with custom weights.
- **Python Callbacks**: Use your own Python functions as scorers.
- **Pandas & NumPy Integration**: Native support for Series and Arrays.
- **Parallelized**: Parallel matching for large datasets using OpenMP.

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

## Installation

```bash
pip install fuzzybunny
```

*Note: On macOS, it is recommended to have `libomp` installed via Homebrew for full parallel processing support.*
