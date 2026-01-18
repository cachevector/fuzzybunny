<p align="center">
  <img src="./docs/assets/fuzzybunny.png" alt="FuzzyBunny Logo" width="150" />
</p>

<h1 align="center">FuzzyBunny</h1>

<p align="center">
  <b> A fuzzy search tool written in C++ with Python bindings </b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/License-MIT-green" />
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C" />
  <img src="https://img.shields.io/badge/Bindings-Pybind11-blue" />
</p>

## Overview

FuzzyBunny is a lightweight, high-performance Python library for fuzzy string matching and ranking. It is implemented in C++ for speed and exposes a Pythonic API via Pybind11. It supports various scoring algorithms including Levenshtein, Jaccard, and Token Sort, along with partial matching capabilities.

## Features

- **Fast C++ Core**: Optimized string matching algorithms.
- **Multiple Scorers**:
  - `levenshtein`: Standard edit distance ratio.
  - `jaccard`: Set-based similarity.
  - `token_sort`: Sorts tokens before comparing (good for "Apple Banana" vs "Banana Apple").
- **Ranking**: Efficiently rank a list of candidates against a query.
- **Partial Matching**: Support for substring matching via `mode='partial'`.
- **Unicode Support**: Correctly handles UTF-8 input.

## Installation

### Prerequisites
- Python 3.8+
- C++17 compatible compiler (GCC, Clang, MSVC)

### Using uv (Recommended)

```bash
uv pip install .
```

### Using pip

```bash
pip install .
```

## Usage

```python
import fuzzybunny

# Basic Levenshtein Ratio
score = fuzzybunny.levenshtein("kitten", "sitting")
print(f"Score: {score}")  # ~0.57

# Partial Matching
# "apple" is a perfect substring of "apple pie"
score = fuzzybunny.partial_ratio("apple", "apple pie")
print(f"Partial Score: {score}")  # 1.0

# Ranking Candidates
candidates = ["apple pie", "banana bread", "cherry tart", "apple crisp"]
results = fuzzybunny.rank(
    query="apple", 
    candidates=candidates, 
    scorer="levenshtein", 
    mode="partial", 
    top_n=2
)

for candidate, score in results:
    print(f"{candidate}: {score}")
# Output:
# apple pie: 1.0
# apple crisp: 1.0
```

## Development

1. **Setup Environment**:
   ```bash
   uv venv
   source .venv/bin/activate
   ```

2. **Install in Editable Mode**:
   ```bash
   uv pip install -e .
   ```

3. **Run Tests**:
   ```bash
   pytest
   ```

## License

This project is licensed under the [MIT License](LICENSE).