from distutils.core import setup,Extension
from Cython.Build import cythonize
import numpy as np

setup(
	ext_modules=cythonize(Extension(
		name='HyperLogLog',
		version='0.0.1',
		sources=['hyperLL.pyx'],
		language='c++',
		extra_compile_args=["-std=c++11"],
		include_dirs=[".", "module-dir-name",np.get_include()],
		install_requires=['Cython>=0.2.15'],
		packages=['hll'],
		python_requires='>=3'
	))
)
