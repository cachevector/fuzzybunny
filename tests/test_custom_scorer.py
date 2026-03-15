import pytest
import fuzzybunny

def test_custom_scorer_basic():
    def my_scorer(s1, s2):
        if s1 == s2:
            return 1.0
        return 0.0

    candidates = ["apple", "banana"]
    res = fuzzybunny.rank("apple", candidates, scorer=my_scorer)
    assert res[0][0] == "apple"
    assert res[0][1] == 1.0
    assert res[1][1] == 0.0

def test_custom_scorer_case_sensitive():
    def exact_case_scorer(s1, s2):
        return 1.0 if s1 == s2 else 0.0

    # With process=True (default), strings are normalized before being passed to custom scorer
    res = fuzzybunny.rank("APPLE", ["apple"], scorer=exact_case_scorer, process=True)
    assert res[0][1] == 1.0

    # With process=False, custom scorer gets raw strings
    res2 = fuzzybunny.rank("APPLE", ["apple"], scorer=exact_case_scorer, process=False)
    assert res2[0][1] == 0.0

def test_batch_custom_scorer():
    def length_diff_scorer(s1, s2):
        return 1.0 / (1.0 + abs(len(s1) - len(s2)))

    queries = ["a", "abc"]
    candidates = ["ab", "abcd", "abcdef"]
    
    # This tests the GIL acquisition in the parallel loop
    results = fuzzybunny.batch_match(queries, candidates, scorer=length_diff_scorer)
    
    assert len(results) == 2
    # "a" (len 1) vs "ab" (len 2) -> diff 1 -> 0.5
    assert results[0][0][1] == 0.5
