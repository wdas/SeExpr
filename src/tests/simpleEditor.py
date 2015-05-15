from PyQt4 import QtCore, QtGui

from SeExpr.expreditor import SeExprEdControlCollection
from SeExpr.expreditor import SeExprEditor
from SeExpr.expreditor import SeExprEdBrowser

class ImageEditorWindow(QtGui.QMainWindow):

    def __init__(self, parent=None):
        QtGui.QMainWindow.__init__(self, parent)
        self.setObjectName("ImageEditorWindow")
        self.setWindowTitle("Image Editor")

        mainWidget = QtGui.QWidget()
        mainLayout = QtGui.QVBoxLayout()
        mainWidget.setLayout(mainLayout)
        self.setCentralWidget(mainWidget)

        # Expression controls
        controls = SeExprEdControlCollection()
        scrollArea = QtGui.QScrollArea(mainWidget)
        scrollArea.setMinimumHeight(100)
        scrollArea.setFixedWidth(450)
        scrollArea.setWidgetResizable(True)
        scrollArea.setWidget(controls)

        # Expression editor
        editor = SeExprEditor(mainWidget, controls)

        # Expression browser
        browser = SeExprEdBrowser(mainWidget, editor)

        mainLayout.addWidget(scrollArea)
        mainLayout.addWidget(browser)
        mainLayout.addWidget(editor)



if __name__ == "__main__":
    import sys, os
    app = QtGui.QApplication( sys.argv )
    mainWin = ImageEditorWindow()
    mainWin.show()
    sys.exit( app.exec_() )

