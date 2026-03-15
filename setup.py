import sys
import subprocess
import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11

def get_brew_prefix():
    try:
        return subprocess.check_output(["brew", "--prefix"], encoding="utf8").strip()
    except Exception:
        return "/usr/local"

class PybindBuildExt(build_ext):
    """Custom build_ext to ensure specific compiler flags."""
    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = []
        link_opts = []
        
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append('-std=c++17')
            opts.append('-fvisibility=hidden')
            
            if sys.platform == 'darwin':
                # Try to enable OpenMP on macOS using libomp from Homebrew
                brew_prefix = get_brew_prefix()
                omp_prefix = os.path.join(brew_prefix, "opt", "libomp")
                if os.path.exists(omp_prefix):
                    opts.append('-Xpreprocessor')
                    opts.append('-fopenmp')
                    opts.append(f'-I{omp_prefix}/include')
                    link_opts.append(f'-L{omp_prefix}/lib')
                    link_opts.append('-lomp')
            else:
                opts.append('-fopenmp')
                link_opts.append('-fopenmp')
                
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\"%s\"' % self.distribution.get_version())
            opts.append('/std:c++17')
            opts.append('/openmp')
        
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.extra_link_args = link_opts
        
        build_ext.build_extensions(self)

ext_modules = [
    Extension(
        "fuzzybunny._fuzzybunny",
        ["src/bindings.cpp", "src/scorers.cpp"],
        include_dirs=[
            pybind11.get_include(),
            "src"
        ],
        language="c++"
    ),
]

setup(
    name="fuzzybunny",
    version="0.3.0",
    description="A fuzzy search tool for python written in C++",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/cachevector/fuzzybunny",
    project_urls={
        "Bug Tracker": "https://github.com/cachevector/fuzzybunny/issues",
        "Source Code": "https://github.com/cachevector/fuzzybunny",
    },
    packages=["fuzzybunny"],
    package_dir={"": "src"},
    package_data={"fuzzybunny": ["py.typed", "*.pyi"]},
    ext_modules=ext_modules,
    cmdclass={"build_ext": PybindBuildExt},
    zip_safe=False,
    python_requires=">=3.8",
)
