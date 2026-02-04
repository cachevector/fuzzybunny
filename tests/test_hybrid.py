import pytest
import fuzzybunny

def test_hybrid_scorer():
    # Setup: "apple" is the query
    # "apple pie" matches partially well
    # "banana" matches poorly
    
    candidates = ["apple pie", "banana"]
    
    # Weight only levenshtein
    res_lev = fuzzybunny.rank("apple", candidates, scorer="hybrid", 
                              weights={"levenshtein": 1.0})
    # Should be equivalent to standard levenshtein
    expected_lev = fuzzybunny.rank("apple", candidates, scorer="levenshtein")
    assert abs(res_lev[0][1] - expected_lev[0][1]) < 0.001

    # Hybrid: 50% Levenshtein, 50% Token Sort
    # "apple pie" vs "apple"
    # Levenshtein: ratio is low (length diff)
    # Token Sort: "apple" vs "apple", "pie" -> ratio slightly better or same?
    
    # Let's test with a clearer case
    # s1="apple banana", s2="banana apple"
    # Levenshtein low (~0.5), Token Sort high (1.0)
    
    q = "apple banana"
    c = ["banana apple"]
    
    score_lev = fuzzybunny.levenshtein(q, c[0])
    score_ts = fuzzybunny.token_sort(q, c[0])
    
    res_hybrid = fuzzybunny.rank(q, c, scorer="hybrid", 
                                 weights={"levenshtein": 0.5, "token_sort": 0.5})
    
    expected_score = (score_lev * 0.5 + score_ts * 0.5)
    assert abs(res_hybrid[0][1] - expected_score) < 0.001

def test_unknown_scorer_error():
    with pytest.raises(ValueError, match="Unknown scorer"):
        fuzzybunny.rank("a", ["b"], scorer="non_existent_scorer")

def test_hybrid_empty_weights():
    # If weights are empty or total weight is 0, score should be 0
    res = fuzzybunny.rank("apple", ["apple"], scorer="hybrid", weights={})
    # threshold 0.0 (default), score 0.0. 0.0 >= 0.0 is True.
    assert len(res) == 1
    assert res[0][1] == 0.0
