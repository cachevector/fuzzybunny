import fuzzybunny

def test_benchmark():
    query = "apple"
    candidates = ["apple", "banana", "cherry"]
    results = fuzzybunny.benchmark(query, candidates, n_runs=2)
    
    assert "levenshtein" in results
    assert "mean" in results["levenshtein"]
    assert results["levenshtein"]["mean"] > 0

def test_benchmark_batch():
    queries = ["apple", "banana"]
    candidates = ["apple pie", "banana bread"]
    results = fuzzybunny.benchmark_batch(queries, candidates, n_runs=2)
    
    assert "queries_per_second" in results
    assert results["total_queries"] == 2
