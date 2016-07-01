/*
 Copyright Disney Enterprises, Inc.  All rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License
 and the following modification to it: Section 6 Trademarks.
 deleted and replaced with:

 6. Trademarks. This License does not grant permission to use the
 trade names, trademarks, service marks, or product names of the
 Licensor and its affiliates, except as required for reproducing
 the content of the NOTICE file.

 You may obtain a copy of the License at
 http://www.apache.org/licenses/LICENSE-2.0
*/
#ifndef immutable_hash_map_h
#define immutable_hash_map_h

// a simple hash map optimized for simple, immutable data

#include <inttypes.h>
#include <math.h>
#include <vector>

template <typename key_type, typename value_type>
class immutable_hash_map {
  public:
    std::vector<key_type> _keys;
    std::vector<value_type> _values;
    int _size;
    int _mask;

    immutable_hash_map(key_type* keys, value_type* values, int size) : _size(size) {
        if (size <= 0) {
            _keys.resize(1, "");
            _values.resize(1);
            _mask = 0;
            return;
        }

        // build table, size = (nearest power of two >= size*2)
        int table_size = 1 << int(ceil(log(size * 2) * (1 / M_LN2)));
        _mask = table_size - 1;

        _keys.resize(table_size);
        _values.resize(table_size);

        key_type* kp = keys, *end = kp + size;
        value_type* vp = values;
        while (kp != end) {
            int pos = find(*kp);
            _keys[pos] = *kp++;
            _values[pos] = *vp++;
        }
    }

    const value_type& operator[](const key_type& key) const { return _values[find(key)]; }

    int size() const { return _size; }

  private:
    int find(const key_type& key) const {
        uint32_t hash = intptr_t(key) ^ (intptr_t(key) >> 32);
        // hash function from Thomas Wang (wang@cup.hp.com)
        hash = ~hash + (hash << 15);
        hash = hash ^ (hash >> 12);
        hash = hash + (hash << 2);
        hash = hash ^ (hash >> 4);
        hash = hash * 2057;
        hash = hash ^ (hash >> 16);
        while (1) {
            int pos = hash & _mask;
            const key_type& k = _keys[pos];
            if (k == key || !k) return pos;  // found key or blank
            hash++;                          // collision, keep looking
        }
    }
};

#endif
