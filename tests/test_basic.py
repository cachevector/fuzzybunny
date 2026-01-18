import pytest
import fuzzybunny

def test_levenshtein():
    assert fuzzybunny.levenshtein("kitten", "sitting") > 0.5
    assert fuzzybunny.levenshtein("apple", "apple") == 1.0
    assert fuzzybunny.levenshtein("abc", "def") == 0.0  # Actually 0.0 because len=3, dist=3, 1 - 3/3 = 0
    assert fuzzybunny.levenshtein("", "") == 1.0

def test_partial():
    # "apple" is inside "apple pie"
    assert fuzzybunny.partial_ratio("apple", "apple pie") == 1.0
    # "pie" is inside "apple pie"
    assert fuzzybunny.partial_ratio("pie", "apple pie") == 1.0
    # "xyz" is not in "apple pie"
    assert fuzzybunny.partial_ratio("xyz", "apple pie") == 0.0
    
    # rank with mode='partial'
    cands = ["apple pie", "banana split", "cherry tart"]
    results = fuzzybunny.rank("apple", cands, scorer="levenshtein", mode="partial")
    assert results[0][0] == "apple pie"
    assert results[0][1] == 1.0

def test_jaccard():
    assert fuzzybunny.jaccard("apple banana", "banana apple") == 1.0
    assert fuzzybunny.jaccard("apple", "banana") == 0.0
    assert fuzzybunny.jaccard("a b c", "a c") > 0.5

def test_token_sort():
    assert fuzzybunny.token_sort("apple banana", "banana apple") == 1.0
    assert fuzzybunny.token_sort("fuzzy bunny", "bunny fuzzy") == 1.0

def test_unicode():
    # 'café' vs 'cafe' (levenshtein should catch diff)
    s1 = "café"
    s2 = "cafe"
    assert fuzzybunny.levenshtein(s1, s2) < 1.0
    assert fuzzybunny.levenshtein(s1, s1) == 1.0
    
    # Emoji
    assert fuzzybunny.levenshtein("😊", "😊") == 1.0
    assert fuzzybunny.levenshtein("😊", "😂") < 1.0

def test_rank():
    candidates = ["apple", "apricot", "banana", "cherry"]
    results = fuzzybunny.rank("app", candidates, scorer="levenshtein", top_n=2)
    assert len(results) == 2
    assert results[0][0] == "apple"
    
    results_empty = fuzzybunny.rank("xyz", candidates, threshold=0.9)
    assert len(results_empty) == 0

def test_batch_match():
    queries = ["apple", "banana"]
    candidates = ["apple pie", "banana bread", "cherry tart"]
    results = fuzzybunny.batch_match(queries, candidates, mode="partial")
    
    assert len(results) == 2
    # First query "apple" matches "apple pie"
    assert results[0][0][0] == "apple pie"
    assert results[0][0][1] == 1.0
    
    # Second query "banana" matches "banana bread"
    assert results[1][0][0] == "banana bread"
    assert results[1][0][1] == 1.0

def test_invalid_scorer_rank():
    # Should default to 0.0 or handle gracefully
    results = fuzzybunny.rank("a", ["a"], scorer="unknown")
    assert results[0][1] == 0.0
