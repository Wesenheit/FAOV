import numpy
from setuptools import setup,Extension
args = ["-O3","-march=native","-g"]

wraper = Extension("FAOV",sources=["src/python_wrapper.c","src/FAOV.c"],extra_compile_args = args, include_dirs = [numpy.get_include()])

setup(
    name = "FAOV",
    version = "0.1",
    maintainer = "Mateusz Kapusta",
    author_email ="mr.kapusta@student.uw.edu.pl",
    install_requires = [
            "numpy>=1.22.3",
        ],
    ext_modules = [wraper,],
    platforms = ['POSIX',]
    )
