import pytest
import fuzzybunny

def test_token_set_ratio():
    # Order and duplicates should not matter much
    assert fuzzybunny.token_set("fuzzy bunny", "bunny fuzzy") == 1.0
    assert fuzzybunny.token_set("fuzzy bunny", "fuzzy bunny bunny") == 1.0
    assert fuzzybunny.token_set("apple banana", "banana apple cherry") > 0.8

def test_qratio():
    assert fuzzybunny.qratio("kitten", "sitting") == fuzzybunny.levenshtein("kitten", "sitting")

def test_wratio():
    # WRatio should be high for these
    assert fuzzybunny.wratio("fuzzy bunny", "fuzzy bunny!") > 0.95
    assert fuzzybunny.wratio("apple banana", "banana apple") == 1.0
    
    # Partial match scenario
    s1 = "apple"
    s2 = "there is an apple here"
    # Levenshtein will be low, but WRatio should catch the partial match
    assert fuzzybunny.wratio(s1, s2) > 0.7
    assert fuzzybunny.wratio(s1, s2) > fuzzybunny.levenshtein(s1, s2)

def test_rank_with_new_scorers():
    candidates = ["apple pie", "banana bread", "cherry tart"]
    
    res_set = fuzzybunny.rank("apple apple pie", candidates, scorer="token_set")
    assert res_set[0][0] == "apple pie"
    assert res_set[0][1] == 1.0
    
    res_w = fuzzybunny.rank("apple", candidates, scorer="wratio")
    assert res_w[0][0] == "apple pie"
    assert res_w[0][1] > 0.8
