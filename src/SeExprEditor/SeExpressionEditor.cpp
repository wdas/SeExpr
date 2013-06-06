/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/

#include <iostream>
#include <string>

#include <QtGui/QApplication>
#include "SeExprEdDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv); 
    SeExprEdDialog dialog(0);
    dialog.setWindowTitle("Expression Editor");
    dialog.show();

    if (argc < 2 || std::string(argv[1]) != "-automatedTest") {
        if (dialog.exec() == QDialog::Accepted)
            std::cerr << "returned expression: " << dialog.getExpressionString() << std::endl;
    } else {
        std::string str = "$u + $v";
        dialog.setExpressionString(str);
        if (dialog.getExpressionString() != str ) {
            std::cerr << "test failed: " << dialog.getExpressionString() << " != " << str << std::endl;
            return 1;
        }
    }

    return 0;
}

