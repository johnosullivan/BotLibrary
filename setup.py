# Setup for python verison of rolib

from setuptools import setup, find_packages


with open('README.md') as f:
    readme = f.read()

with open('LICENSE') as f:
    license = f.read()

setup(
    name='pyrolib',
    version='1.0',
    description='Python verison of rolib',
    long_description=readme,
    author='John OSullivan',
    author_email='josullivan1@luc.edu',
    url='luc.edu',
    license=license,
    packages=find_packages(exclude=('tests','docs','src','lua_headers'))
)
