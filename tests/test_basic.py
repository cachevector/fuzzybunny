import pytest
import fuzzybunny

def test_levenshtein():
    assert fuzzybunny.levenshtein("kitten", "sitting") > 0.5
    assert fuzzybunny.levenshtein("apple", "apple") == 1.0
    assert fuzzybunny.levenshtein("abc", "def") == 0.0
    assert fuzzybunny.levenshtein("", "") == 1.0

def test_partial():
    assert fuzzybunny.partial_ratio("apple", "apple pie") == 1.0
    assert fuzzybunny.partial_ratio("pie", "apple pie") == 1.0
    assert fuzzybunny.partial_ratio("xyz", "apple pie") == 0.0
    
    cands = ["apple pie", "banana split", "cherry tart"]
    results = fuzzybunny.rank("apple", cands, scorer="levenshtein", mode="partial")
    assert results[0][0] == "apple pie"
    assert results[0][1] == 1.0

def test_processing():
    # With process=True (default), case and punctuation should not matter
    assert fuzzybunny.rank("APPLE!", ["apple"], process=True)[0][1] == 1.0
    assert fuzzybunny.rank("apple", ["Apple"], process=True)[0][1] == 1.0
    
    # With process=False, they should matter
    res = fuzzybunny.rank("APPLE", ["apple"], process=False)
    assert res[0][1] < 1.0

def test_jaccard():
    assert fuzzybunny.jaccard("apple banana", "banana apple") == 1.0
    assert fuzzybunny.jaccard("apple", "banana") == 0.0
    assert fuzzybunny.jaccard("a b c", "a c") > 0.5

def test_token_sort():
    assert fuzzybunny.token_sort("apple banana", "banana apple") == 1.0
    assert fuzzybunny.token_sort("fuzzy bunny", "bunny fuzzy") == 1.0

def test_unicode():
    s1 = "café"
    s2 = "cafe"
    assert fuzzybunny.levenshtein(s1, s2) < 1.0
    assert fuzzybunny.levenshtein(s1, s1) == 1.0
    assert fuzzybunny.levenshtein("😊", "😊") == 1.0
    
    # Test normalization of international characters
    res = fuzzybunny.rank("CAFÉ", ["café"], process=True)
    assert res[0][1] == 1.0

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
    assert results[0][0][0] == "apple pie"
    assert results[1][0][0] == "banana bread"

def test_invalid_scorer_rank():
    with pytest.raises(ValueError, match="Unknown scorer"):
        fuzzybunny.rank("a", ["a"], scorer="unknown")