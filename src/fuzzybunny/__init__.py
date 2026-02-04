from . import _fuzzybunny
from ._fuzzybunny import (
    levenshtein,
    jaccard,
    token_sort,
    partial_ratio,
)

from .benchmark import benchmark, benchmark_batch

def rank(query, candidates, scorer="levenshtein", mode="full", process=True, threshold=0.0, top_n=-1, weights=None):
    """
    Enhanced rank function with support for Pandas Series and NumPy arrays.
    """
    if weights is None:
        weights = {}

    # Check for pandas/numpy
    if _is_pandas_series(candidates):
        candidates = candidates.astype(str).tolist()
    elif _is_numpy_array(candidates):
        candidates = candidates.astype(str).tolist()

    return _fuzzybunny.rank(query, candidates, scorer, mode, process, threshold, top_n, weights)

def batch_match(queries, candidates, scorer="levenshtein", mode="full", process=True, threshold=0.0, top_n=-1, weights=None):
    """
    Enhanced batch_match function with support for Pandas/NumPy candidates.
    """
    if weights is None:
        weights = {}

    if _is_pandas_series(candidates):
        candidates = candidates.astype(str).tolist()
    elif _is_numpy_array(candidates):
        candidates = candidates.astype(str).tolist()

    # queries can also be pandas/numpy
    if _is_pandas_series(queries) or _is_numpy_array(queries):
        import numpy as np
        queries = np.array(queries).astype(str).tolist()

    return _fuzzybunny.batch_match(queries, candidates, scorer, mode, process, threshold, top_n, weights)

def _is_pandas_series(obj):
    try:
        import pandas as pd
        return isinstance(obj, pd.Series)
    except ImportError:
        return False

def _is_numpy_array(obj):
    try:
        import numpy as np
        return isinstance(obj, np.ndarray)
    except ImportError:
        return False

def _register_pandas_accessor():
    try:
        import pandas as pd
        
        @pd.api.extensions.register_series_accessor("fuzzy")
        class FuzzyAccessor:
            def __init__(self, pandas_obj):
                self._obj = pandas_obj

            def match(self, query, scorer="levenshtein", mode="full", process=True, threshold=0.0, top_n=-1, weights=None):
                return rank(query, self._obj, scorer, mode, process, threshold, top_n, weights)
    except (ImportError, AttributeError):
        pass

_register_pandas_accessor()

__version__ = getattr(_fuzzybunny, "__version__", "dev")