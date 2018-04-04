/*
* Copyright Disney Enterprises, Inc.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License
* and the following modification to it: Section 6 Trademarks.
* deleted and replaced with:
*
* 6. Trademarks. This License does not grant permission to use the
* trade names, trademarks, service marks, or product names of the
* Licensor and its affiliates, except as required for reproducing
* the content of the NOTICE file.
*
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*/

#include "ContextUtils.h"
#include "Context.h"

namespace SeExpr2 {

namespace /* anonymous */ {
const std::string disableThreading = "disableThreading";
}  // namespace anonymous

void ContextUtils::DisableThreading(Context& context)
{
    context.setParameter(disableThreading, "true");
}

bool ContextUtils::IsThreading(const Context& context)
{
    bool isThreading = true;

    std::string result;
    if (context.lookupParameter(disableThreading, result) && result == "true") {
        isThreading = false;
    }

    return isThreading;
}

}  // namespace SeExpr2
