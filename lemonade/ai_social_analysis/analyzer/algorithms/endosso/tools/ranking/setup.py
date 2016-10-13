from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

# Run: python setup.py build_ext --inplace
setup(
	cmdclass = {'build_ext': build_ext},
	ext_modules = [Extension("arlei_ranking_c", ["arlei_ranking_c.pyx"])]
)