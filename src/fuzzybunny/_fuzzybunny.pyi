from typing import List, Tuple, Dict, Any

def levenshtein(s1: str, s2: str) -> float: ...
def partial_ratio(s1: str, s2: str) -> float: ...
def jaccard(s1: str, s2: str) -> float: ...
def token_sort(s1: str, s2: str) -> float: ...

def rank(
    query: str,
    candidates: List[str],
    scorer: str = "levenshtein",
    mode: str = "full",
    process: bool = True,
    threshold: float = 0.0,
    top_n: int = -1,
    weights: Dict[str, float] = {}
) -> List[Tuple[str, float]]: ...

def batch_match(
    queries: List[str],
    candidates: List[str],
    scorer: str = "levenshtein",
    mode: str = "full",
    process: bool = True,
    threshold: float = 0.0,
    top_n: int = -1,
    weights: Dict[str, float] = {}
) -> List[List[Tuple[str, float]]]: ...

__version__: str
