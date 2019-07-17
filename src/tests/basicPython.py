#!/usr/bin/env python
from __future__ import absolute_import, division, unicode_literals
import sys
import unittest

try:
    import SeExprPy
except ImportError:
    sys.stderr.write(
        'WARNING: SeExprPy is not importable: Python support is disabled\n')
    sys.exit(0)


MAP_EXPR = """
    # a cool expression
    a = 3;
    if (a > 5) {
        b = 10;
    } else if (a > 10) {
        b = 20;
    }
    c += 10 + P;

    # note: intentional use of different quotes
    map("a.png") + map('b.png') + map("c.png")
"""


class SeExprTestCase(unittest.TestCase):
    """Basic SeExpr python tests"""

    def test_core_namespace(self):
        self.assertTrue(hasattr(SeExprPy, 'core'))

    def test_utils_namespace(self):
        self.assertTrue(hasattr(SeExprPy, 'utils'))

    def test_AST(self, expr=MAP_EXPR):
        ast = SeExprPy.AST(expr)
        edits = SeExprPy.Edits(expr)
        maps = []

        def collect_maps(node, childs):
            # check node is a call
            if node.type == SeExprPy.ASTType.Call:
                # check if the call is map
                if node.value == 'map':
                    # check if the first argument is a string
                    if childs[0].type == SeExprPy.ASTType.String:
                        maps.append(childs[0].value)

        SeExprPy.traverseCallback(ast.root(), collect_maps)

        expect_maps = ['a.png', 'b.png', 'c.png']
        self.assertEqual(expect_maps, maps)

    def test_AST_bytes(self):
        self.test_AST(expr=MAP_EXPR.encode('utf-8'))


if __name__ == '__main__':
    unittest.main()
