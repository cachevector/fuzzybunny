import sys
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import pybind11

class PybindBuildExt(build_ext):
    """Custom build_ext to ensure specific compiler flags."""
    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = []
        if ct == 'unix':
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append('-std=c++17')
            opts.append('-fvisibility=hidden')
            if sys.platform != 'darwin':
                opts.append('-fopenmp')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\"%s\"' % self.distribution.get_version())
            opts.append('/std:c++17')
            opts.append('/openmp')
        
        for ext in self.extensions:
            ext.extra_compile_args = opts
            if ct == 'unix' and sys.platform != 'darwin':
                ext.extra_link_args = ['-fopenmp']
        
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
    version="0.2.0",
    description="A fuzzy search tool for python written in C++",
    long_description=open("README.md").read(),
    long_description_content_type="text/markdown",
    packages=["fuzzybunny"],
    package_dir={"": "src"},
    ext_modules=ext_modules,
    cmdclass={"build_ext": PybindBuildExt},
    zip_safe=False,
    python_requires=">=3.8",
)
