# Installation

FuzzyBunny can be installed from PyPI using `pip`.

```bash
pip install fuzzybunny
```

## System Requirements

- **Python**: 3.8 or higher.
- **Compiler**: C++17 compatible compiler (only if building from source).

## Platform Specifics

### macOS

For high-performance parallel processing via OpenMP, it is highly recommended to install `libomp` via Homebrew:

```bash
brew install libomp
```

FuzzyBunny will automatically detect `libomp` and enable multi-threading for `batch_match`.

### Linux

Most Linux distributions have `libgomp` pre-installed as part of `gcc`. No extra steps are typically required.

### Windows

OpenMP is supported via the MSVC compiler flags.
