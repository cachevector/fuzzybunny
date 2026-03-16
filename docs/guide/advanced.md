# Advanced Scoring and Performance

FuzzyBunny provides several advanced tools for performance and custom matching needs.

## WRatio (Weighted Similarity Ratio)

`WRatio` is the recommended general-purpose matcher. It combines several algorithms using heuristics to provide a more "intuitive" similarity score.

```python
import fuzzybunny

# Matches well even with different word orders and lengths
score = fuzzybunny.wratio("fuzzy bunny", "bunny fuzzy!!!")
# 1.0 (Token sort/set will match and WRatio will pick the best)
```

## Hybrid Scorer

The `hybrid` scorer allows you to define a custom weighted average of multiple built-in algorithms. This is useful when you have specific data requirements that a single algorithm can't fully capture.

To use it, set `scorer="hybrid"` and provide a `weights` dictionary in `rank` or `batch_match`.

```python
import fuzzybunny

results = fuzzybunny.rank(
    "fuzzy bunny", 
    ["bunny fuzzy", "the fuzzy bunny", "rabbit"],
    scorer="hybrid",
    weights={
        "levenshtein": 0.2,
        "token_sort": 0.5,
        "token_set": 0.3
    }
)
```

**Supported weight keys:** `levenshtein`, `jaccard`, `token_sort`, `token_set`, `qratio`, `wratio`.

## High-Performance Batch Matching

When comparing many queries against a common candidate set, `batch_match` is the most efficient choice.

It provides two major optimizations:
1.  **Normalization Caching**: In a standard loop, each candidate is normalized once per query. `batch_match` normalizes each candidate only once for the entire batch.
2.  **Multi-threading (OpenMP)**: The C++ core uses OpenMP to parallelize the comparison loops across all available CPU cores.

```python
import fuzzybunny

queries = ["apple", "banana", "cherry"]
candidates = ["apple pie", "banana bread", "cherry tart", "apple turnover"]

# Parallel matching
results = fuzzybunny.batch_match(queries, candidates, top_n=2)

# results is a list of result lists
# results[0] contains matches for "apple"
# results[1] contains matches for "banana"
```

!!! tip "Performance Hint"
    Parallel execution is automatically triggered when the number of queries is greater than 5. It releases the Python GIL during the intensive matching loops, allowing for true multi-core utilization.

## Custom Python Scorers

You can pass a custom Python function as the `scorer` argument. 

!!! warning "Performance"
    Custom Python scorers are significantly slower than C++ scorers because they must acquire the Python Global Interpreter Lock (GIL) for every comparison.

```python
def my_custom_scorer(s1, s2):
    # Your custom logic here
    # Return a score between 0.0 and 1.0
    return 1.0 if s1[0] == s2[0] else 0.0

results = fuzzybunny.rank("apple", ["apricot", "banana"], scorer=my_custom_scorer)
```

## Integration with Pandas and NumPy

FuzzyBunny integrates directly with common data science tools:

```python
import pandas as pd
import fuzzybunny

df = pd.DataFrame({"names": ["apple pie", "banana bread", "cherry tart"]})

# Use the pandas accessor
results = df["names"].fuzzy.match("apple")
```
