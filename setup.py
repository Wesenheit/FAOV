import numpy
from setuptools import setup,Extension
args = ["-O3","-march=native","-g"]

wraper = Extension("FAOV",sources=["src/python_wrapper.c","src/FAOV.c"],extra_compile_args = args, include_dirs = [numpy.get_include()])

setup(
    install_requires=[
            "numpy>=1.22.3",
        ],
    ext_modules = [wraper,],
    platforms = ['POSIX',]
    )
