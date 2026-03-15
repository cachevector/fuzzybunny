import pytest
import fuzzybunny

def test_myers_correctness_short():
    # Should use bit-parallel
    assert fuzzybunny.levenshtein("kitten", "sitting") == pytest.approx(1.0 - 3/7)
    assert fuzzybunny.levenshtein("gumbo", "gambol") == pytest.approx(1.0 - 2/6)

def test_myers_correctness_long():
    # Should use DP fallback (length > 64)
    s1 = "a" * 65
    s2 = "a" * 64 + "b"
    # dist is 1, max_len is 65
    assert fuzzybunny.levenshtein(s1, s2) == pytest.approx(1.0 - 1/65)

def test_myers_correctness_at_limit():
    # Exactly 64 chars
    s1 = "a" * 64
    s2 = "a" * 63 + "b"
    assert fuzzybunny.levenshtein(s1, s2) == pytest.approx(1.0 - 1/64)

def test_myers_unicode():
    # Unicode bitmasks
    assert fuzzybunny.levenshtein("😊" * 10, "😊" * 9 + "😢") == pytest.approx(0.9)
    assert fuzzybunny.levenshtein("你好世界", "你好") == pytest.approx(0.5)

def test_myers_vs_dp_consistency():
    # Test a variety of lengths to ensure bit-parallel and DP match results
    for i in range(60, 70):
        s1 = "x" * i
        s2 = "x" * (i-1) + "y"
        # The result should be consistent regardless of which algorithm is used
        assert fuzzybunny.levenshtein(s1, s2) == pytest.approx(1.0 - 1/i)

def test_very_long_strings():
    s1 = "abc" * 100 # 300 chars
    s2 = "abd" * 100 # 300 chars
    # Each 'abc' vs 'abd' is 1 edit
    # Total dist 100, max_len 300
    assert fuzzybunny.levenshtein(s1, s2) == pytest.approx(1.0 - 100/300)
