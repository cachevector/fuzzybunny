from typing import List, Tuple, Dict, Any, Union, Callable, Sequence

def levenshtein(s1: str, s2: str) -> float: ...
def partial_ratio(s1: str, s2: str) -> float: ...
def jaccard(s1: str, s2: str) -> float: ...
def token_sort(s1: str, s2: str) -> float: ...
def token_set(s1: str, s2: str) -> float: ...
def qratio(s1: str, s2: str) -> float: ...
def wratio(s1: str, s2: str) -> float: ...

def rank(
    query: str,
    candidates: Sequence[str],
    scorer: Union[str, Callable[[str, str], float]] = "levenshtein",
    mode: str = "full",
    process: bool = True,
    threshold: float = 0.0,
    top_n: int = -1,
    weights: Dict[str, float] = {}
) -> List[Tuple[str, float]]: ...

def batch_match(
    queries: Sequence[str],
    candidates: Sequence[str],
    scorer: Union[str, Callable[[str, str], float]] = "levenshtein",
    mode: str = "full",
    process: bool = True,
    threshold: float = 0.0,
    top_n: int = -1,
    weights: Dict[str, float] = {}
) -> List[List[Tuple[str, float]]]: ...

__version__: str
