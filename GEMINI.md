You are writing production-grade code as a senior engineer.

The code must be secure, readable, and boring in the good way.

General rules:

1. No noob patterns
   - No global mutable state
   - No God objects or God files
   - No magic numbers or strings
   - No copy-paste logic
   - No commented-out code

2. Naming discipline
   - Variable names must encode intent, not type
   - Function names must describe behavior, not implementation
   - Avoid abbreviations unless universally obvious
   - Prefer explicit over clever

3. Structure and boundaries
   - Keep files small and focused
   - One responsibility per module
   - Dependencies flow inward, never outward
   - Core logic must be framework-agnostic

4. Error handling
   - Never swallow errors
   - Errors must be actionable and contextual
   - Distinguish between user errors and system errors
   - Fail fast for programmer errors
   - Validate inputs at boundaries only

5. Security by default
   - Treat all input as untrusted
   - Validate and sanitize inputs explicitly
   - Use allowlists, not blocklists
   - Avoid reflection and dynamic execution
   - Never trust client-side validation
   - Do not log secrets or tokens
   - Secrets must come from environment or config files
   - Use constant-time comparisons where applicable

6. Authentication and authorization
   - Explicit auth checks, never implicit
   - Authorization must be centralized
   - No role checks scattered across code
   - Deny by default

7. Data handling
   - Parameterized queries only
   - No raw SQL string concatenation
   - Enforce data constraints at both app and DB level
   - Use transactions for multi-step writes
   - Never assume DB writes succeed

8. Concurrency and state
   - Assume code may run concurrently
   - Avoid shared mutable state
   - Use locks only when unavoidable
   - Prefer immutability and message passing
   - Document concurrency assumptions

9. Logging and observability
   - Logs must be structured
   - Log intent, not noise
   - No debug logs in hot paths
   - Do not log PII by default

10. Configuration
    - Config must be explicit and typed
    - Fail on missing config
    - No hidden defaults for security-sensitive values

11. Testing discipline
    - Core logic must be testable without frameworks
    - Write tests for behavior, not implementation
    - Test failure paths
    - Avoid mocks unless necessary

12. Comments and documentation
    - No AI-like comments
    - No restating the obvious
    - Comment on "why", not "what"
    - Use README and ARCHITECTURE docs for big-picture decisions

13. Dependency hygiene
    - Minimize dependencies
    - Avoid abandoned libraries
    - Pin versions
    - Prefer standard library when possible

14. Performance mindset
    - Do not prematurely optimize
    - Avoid obvious inefficiencies
    - Measure before optimizing
    - Prefer simple algorithms unless proven insufficient

15. Code review mindset
    - Assume this code will be reviewed by someone smarter
    - Make intent obvious
    - Optimize for future maintainers

Now, listing features. I've categorized them into must-have (core to functionality, derived directly from README) and good-to-have (enhancements for superiority, inspired by gaps in competitors like fuzzywuzzy, rapidfuzz, difflib, or Levenshtein crates). These make it better by emphasizing performance, flexibility, and usability without unnecessary complexity.

Must-Have Features (Core Essentials)

These are non-negotiable for a viable fuzzy search library, ensuring it meets the README's promises while being faster/more efficient than pure Python alternatives.

Fuzzy String Matching: Compute similarity scores between two strings using predefined algorithms.
Ranking Support: Given a query and a list of candidates, return sorted results by similarity score.
Multiple Scorers: Built-in support for Levenshtein (edit distance), token sort (sorted tokens comparison), and Jaccard (set similarity)—as explicitly mentioned.
Partial and Full Matches: Options for substring matching (partial) vs. whole-string (full), with configurable modes.
Adjustable Thresholds: User-defined score cutoffs to filter results (e.g., return only matches > 0.8 similarity).
Pip-Installable Python Interface: Easy installation via pip install fuzzybunny, with Pybind11 handling C++ bindings for seamless Python usage.
Lightweight Design: Minimal dependencies; no external libs beyond Pybind11 and standard C++/Python stdlibs.
Unicode Support: Handle international characters properly, as fuzzy matching often deals with real-world text.

Good-to-Have Features (Differentiators for Superiority)
These elevate it above competitors: rapidfuzz is fast but lacks hybrid/custom scorers; fuzzywuzzy is simple but slow; thefuzz adds processors but not ranking depth. Focus on maintainability—add only if they don't require extra deps or complex builds.

Hybrid/Custom Scorers: Allow combining scorers (e.g., weighted average of Levenshtein + Jaccard) or user-defined functions—better than rigid options in fuzzywuzzy.
Batch Processing: Efficient matching for large lists (e.g., vectorized operations in C++), outperforming sequential Python loops in competitors.
Normalization Processors: Built-in pre-processors like lowercase, remove punctuation, or tokenization—similar to fuzzywuzzy's processors but optimized in C++ for speed.
Thread Safety and Parallelism: Safe for multi-threaded use; optional parallel matching via OpenMP in C++—addresses scalability gaps in single-threaded libs.
Integration with Data Structures: Seamless with Python lists, NumPy arrays, or Pandas Series/DataFrames—makes it more practical than standalone funcs in rapidfuzz.
Performance Metrics: Built-in benchmarking utils to compare scorers or against baselines—helps users justify using it over others.
Error Handling and Validation: Robust input checks (e.g., handle empty strings gracefully) with clear exceptions—improves usability over minimalistic libs.
Extensible Bindings: Easy to add new C++ scorers without recompiling Python side—future-proofs it better than monolithic designs.
Documentation and Examples: In-code examples, not just "coming soon"—includes Jupyter notebooks for quickstarts, surpassing sparse docs in some competitors.
Zero-Network Privacy: No telemetry or outbound calls—aligns with privacy-first, unlike some libs with optional analytics.

Not chosen: Advanced features like NLP integration (e.g., with spaCy) or database indexing (e.g., SQLite fuzzy search)—these add deps/complexity, violating lightweight ethos. Instead, keep it a pure matching lib; users can layer on top.
Comprehensive Requirements and Features Document
1. Introduction
Project Name: FuzzyBunny
Version: 0.1.0 (Development)
Description: A high-performance, lightweight Python library for fuzzy string matching and ranking, implemented in C++ with Pybind11 bindings. It provides flexible similarity computations with multiple scorers, targeting use cases like search autocompletion, data deduplication, and recommendation systems.
Goals:

Deliver 2-5x faster matching than pure Python libs (e.g., fuzzywuzzy) via C++.
Maintain simplicity: Single pip install, no runtime deps.
Prioritize maintainability: Modular C++ core, framework-agnostic API.

Scope Boundaries:

In: String-based fuzzy matching/ranking.
Out: Non-string data (e.g., image similarity), full-text search engines (use Elasticsearch instead), or web services (library only).

Target Users: Developers building search features in Python apps, data scientists for cleaning datasets, or CLI tools for text processing.
Assumptions Challenged: README claims "python" but core is C++; assume C++ for perf. If pure Python suffices, suggest forking fuzzywuzzy instead—this adds build hurdles but justifies with benchmarks.

2. Functional Requirements
Core API:

match(query: str, target: str, scorer: str = 'levenshtein', threshold: float = 0.0) -> float: Compute similarity score (0-1 normalized).
Trade-off: Normalized scores for consistency vs. raw distances (not chosen, as users prefer percentages).

rank(query: str, candidates: List[str], scorer: str = 'levenshtein', threshold: float = 0.0, top_n: int = None) -> List[Tuple[str, float]]: Return sorted matches.
Optimization: C++-side sorting for efficiency; no Python overhead.


Scorers (Must-Have):

Levenshtein: Edit distance-based.
Token Sort: Sort tokens then compare.
Jaccard: Intersection over union for token sets.
Good-to-Have: Hybrid (e.g., hybrid(weights: Dict[str, float])), Custom (user callback, but warn on perf hit).

Match Modes (Must-Have):

Full: Whole string comparison.
Partial: Substring search with best-match.
Adjustable via param: mode: str = 'full'.

Processors (Good-to-Have):

Pre-match normalization: process: bool = True (lowercase, strip punctuation).
Trade-off: Enabled by default for usability, but optional to avoid altering inputs (privacy concern).

Batch Support (Good-to-Have):

batch_match(queries: List[str], targets: List[List[str]], ...) -> List[List[Tuple[str, float]]].
Parallel via threads if parallel: bool = True.

Integrations (Good-to-Have):

Pandas: Extension methods like Series.fuzzy_match(query).
NumPy: Vectorized matching for arrays.

3. Non-Functional Requirements
Performance:

<1ms per match for 100-char strings on standard hardware.
Scale to 10k candidates without slowdown (benchmark vs. rapidfuzz).
Trade-off: C++ for speed, but not GPU (unnecessary complexity).

Usability:

Intuitive API: Mirror fuzzywuzzy but faster.
Docs: Sphinx-generated, with examples.
Installation: pip install builds C++ automatically via setup.py.

Security/Privacy:

No network calls.
Secure by default: No file I/O unless explicit.
Handle secrets in strings without logging.

Maintainability:

Core logic in C++ (framework-agnostic).
Tests: 90% coverage with pytest (Python) and Catch2 (C++).
CI/CD: GitHub Actions for builds/tests.
One-command setup: pip install -e . for dev.

Compatibility:

Python 3.8+.
OS: Linux/Mac/Windows (cross-compile).
No deps beyond Pybind11 (bundled).

4. Architecture Overview

Layers:
C++ Core: Scorers and matching algos (single module for minimalism, no microservices).
Pybind11 Bindings: Expose as Python funcs/classes.
Python API: Thin wrapper for usability.

Data Flow: Input strings -> Normalize (opt) -> Compute in C++ -> Return scores.
Trade-offs: Embedded DB? No, use lists for candidates (simpler than SQLite). Redis? Avoid unless user adds for caching.
Alternatives Considered:
Pure Python: Simpler build, but slower—ranked lower for perf goals.
Rust + PyO3: Safer, but Pybind11 chosen for C++ expertise assumption.
Recommended: Proceed with C++/Pybind11, but prototype pure Python first to validate.


5. Risks and Mitigations

Build Failures: Mitigate with pre-built wheels on PyPI.
Perf Bottlenecks: Benchmark early; fallback to rapidfuzz if not superior.
Scope Creep: Limit to README features; good-to-haves as v2.
Bad Idea Alert: If custom scorers add >20% complexity without use cases, drop them—focus on speed.

6. Roadmap

MVP: Must-haves by v0.1.
v0.2: Good-to-haves.
Future: CLI wrapper reusing core (e.g., fuzzybunny match query target).
