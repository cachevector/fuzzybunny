#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace fuzzybunny {

// Core Scorers
double levenshtein_ratio(const std::u32string& s1, const std::u32string& s2);
double partial_ratio(const std::u32string& s1, const std::u32string& s2);
double jaccard_similarity(const std::u32string& s1, const std::u32string& s2);
double token_sort_ratio(const std::u32string& s1, const std::u32string& s2);

// Ranking
using MatchResult = std::pair<std::string, double>;

std::vector<MatchResult> rank(
    const std::string& query,
    const std::vector<std::string>& candidates,
    const std::string& scorer = "levenshtein",
    const std::string& mode = "full",
    bool process = true,
    double threshold = 0.0,
    int top_n = -1,
    const std::map<std::string, double>& weights = {}
);

std::vector<std::vector<MatchResult>> batch_match(
    const std::vector<std::string>& queries,
    const std::vector<std::string>& candidates,
    const std::string& scorer = "levenshtein",
    const std::string& mode = "full",
    bool process = true,
    double threshold = 0.0,
    int top_n = -1,
    const std::map<std::string, double>& weights = {}
);

// Helpers
std::u32string utf8_to_u32(const std::string& s);

} // namespace fuzzybunny
