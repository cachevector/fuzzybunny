# Basic Usage

FuzzyBunny provides a simple and intuitive API for fuzzy string matching.

## Individual Scorers

The library offers several algorithms to compare two strings directly. These functions expect strings as input and return a score between 0.0 and 1.0.

```python
import fuzzybunny

# Levenshtein Ratio (edit distance)
fuzzybunny.levenshtein("kitten", "sitting")
# 0.5714...

# Partial Ratio (best substring match)
fuzzybunny.partial_ratio("apple", "apple pie")
# 1.0

# Token Sort Ratio (alphabetical word ordering)
fuzzybunny.token_sort("apple banana", "banana apple")
# 1.0

# Token Set Ratio (set intersection/difference)
# Good for strings with extra words or duplicates
fuzzybunny.token_set("apple banana", "apple banana banana")
# 1.0

# Jaccard Similarity (intersection over union)
fuzzybunny.jaccard("apple banana cherry", "banana apple")
# 0.666...

# WRatio (Weighted Ratio - Recommended for general use)
fuzzybunny.wratio("fuzzy bunny", "bunny fuzzy!!!")
# 1.0
```

!!! info "Direct vs. Ranked Matching"
    Individual scorer functions (like `levenshtein`, `jaccard`, etc.) do **not** automatically normalize your strings. They perform a direct comparison. If you need automatic lowercasing or punctuation removal, use `rank` or `batch_match`, or preprocess your strings manually.

## Ranking Candidates

To find the best matches from a list of strings, use the `rank` function. This function *does* provide integrated normalization.

```python
candidates = ["apple pie", "banana bread", "cherry tart", "apple turnover"]

# Find top 2 matches for "apple"
# By default, it uses 'levenshtein' and 'process=True'
results = fuzzybunny.rank("apple", candidates, top_n=2)
# [('apple pie', 0.55), ('apple turnover', 0.35)]
```

### Partial Matching

If you want to find if a query exists as a substring of a candidate, use `mode="partial"`. In `rank`, this uses the `partial_ratio` logic.

```python
# Standard rank (full match)
res_full = fuzzybunny.rank("apple", ["apple pie"], mode="full")
# Score: 0.555...

# Partial rank (substring match)
res_partial = fuzzybunny.rank("apple", ["apple pie"], mode="partial")
# Score: 1.0
```

## Normalization

By default, `rank` and `batch_match` normalize strings by lowercasing and removing punctuation. You can disable this by passing `process=False`:

```python
# Default (case-insensitive & punctuation-agnostic)
fuzzybunny.rank("APPLE!", ["apple"], process=True) 
# [('apple', 1.0)]

# Case-sensitive and strict
fuzzybunny.rank("APPLE!", ["apple"], process=False) 
# [('apple', 0.0)]
```
