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
    Rank candidates against a query string. Supports List, NumPy arrays, and Pandas Series.
    """
    if weights is None:
        weights = {}

    # Check for pandas/numpy
    if _is_pandas_series(candidates):
        candidates = candidates.astype(str).tolist()
    elif _is_numpy_array(candidates):
        import numpy as np
        candidates = np.array(candidates).astype(str).tolist()
    elif not isinstance(candidates, list):
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
    Batch match multiple queries against candidates. Supports List, NumPy arrays, and Pandas Series.
    """
    if weights is None:
        weights = {}

    if _is_pandas_series(candidates):
        candidates = candidates.astype(str).tolist()
    elif _is_numpy_array(candidates):
        import numpy as np
        candidates = np.array(candidates).astype(str).tolist()
    elif not isinstance(candidates, list):
        candidates = list(candidates)

    # queries can also be pandas/numpy
    if _is_pandas_series(queries) or _is_numpy_array(queries):
        import numpy as np
        queries = np.array(queries).astype(str).tolist()
    elif not isinstance(queries, list):
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