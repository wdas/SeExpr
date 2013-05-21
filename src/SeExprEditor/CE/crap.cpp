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

struct Foo{
    int a,b;
    float getSum(){
        return a+b;
    }
}

int foo()
{
    Foo foo;
    auto it=3+4;
    
    foo.getSum();
}
