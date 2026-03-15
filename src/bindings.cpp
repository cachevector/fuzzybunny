#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "scorers.hpp"

namespace py = pybind11;
using namespace fuzzybunny;

PYBIND11_MODULE(_fuzzybunny, m) {
    m.doc() = R"pbdoc(
        FuzzyBunny: A fast fuzzy string matching library
        ------------------------------------------------
        .. currentmodule:: fuzzybunny
        .. autosummary::
           :toctree: _generate
           levenshtein
           jaccard
           token_sort
           rank
    )pbdoc";

    m.def("levenshtein", [](const std::string& s1, const std::string& s2) {
        return levenshtein_ratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        Calculate the Levenshtein similarity ratio between two strings.
        
        Returns a score between 0.0 and 1.0, where 1.0 is an exact match.
        The ratio is calculated as: 1 - (distance / max_length).
    )pbdoc");

    m.def("partial_ratio", [](const std::string& s1, const std::string& s2) {
        return partial_ratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        Calculate the best substring similarity ratio.
        
        If the shorter string has length k, this finds the best Levenshtein 
        ratio between the shorter string and any substring of length k 
        in the longer string.
    )pbdoc");

    m.def("jaccard", [](const std::string& s1, const std::string& s2) {
        return jaccard_similarity(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        Calculate Jaccard similarity between token sets.
        
        Tokenizes both strings and calculates the intersection over union 
        of the unique tokens.
    )pbdoc");

    m.def("token_sort", [](const std::string& s1, const std::string& s2) {
        return token_sort_ratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        Calculate similarity ratio after sorting tokens.
        
        Tokenizes both strings, sorts the tokens alphabetically, joins them 
        back with spaces, and then calculates the Levenshtein ratio.
    )pbdoc");

    m.def("token_set", [](const std::string& s1, const std::string& s2) {
        return token_set_ratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        Calculate similarity ratio while ignoring duplicates and token order.
        
        Finds the intersection and differences between token sets and 
        compares them to find the best possible match.
    )pbdoc");

    m.def("qratio", [](const std::string& s1, const std::string& s2) {
        return qratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        A simple Levenshtein ratio matching the behavior of other fuzzy libs.
    )pbdoc");

    m.def("wratio", [](const std::string& s1, const std::string& s2) {
        return wratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), R"pbdoc(
        Weighted similarity ratio (recommended for general use).
        
        Combines Levenshtein, partial ratio, and token-based ratios using 
        heuristics to provide the most 'intuitive' similarity score.
    )pbdoc");

    m.def("rank", &rank,
          py::arg("query"),
          py::arg("candidates"),
          py::arg("scorer") = py::str("levenshtein"),
          py::arg("mode") = "full",
          py::arg("process") = true,
          py::arg("threshold") = 0.0,
          py::arg("top_n") = -1,
          py::arg("weights") = std::map<std::string, double>{},
          "Rank candidates against a query string. Returns list of (string, score) tuples.");

    m.def("batch_match", &batch_match,
          py::arg("queries"),
          py::arg("candidates"),
          py::arg("scorer") = py::str("levenshtein"),
          py::arg("mode") = "full",
          py::arg("process") = true,
          py::arg("threshold") = 0.0,
          py::arg("top_n") = -1,
          py::arg("weights") = std::map<std::string, double>{},
          "Batch match multiple queries against candidates.");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
