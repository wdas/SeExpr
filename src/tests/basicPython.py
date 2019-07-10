#!/usr/bin/env python
import sys
import unittest

import SeExprPy


class SeExprTestCase(unittest.TestCase):
    """Basic SeExpr python tests"""

    def test_core_namespace(self):
        self.assertTrue(hasattr(SeExprPy, 'core'))

    def test_utils_namespace(self):
        self.assertTrue(hasattr(SeExprPy, 'utils'))

    def test_AST(self):
        expr = """
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


if __name__ == '__main__':
    unittest.main()
