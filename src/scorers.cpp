#include "scorers.hpp"
#include <algorithm>
#include <vector>
#include <set>
#include <sstream>
#include <cmath>
#include <map>
#include <unordered_map>
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

/**
 * Myers' Bit-Parallel Levenshtein algorithm.
 * Optimized for strings where shorter.size() <= 64.
 * Complexity: O(N * (M/W)) where W=64.
 */
size_t myers_levenshtein(const std::u32string& shorter, const std::u32string& longer) {
    size_t m = shorter.size();
    size_t n = longer.size();

    uint64_t pv = -1;
    uint64_t mv = 0;
    size_t dist = m;

    // Precompute character masks
    std::unordered_map<char32_t, uint64_t> peq;
    for (size_t i = 0; i < m; ++i) {
        peq[shorter[i]] |= (1ULL << i);
    }

    uint64_t last_bit = 1ULL << (m - 1);

    for (size_t j = 0; j < n; ++j) {
        uint64_t eq = peq[longer[j]];
        uint64_t xv = eq | mv;
        uint64_t xh = (((eq & pv) + pv) ^ pv) | eq | mv;

        uint64_t ph = mv | ~(xh | pv);
        uint64_t mh = pv & xh;

        if (ph & last_bit) dist++;
        if (mh & last_bit) dist--;

        ph = (ph << 1) | 1;
        mh = (mh << 1);

        pv = mh | ~(ph | xv);
        mv = ph & xv;
    }

    return dist;
}

double levenshtein_ratio(const std::u32string& s1, const std::u32string& s2) {
    size_t len1 = s1.size();
    size_t len2 = s2.size();

    if (len1 == 0 && len2 == 0) return 1.0;
    if (len1 == 0 || len2 == 0) return 0.0;

    const auto& shorter = (len1 <= len2) ? s1 : s2;
    const auto& longer = (len1 > len2) ? s1 : s2;
    
    size_t m = shorter.size();
    size_t n = longer.size();
    size_t dist;

    // Use Myers' bit-parallel algorithm if the shorter string fits in 64 bits
    if (m <= 64) {
        dist = myers_levenshtein(shorter, longer);
    } else {
        // Fallback to standard DP for very long strings
        std::vector<size_t> prev(m + 1);
        std::vector<size_t> curr(m + 1);

        for (size_t i = 0; i <= m; ++i) prev[i] = i;

        for (size_t j = 1; j <= n; ++j) {
            curr[0] = j;
            for (size_t i = 1; i <= m; ++i) {
                size_t cost = (shorter[i - 1] == longer[j - 1]) ? 0 : 1;
                curr[i] = std::min({
                    prev[i] + 1,       
                    curr[i - 1] + 1,   
                    prev[i - 1] + cost 
                });
            }
            prev = curr;
        }
        dist = prev[m];
    }

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
    
    if (t1.empty() && t2.empty()) return 1.0;
    if (t1.empty() || t2.empty()) return 0.0;

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

double token_set_ratio(const std::u32string& s1, const std::u32string& s2) {
    auto t1 = tokenize(s1);
    auto t2 = tokenize(s2);

    if (t1.empty() && t2.empty()) return 1.0;
    if (t1.empty() || t2.empty()) return 0.0;

    std::set<std::u32string> set1(t1.begin(), t1.end());
    std::set<std::u32string> set2(t2.begin(), t2.end());

    std::vector<std::u32string> intersection;
    std::set_intersection(set1.begin(), set1.end(),
                          set2.begin(), set2.end(),
                          std::back_inserter(intersection));

    std::vector<std::u32string> diff1to2;
    std::set_difference(set1.begin(), set1.end(),
                        set2.begin(), set2.end(),
                        std::back_inserter(diff1to2));

    std::vector<std::u32string> diff2to1;
    std::set_difference(set2.begin(), set2.end(),
                        set1.begin(), set1.end(),
                        std::back_inserter(diff2to1));

    // Common part
    std::u32string common;
    for (size_t i = 0; i < intersection.size(); ++i) {
        common += intersection[i];
        if (i < intersection.size() - 1) common += ' ';
    }

    // Common + diff1
    std::u32string s1_res = common;
    if (!common.empty() && !diff1to2.empty()) s1_res += ' ';
    for (size_t i = 0; i < diff1to2.size(); ++i) {
        s1_res += diff1to2[i];
        if (i < diff1to2.size() - 1) s1_res += ' ';
    }

    // Common + diff2
    std::u32string s2_res = common;
    if (!common.empty() && !diff2to1.empty()) s2_res += ' ';
    for (size_t i = 0; i < diff2to1.size(); ++i) {
        s2_res += diff2to1[i];
        if (i < diff2to1.size() - 1) s2_res += ' ';
    }

    double r1 = levenshtein_ratio(common, s1_res);
    double r2 = levenshtein_ratio(common, s2_res);
    double r3 = levenshtein_ratio(s1_res, s2_res);

    return std::max({r1, r2, r3});
}

double qratio(const std::u32string& s1, const std::u32string& s2) {
    // Basic Levenshtein ratio
    return levenshtein_ratio(s1, s2);
}

double wratio(const std::u32string& s1, const std::u32string& s2) {
    double end_ratio = levenshtein_ratio(s1, s2);
    
    double len_ratio = static_cast<double>(std::max(s1.size(), s2.size())) / 
                       (s2.empty() || s1.empty() ? 1.0 : static_cast<double>(std::min(s1.size(), s2.size())));

    // If there is a big difference in lengths, we should use partial ratio
    double partial_scale = 1.0;
    if (len_ratio > 1.5) {
        partial_scale = 0.9;
    }

    double partial = partial_ratio(s1, s2);
    end_ratio = std::max(end_ratio, partial * partial_scale);

    double token_sort = token_sort_ratio(s1, s2);
    double token_set = token_set_ratio(s1, s2);
    
    return std::max({end_ratio, token_sort, token_set});
}

// Helpers
std::string u32_to_utf8(const std::u32string& s) {
    std::string result;
    result.reserve(s.size());
    for (char32_t cp : s) {
        if (cp < 0x80) {
            result.push_back(static_cast<char>(cp));
        } else if (cp < 0x800) {
            result.push_back(static_cast<char>((cp >> 6) | 0xc0));
            result.push_back(static_cast<char>((cp & 0x3f) | 0x80));
        } else if (cp < 0x10000) {
            result.push_back(static_cast<char>((cp >> 12) | 0xe0));
            result.push_back(static_cast<char>(((cp >> 6) & 0x3f) | 0x80));
            result.push_back(static_cast<char>((cp & 0x3f) | 0x80));
        } else {
            result.push_back(static_cast<char>((cp >> 18) | 0xf0));
            result.push_back(static_cast<char>(((cp >> 12) & 0x3f) | 0x80));
            result.push_back(static_cast<char>(((cp >> 6) & 0x3f) | 0x80));
            result.push_back(static_cast<char>((cp & 0x3f) | 0x80));
        }
    }
    return result;
}

// Internal helper for ranking using pre-normalized strings
std::vector<MatchResult> rank_normalized(
    const std::u32string& uQuery,
    const std::vector<std::string>& candidates,
    const std::vector<std::u32string>& uCandidates,
    const std::string& scorer_name,
    const py::function& scorer_func,
    const std::string& mode,
    double threshold,
    int top_n,
    const std::map<std::string, double>& weights
) {
    std::vector<MatchResult> results;
    results.reserve(candidates.size());

    bool is_custom = scorer_func && !scorer_func.is_none();

    for (size_t i = 0; i < candidates.size(); ++i) {
        const auto& uCand = uCandidates[i];
        double score = 0.0;

        if (is_custom) {
            score = scorer_func(u32_to_utf8(uQuery), u32_to_utf8(uCand)).cast<double>();
        } else {
            if (scorer_name == "levenshtein") {
                if (mode == "partial") score = partial_ratio(uQuery, uCand);
                else score = levenshtein_ratio(uQuery, uCand);
            } else if (scorer_name == "jaccard") {
                score = jaccard_similarity(uQuery, uCand);
            } else if (scorer_name == "token_sort") {
                score = token_sort_ratio(uQuery, uCand);
            } else if (scorer_name == "token_set") {
                score = token_set_ratio(uQuery, uCand);
            } else if (scorer_name == "qratio") {
                score = qratio(uQuery, uCand);
            } else if (scorer_name == "wratio") {
                score = wratio(uQuery, uCand);
            } else if (scorer_name == "hybrid") {
                double weighted_sum = 0.0;
                double total_weight = 0.0;
                for (const auto& [name, weight] : weights) {
                    double sub_score = 0.0;
                    if (name == "levenshtein") {
                        if (mode == "partial") sub_score = partial_ratio(uQuery, uCand);
                        else sub_score = levenshtein_ratio(uQuery, uCand);
                    } else if (name == "jaccard") sub_score = jaccard_similarity(uQuery, uCand);
                    else if (name == "token_sort") sub_score = token_sort_ratio(uQuery, uCand);
                    else if (name == "token_set") sub_score = token_set_ratio(uQuery, uCand);
                    else if (name == "qratio") sub_score = qratio(uQuery, uCand);
                    else if (name == "wratio") sub_score = wratio(uQuery, uCand);
                    weighted_sum += sub_score * weight;
                    total_weight += weight;
                }
                if (total_weight > 0.0) score = weighted_sum / total_weight;
            } else {
                throw std::invalid_argument("Unknown scorer: " + scorer_name);
            }
        }

        if (score >= threshold) {
            results.push_back({candidates[i], score});
        }
    }

    std::sort(results.begin(), results.end(), [](const MatchResult& a, const MatchResult& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; 
    });

    if (top_n > 0 && static_cast<size_t>(top_n) < results.size()) {
        results.resize(top_n);
    }

    return results;
}

std::vector<MatchResult> rank(
    const std::string& query,
    const std::vector<std::string>& candidates,
    const py::object& scorer,
    const std::string& mode,
    bool process,
    double threshold,
    int top_n,
    const std::map<std::string, double>& weights
) {
    if (query.empty() || candidates.empty()) return {};

    std::string scorer_name;
    py::function scorer_func;

    if (py::isinstance<py::str>(scorer)) {
        scorer_name = scorer.cast<std::string>();
    } else if (py::isinstance<py::function>(scorer)) {
        scorer_func = scorer.cast<py::function>();
    } else {
        throw std::invalid_argument("Scorer must be a string or a callable");
    }

    std::u32string uQuery = utf8_to_u32(query);
    if (process) uQuery = normalize(uQuery);

    std::vector<std::u32string> uCandidates;
    uCandidates.reserve(candidates.size());
    for (const auto& cand : candidates) {
        std::u32string uCand = utf8_to_u32(cand);
        if (process) uCand = normalize(uCand);
        uCandidates.push_back(uCand);
    }

    if (!scorer_func) {
        py::gil_scoped_release release;
        return rank_normalized(uQuery, candidates, uCandidates, scorer_name, scorer_func, mode, threshold, top_n, weights);
    }
    
    return rank_normalized(uQuery, candidates, uCandidates, scorer_name, scorer_func, mode, threshold, top_n, weights);
}

std::vector<std::vector<MatchResult>> batch_match(
    const std::vector<std::string>& queries,
    const std::vector<std::string>& candidates,
    const py::object& scorer,
    const std::string& mode,
    bool process,
    double threshold,
    int top_n,
    const std::map<std::string, double>& weights
) {
    if (queries.empty() || candidates.empty()) return std::vector<std::vector<MatchResult>>(queries.size());

    std::string scorer_name;
    py::function scorer_func;

    if (py::isinstance<py::str>(scorer)) {
        scorer_name = scorer.cast<std::string>();
    } else if (py::isinstance<py::function>(scorer)) {
        scorer_func = scorer.cast<py::function>();
    } else {
        throw std::invalid_argument("Scorer must be a string or a callable");
    }

    // Pre-normalize
    std::vector<std::u32string> uCandidates;
    uCandidates.reserve(candidates.size());
    for (const auto& cand : candidates) {
        std::u32string uCand = utf8_to_u32(cand);
        if (process) uCand = normalize(uCand);
        uCandidates.push_back(uCand);
    }

    std::vector<std::u32string> uQueries;
    uQueries.reserve(queries.size());
    for (const auto& q : queries) {
        std::u32string uQ = utf8_to_u32(q);
        if (process) uQ = normalize(uQ);
        uQueries.push_back(uQ);
    }

    std::vector<std::vector<MatchResult>> batch_results(queries.size());

    if (!scorer_func) {
        py::gil_scoped_release release;
        #pragma omp parallel for if(queries.size() > 5)
        for (int i = 0; i < static_cast<int>(queries.size()); ++i) {
            batch_results[i] = rank_normalized(uQueries[i], candidates, uCandidates, scorer_name, scorer_func, mode, threshold, top_n, weights);
        }
    } else {
        // For custom scorers, we can't release the GIL easily for the whole parallel block
        // because each call needs the GIL. 
        // For simplicity and safety, if it's a custom scorer, we don't parallelize here
        // or we have to acquire/release GIL inside the loop.
        // Let's parallelize with GIL acquisition inside the loop.
        #pragma omp parallel for if(queries.size() > 5)
        for (int i = 0; i < static_cast<int>(queries.size()); ++i) {
            py::gil_scoped_acquire acquire;
            batch_results[i] = rank_normalized(uQueries[i], candidates, uCandidates, scorer_name, scorer_func, mode, threshold, top_n, weights);
        }
    }

    return batch_results;
}

} // namespace fuzzybunny