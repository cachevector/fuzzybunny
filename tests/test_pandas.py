import pytest
import fuzzybunny
try:
    import pandas as pd
    import numpy as np
    HAS_PANDAS = True
except ImportError:
    HAS_PANDAS = False

@pytest.mark.skipif(not HAS_PANDAS, reason="pandas not installed")
def test_pandas_series_support():
    s = pd.Series(["apple", "banana", "cherry"])
    results = fuzzybunny.rank("app", s)
    assert results[0][0] == "apple"
    assert results[0][1] > 0.5

@pytest.mark.skipif(not HAS_PANDAS, reason="pandas not installed")
def test_pandas_accessor():
    s = pd.Series(["apple pie", "banana bread", "cherry tart"])
    # Test the accessor
    results = s.fuzzy.match("apple", mode="partial")
    assert results[0][0] == "apple pie"
    assert results[0][1] == 1.0

@pytest.mark.skipif(not HAS_PANDAS, reason="numpy not installed")
def test_numpy_support():
    arr = np.array(["apple", "banana", "cherry"])
    results = fuzzybunny.rank("ban", arr)
    assert results[0][0] == "banana"
