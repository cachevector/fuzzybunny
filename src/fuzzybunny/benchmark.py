import time
import statistics

def benchmark(query, candidates, scorers=None, n_runs=5):
    """
    Benchmark different scorers on a given query and set of candidates.
    Returns a dictionary with timing results.
    """
    from . import rank
    if scorers is None:
        scorers = ["levenshtein", "jaccard", "token_sort"]
    
    results = {}
    
    for scorer in scorers:
        times = []
        for _ in range(n_runs):
            start = time.perf_counter()
            rank(query, candidates, scorer=scorer)
            end = time.perf_counter()
            times.append(end - start)
        
        results[scorer] = {
            "mean": statistics.mean(times),
            "stddev": statistics.stdev(times) if len(times) > 1 else 0,
            "min": min(times),
            "max": max(times)
        }
        
    return results

def benchmark_batch(queries, candidates, scorer="levenshtein", n_runs=3):
    """
    Benchmark batch_match performance.
    """
    from . import batch_match
    times = []
    for _ in range(n_runs):
        start = time.perf_counter()
        batch_match(queries, candidates, scorer=scorer)
        end = time.perf_counter()
        times.append(end - start)
    
    return {
        "mean": statistics.mean(times),
        "total_queries": len(queries),
        "total_candidates": len(candidates),
        "queries_per_second": len(queries) / statistics.mean(times)
    }
