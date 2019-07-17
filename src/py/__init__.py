#!/bin/env python
# Copyright Disney Enterprises, Inc.  All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License
# and the following modification to it: Section 6 Trademarks.
# deleted and replaced with:
# 
# 6. Trademarks. This License does not grant permission to use the
# trade names, trademarks, service marks, or product names of the
# Licensor and its affiliates, except as required for reproducing
# the content of the NOTICE file.
# 
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

"""
SeExprPy Python Bindings of SeExprPy

Getting more help:

help(SeExprPy._SeExprPy) 
  - to get info core classes
help(SeExprPy.utils)
  - interesting helper functions/classes

"""

from __future__ import absolute_import
import sys

from .core import AST as _AST, ASTHandle, ASTType
from .utils import (
    getComment, Edits, traverseCallback, traverseFilter, printTree
)

_PY2 = sys.version_info[0] == 2
try:
    ustr = unicode
except NameError:
    ustr = str


def _encode(obj, encoding='utf-8'):
    # The Python2 Boost.Python API requires byte strings
    if _PY2 and isinstance(obj, ustr):
        value = obj.encode(encoding)
    else:
        value = obj
    return value


# Override AST to handle unicode vs. str constructor values
class AST(_AST):
    def __init__(self, string):
        super(AST, self).__init__(_encode(string))
