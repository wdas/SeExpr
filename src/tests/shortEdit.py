from PyQt4 import QtCore, QtGui

import sys, os
# sys.settrace # uncomment to use gdb for debugging

from SeExpr.expreditor import SeExprEdShortEdit

class ShortEditWindow(QtGui.QMainWindow):

    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.setObjectName("ShortEditWindow")
        self.setWindowTitle("Test ShortEdit Widget")

        mainWidget = QtGui.QWidget()
        mainLayout = QtGui.QVBoxLayout()
        mainWidget.setLayout(mainLayout)
        self.setCentralWidget(mainWidget)

        label = QtGui.QLabel("Test SeExprEdShortEdit:")
        expr = SeExprEdShortEdit(parent)
        expr.setExpressionString("$var0 = [1,0,0]; $var0");

        mainLayout.addWidget(label)
        mainLayout.addWidget(expr)


if __name__ == "__main__":
    app = QtGui.QApplication( sys.argv )
    mainWin = ShortEditWindow()
    mainWin.show()
    sys.exit( app.exec_() )
