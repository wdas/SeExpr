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
#pragma once

#include <map>
#include <string>

class SeContext{
public:
	/// Lookup a SeContext parameter by name.
	bool lookupParameter(const std::string& parameterName,std::string& value) const{
		ParameterMap::const_iterator it=_parameters.find(parameterName);
		if(it != _parameters.end()){value=it->second;return true;}
		else if(_parent) return _parent->lookupParameter(parameterName,value);
		else return false;
	}
	/// Set a parameter. NOTE: this must be done when no threads are accessing lookupParameter for safety
	void setParameter(const std::string& parameterName,const std::string& value);
	/// Create a context that is a child of this context
	SeContext* createChildContext() const;

	// Parent access uses pointers as it is acceptable to set/get a NULL parent
	void setParent(const SeContext* context) {_parent = context;}
	const SeContext* getParent() const {return _parent;}

	bool hasContext(const SeContext* context) const{
		if(this == context) return true;
		if(_parent) return _parent->hasContext(context);
		return false;
	}

	/// The global default context of the seexpr
	static SeContext& global();
private:
	/// Private constructor and un-implemented default/copy/assignment
	/// (it is required that we derive from the global context via createChildContext)
	SeContext(const SeContext&);
	SeContext& operator=(const SeContext&);

	SeContext(const SeContext* parent);
	/// The parent scope
	const SeContext* _parent;

	// TODO: Use std::map until C++11 is ubiq.
	typedef std::map<std::string,std::string> ParameterMap;
	/// Attribute/value pairs
	ParameterMap _parameters;
};
