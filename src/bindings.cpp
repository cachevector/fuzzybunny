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
    }, py::arg("s1"), py::arg("s2"), "Calculate Levenshtein ratio (0.0 - 1.0)");

    m.def("partial_ratio", [](const std::string& s1, const std::string& s2) {
        return partial_ratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), "Calculate Partial Levenshtein ratio (0.0 - 1.0)");

    m.def("jaccard", [](const std::string& s1, const std::string& s2) {
        return jaccard_similarity(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), "Calculate Jaccard similarity (0.0 - 1.0)");

    m.def("token_sort", [](const std::string& s1, const std::string& s2) {
        return token_sort_ratio(utf8_to_u32(s1), utf8_to_u32(s2));
    }, py::arg("s1"), py::arg("s2"), "Calculate Token Sort ratio (0.0 - 1.0)");

    m.def("rank", &rank,
          py::arg("query"),
          py::arg("candidates"),
          py::arg("scorer") = "levenshtein",
          py::arg("mode") = "full",
          py::arg("process") = true,
          py::arg("threshold") = 0.0,
          py::arg("top_n") = -1,
          py::arg("weights") = std::map<std::string, double>{},
          "Rank candidates against a query string. Returns list of (string, score) tuples.");

    m.def("batch_match", &batch_match,
          py::arg("queries"),
          py::arg("candidates"),
          py::arg("scorer") = "levenshtein",
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
