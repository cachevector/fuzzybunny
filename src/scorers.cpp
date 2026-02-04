#include "scorers.hpp"
#include <algorithm>
#include <vector>
#include <set>
#include <sstream>
#include <cmath>
#include <map>
#include <iostream>
#include <cstdint>
#include <cwctype>

namespace fuzzybunny {

// --- Unicode Helper ---

std::u32string normalize(const std::u32string& s) {
    std::u32string result;
    result.reserve(s.size());
    for (char32_t c : s) {
        // Lowercase and remove punctuation
        // Supporting basic Latin-1 range for better international support
        if (c < 128) {
            if (std::iswalnum(static_cast<wint_t>(c)) || std::iswspace(static_cast<wint_t>(c))) {
                result.push_back(static_cast<char32_t>(std::towlower(static_cast<wint_t>(c))));
            }
        } else if (c < 256) {
            // Latin-1 Supplement
            if (std::iswalpha(static_cast<wint_t>(c)) || std::iswspace(static_cast<wint_t>(c))) {
                result.push_back(static_cast<char32_t>(std::towlower(static_cast<wint_t>(c))));
            }
        } else {
            // For other non-ASCII, just pass through
            result.push_back(c);
        }
    }
    return result;
}

// Converting manually because std::codecvt is deprecated in C++17
// and we want to avoid external dependencies like ICU for this lightweight lib.
std::u32string utf8_to_u32(const std::string& s) {
    std::u32string result;
    result.reserve(s.size()); 

    for (size_t i = 0; i < s.length(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        uint32_t code_point = 0;
        int seq_len = 0;

        if (c < 0x80) {
            code_point = c;
            seq_len = 1;
        } else if ((c & 0xE0) == 0xC0) {
            code_point = c & 0x1F;
            seq_len = 2;
        } else if ((c & 0xF0) == 0xE0) {
            code_point = c & 0x0F;
            seq_len = 3;
        } else if ((c & 0xF8) == 0xF0) {
            code_point = c & 0x07;
            seq_len = 4;
        } else {
            // Skip invalid start bytes to prevent decoding errors
            i++;
            continue;
        }

        if (i + seq_len > s.length()) break; 

        bool valid = true;
        for (int k = 1; k < seq_len; ++k) {
            unsigned char next = static_cast<unsigned char>(s[i + k]);
            if ((next & 0xC0) != 0x80) {
                valid = false;
                break;
            }
            code_point = (code_point << 6) | (next & 0x3F);
        }

        if (valid) {
            result.push_back(code_point);
            i += seq_len;
        } else {
            i++; 
        }
    }
    return result;
}

// --- Internal Utils ---

std::vector<std::u32string> tokenize(const std::u32string& s) {
    std::vector<std::u32string> tokens;
    std::u32string current;
    for (char32_t c : s) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) tokens.push_back(current);
    return tokens;
}

// --- Scorers ---

double levenshtein_ratio(const std::u32string& s1, const std::u32string& s2) {
    size_t len1 = s1.size();
    size_t len2 = s2.size();

    if (len1 == 0 && len2 == 0) return 1.0;
    if (len1 == 0 || len2 == 0) return 0.0;

    std::vector<size_t> prev(len2 + 1);
    std::vector<size_t> curr(len2 + 1);

    for (size_t j = 0; j <= len2; ++j) prev[j] = j;

    for (size_t i = 1; i <= len1; ++i) {
        curr[0] = i;
        for (size_t j = 1; j <= len2; ++j) {
            size_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            curr[j] = std::min({
                prev[j] + 1,       
                curr[j - 1] + 1,   
                prev[j - 1] + cost 
            });
        }
        prev = curr;
    }

    size_t dist = prev[len2];
    size_t max_len = std::max(len1, len2);
    
    return 1.0 - (static_cast<double>(dist) / static_cast<double>(max_len));
}

double partial_ratio(const std::u32string& s1, const std::u32string& s2) {
    if (s1.empty() && s2.empty()) return 1.0;
    if (s1.empty() || s2.empty()) return 0.0;

    const auto& shorter = (s1.size() <= s2.size()) ? s1 : s2;
    const auto& longer = (s1.size() > s2.size()) ? s1 : s2;

    double max_ratio = 0.0;
    size_t k = shorter.size();
    
    // Sliding window over the longer string to find the best matching substring
    for (size_t i = 0; i <= longer.size() - k; ++i) {
        std::u32string sub = longer.substr(i, k);
        double ratio = levenshtein_ratio(shorter, sub);
        if (ratio > max_ratio) max_ratio = ratio;
    }
    return max_ratio;
}

double jaccard_similarity(const std::u32string& s1, const std::u32string& s2) {
    std::vector<std::u32string> tokens1 = tokenize(s1);
    std::vector<std::u32string> tokens2 = tokenize(s2);

    if (tokens1.empty() && tokens2.empty()) return 1.0;
    if (tokens1.empty() || tokens2.empty()) return 0.0;

    std::set<std::u32string> set1(tokens1.begin(), tokens1.end());
    std::set<std::u32string> set2(tokens2.begin(), tokens2.end());

    std::vector<std::u32string> intersection;
    std::set_intersection(set1.begin(), set1.end(),
                          set2.begin(), set2.end(),
                          std::back_inserter(intersection));

    std::vector<std::u32string> union_set;
    std::set_union(set1.begin(), set1.end(),
                   set2.begin(), set2.end(),
                   std::back_inserter(union_set));

    if (union_set.empty()) return 0.0;
    return static_cast<double>(intersection.size()) / static_cast<double>(union_set.size());
}

double token_sort_ratio(const std::u32string& s1, const std::u32string& s2) {
    auto t1 = tokenize(s1);
    auto t2 = tokenize(s2);
    
    std::sort(t1.begin(), t1.end());
    std::sort(t2.begin(), t2.end());

    std::u32string joined1, joined2;
    for (size_t i = 0; i < t1.size(); ++i) {
        joined1 += t1[i];
        if (i < t1.size() - 1) joined1 += ' ';
    }
    for (size_t i = 0; i < t2.size(); ++i) {
        joined2 += t2[i];
        if (i < t2.size() - 1) joined2 += ' ';
    }

    return levenshtein_ratio(joined1, joined2);
}

// --- Ranking ---

std::vector<MatchResult> rank(
    const std::string& query,
    const std::vector<std::string>& candidates,
    const std::string& scorer,
    const std::string& mode,
    bool process,
    double threshold,
    int top_n,
    const std::map<std::string, double>& weights
) {
    if (query.empty() || candidates.empty()) return {};

    std::u32string uQuery = utf8_to_u32(query);
    if (process) uQuery = normalize(uQuery);

    std::vector<MatchResult> results;
    results.reserve(candidates.size());

    for (const auto& cand : candidates) {
        std::u32string uCand = utf8_to_u32(cand);
        if (process) uCand = normalize(uCand);

        double score = 0.0;

        if (scorer == "levenshtein") {
            if (mode == "partial") {
                score = partial_ratio(uQuery, uCand);
            } else {
                score = levenshtein_ratio(uQuery, uCand);
            }
        } else if (scorer == "jaccard") {
             // Jaccard is inherently set-based, so partial matching on substrings
             // doesn't align with the standard definition. 
            score = jaccard_similarity(uQuery, uCand);
        } else if (scorer == "token_sort") {
            score = token_sort_ratio(uQuery, uCand);
        } else if (scorer == "hybrid") {
            double weighted_sum = 0.0;
            double total_weight = 0.0;

            for (const auto& [name, weight] : weights) {
                double sub_score = 0.0;
                if (name == "levenshtein") {
                     if (mode == "partial") sub_score = partial_ratio(uQuery, uCand);
                     else sub_score = levenshtein_ratio(uQuery, uCand);
                } else if (name == "jaccard") {
                    sub_score = jaccard_similarity(uQuery, uCand);
                } else if (name == "token_sort") {
                    sub_score = token_sort_ratio(uQuery, uCand);
                }
                // Ignore unknown scorers in weights for now, or could throw.
                
                weighted_sum += sub_score * weight;
                total_weight += weight;
            }

            if (total_weight > 0.0) {
                score = weighted_sum / total_weight;
            } else {
                score = 0.0;
            }
        } else {
             throw std::invalid_argument("Unknown scorer: " + scorer);
        }

        if (score >= threshold) {
            results.push_back({cand, score});
        }
    }

    std::sort(results.begin(), results.end(), [](const MatchResult& a, const MatchResult& b) {
        return a.second > b.second;
    });

    if (top_n > 0 && static_cast<size_t>(top_n) < results.size()) {
        results.resize(top_n);
    }

    return results;
}

std::vector<std::vector<MatchResult>> batch_match(
    const std::vector<std::string>& queries,
    const std::vector<std::string>& candidates,
    const std::string& scorer,
    const std::string& mode,
    bool process,
    double threshold,
    int top_n,
    const std::map<std::string, double>& weights
) {
    std::vector<std::vector<MatchResult>> batch_results(queries.size());

    #pragma omp parallel for if(queries.size() > 10)
    for (int i = 0; i < static_cast<int>(queries.size()); ++i) {
        batch_results[i] = rank(queries[i], candidates, scorer, mode, process, threshold, top_n, weights);
    }
    return batch_results;
}

} // namespace fuzzybunny