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

## High-Performance Batch Matching

When comparing many queries against a common candidate set, `batch_match` is the most efficient choice.

It provides two major optimizations over calling `rank` in a loop:
1.  **Multi-threading (OpenMP)**: Automatically distributes work across all CPU cores.
2.  **Normalization Caching**: Normalizes the candidate set only once per batch.

```python
import fuzzybunny

queries = ["apple", "banana", "cherry"]
candidates = ["apple pie", "banana bread", "cherry tart", "apple turnover"]

# Parallel matching
results = fuzzybunny.batch_match(queries, candidates, top_n=2)

# Results is a list where each element matches the corresponding query
for i, res in enumerate(results):
    print(f"Results for {queries[i]}: {res}")
```

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
