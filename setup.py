from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import subprocess
import os

class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        subprocess.check_call(['cmake', ext.sourcedir, '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                               '-DPYTHON_EXECUTABLE=' + os.path.join(os.path.dirname(os.__file__), 'python')])
        subprocess.check_call(['cmake', '--build', '.'])

setup(
    name='STLRom',
    version='0.1.0',
    packages=find_packages(),
    description='A library for robust online monitoring of STL specifications.',
    long_description=open('README.md').read(),
    long_description_content_type='text/markdown',
    url='https://github.com/decyphir/STLRom',
    author='Alexandre Donzé',
    author_email='alex@decyphir.com',
    classifiers=[
        'License :: BSD-3-Clause',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Development Status :: 3 - Alpha'
    ],
    ext_modules=[CMakeExtension('STLRom')],
    cmdclass=dict(build_ext=CMakeBuild),
)
