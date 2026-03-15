# Basic Usage

FuzzyBunny provides a simple and intuitive API for fuzzy string matching.

## Individual Scorers

The library offers several algorithms to compare strings:

```python
import fuzzybunny

# Levenshtein Distance
score = fuzzybunny.levenshtein("kitten", "sitting")
# 0.5714...

# Token Sort Ratio
# Good for strings with the same words but in different orders
score = fuzzybunny.token_sort("apple banana", "banana apple")
# 1.0

# Jaccard Similarity
# Good for comparing sets of tokens
score = fuzzybunny.jaccard("apple banana cherry", "banana apple")
# 0.666...
```

## Ranking Candidates

To find the best matches from a list of strings, use the `rank` function:

```python
candidates = ["apple pie", "banana bread", "cherry tart", "apple turnover"]

# Find top 2 matches for "apple"
results = fuzzybunny.rank("apple", candidates, top_n=2)
# [('apple pie', 0.55), ('apple turnover', 0.35)]
```

### Partial Matching

If you want to find if a query exists as a substring of a candidate, use `mode="partial"`:

```python
# Standard rank (full match)
res_full = fuzzybunny.rank("apple", ["apple pie"], mode="full")
# Score will be ~0.55

# Partial rank (substring match)
res_partial = fuzzybunny.rank("apple", ["apple pie"], mode="partial")
# Score will be 1.0 because "apple" is exactly in "apple pie"
```

## Normalization

By default, FuzzyBunny normalizes strings by lowercasing and removing punctuation. You can disable this by passing `process=False`:

```python
# Default (case-insensitive)
fuzzybunny.levenshtein("APPLE", "apple", process=True) # 1.0

# Case-sensitive
fuzzybunny.levenshtein("APPLE", "apple", process=False) # < 1.0
```
