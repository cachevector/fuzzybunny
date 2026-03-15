from typing import Union, Callable, List, Tuple, Dict, Any, Sequence, Iterable, TYPE_CHECKING

if TYPE_CHECKING:
    import pandas as pd
    import numpy as np
    
    CandidatesType = Union[Sequence[str], Iterable[str], "pd.Series", "np.ndarray"]
    QueriesType = Union[Sequence[str], Iterable[str], "pd.Series", "np.ndarray"]
else:
    CandidatesType = Any
    QueriesType = Any

from . import _fuzzybunny
from ._fuzzybunny import (
    levenshtein,
    jaccard,
    token_sort,
    token_set,
    qratio,
    wratio,
    partial_ratio,
)

from .benchmark import benchmark, benchmark_batch

def rank(
    query: str, 
    candidates: CandidatesType, 
    scorer: Union[str, Callable[[str, str], float]] = "levenshtein", 
    mode: str = "full", 
    process: bool = True, 
    threshold: float = 0.0, 
    top_n: int = -1, 
    weights: Dict[str, float] = None
) -> List[Tuple[str, float]]:
    """
    Ranks a list of candidates based on their similarity to a query string.

    This is the primary function for finding the best matches in a collection. It supports
    multiple scoring algorithms, threshold filtering, and integrated string normalization.

    Args:
        query: The string to search for.
        candidates: A collection of strings to search through. Can be a list, 
            pandas.Series, or numpy.ndarray.
        scorer: The similarity algorithm to use. Options include:
            - `"levenshtein"`: Standard edit distance ratio.
            - `"wratio"`: Weighted combination of multiple algorithms (recommended).
            - `"qratio"`: Simplified Levenshtein ratio.
            - `"token_sort"`: Sorts tokens before comparison.
            - `"token_set"`: Set-based comparison (handles duplicates and order).
            - `"jaccard"`: Jaccard similarity between token sets.
            - Or a custom `Callable[[str, str], float]`.
        mode: Matching mode. 
            - `"full"`: Matches the entire candidate string.
            - `"partial"`: Finds the best substring match.
        process: If True, applies normalization (lowercasing, punctuation removal) 
            before matching.
        threshold: Minimum score (0.0 to 1.0) for a candidate to be included in 
            the results.
        top_n: Maximum number of results to return. Use -1 for all matches.
        weights: Dictionary of weights for the `"hybrid"` scorer.

    Returns:
        A list of tuples containing (matched_string, similarity_score), 
        sorted by score in descending order.

    Examples:
        >>> import fuzzybunny
        >>> fuzzybunny.rank("apple", ["apple pie", "banana", "apricot"])
        [('apple pie', 0.5555555555555556), ('apricot', 0.42857142857142855)]

        >>> # Partial matching
        >>> fuzzybunny.rank("apple", ["apple pie"], mode="partial")
        [('apple pie', 1.0)]
    """
    if weights is None:
        weights = {}

    # Check for pandas/numpy
    if _is_pandas_series(candidates):
        candidates = candidates.astype(str).tolist()
    elif _is_numpy_array(candidates):
        import numpy as np
        candidates = np.array(candidates).astype(str).tolist()
    elif not isinstance(candidates, (list, tuple)):
        candidates = list(candidates)

    return _fuzzybunny.rank(query, candidates, scorer, mode, process, threshold, top_n, weights)

def batch_match(
    queries: QueriesType, 
    candidates: CandidatesType, 
    scorer: Union[str, Callable[[str, str], float]] = "levenshtein", 
    mode: str = "full", 
    process: bool = True, 
    threshold: float = 0.0, 
    top_n: int = -1, 
    weights: Dict[str, float] = None
) -> List[List[Tuple[str, float]]]:
    """
    Efficiently matches multiple queries against a collection of candidates.

    Utilizes multi-threading (OpenMP) and internal string normalization caching
    to provide high-performance batch processing.

    Args:
        queries: A collection of strings to match.
        candidates: A collection of target strings to search through.
        scorer: See `rank` for available options.
        mode: See `rank`.
        process: See `rank`.
        threshold: See `rank`.
        top_n: Maximum number of results per query.
        weights: See `rank`.

    Returns:
        A list of result lists, where each inner list corresponds to a query.

    Note:
        This function is significantly faster than calling `rank` in a loop
        for large datasets due to parallelization and reduced overhead.
    """
    if weights is None:
        weights = {}

    if _is_pandas_series(candidates):
        candidates = candidates.astype(str).tolist()
    elif _is_numpy_array(candidates):
        import numpy as np
        candidates = np.array(candidates).astype(str).tolist()
    elif not isinstance(candidates, (list, tuple)):
        candidates = list(candidates)

    # queries can also be pandas/numpy
    if _is_pandas_series(queries) or _is_numpy_array(queries):
        import numpy as np
        queries = np.array(queries).astype(str).tolist()
    elif not isinstance(queries, (list, tuple)):
        queries = list(queries)

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