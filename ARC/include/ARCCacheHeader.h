/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      ARCCacheHeader.h
工程代码: ARC
模块名字: ARC缓存
创建时间: 2025-03-15
    作者: houchao
    描述: ARC类头文件
--------------------------------------------------------------------------------
*******************************************************************************/

#pragma once 
#include <iostream>
#include <unordered_map>
#include "infra_comp.h"
#include "LFUCacheHeader.h"
#include "LRUCacheHeader.h"

#ifndef ARC_CACHE_HEADER_H
#define ARC_CACHE_HEADER_H
#define CACHE_CAPACITY 30
#define CACHE_TRANSFROM_COUNT 3

#endif // !ARCCACHEHEADER

class ARCcache {
public:
    ARCcache():_capacity(CACHE_CAPACITY),_size(0),_transform_time(CACHE_TRANSFROM_COUNT) {
        _lrucache = new LRUCache(CACHE_CAPACITY);
        _lfucache = new LFUCache(CACHE_CAPACITY);
    }
    ARCcache(uint64 capacity):_capacity(capacity),_size(0),_transform_time(CACHE_TRANSFROM_COUNT) {
        _lrucache = new LRUCache(capacity);
        _lfucache = new LFUCache(capacity);
    }
    ARCcache(uint64 capacity, int transform_time):_capacity(capacity),_size(0),_transform_time(transform_time)  {
        _lrucache = new LRUCache(capacity);
        _lfucache = new LFUCache(capacity,transform_time);
    }

    void cache_insert(DataType data);
    void cache_show(bool show_ghost);

private:
    LRUCache *_lrucache;
    LFUCache *_lfucache;
    uint64 _size;
    uint64 _capacity;
    int _transform_time;
};


//#endif // !ARCCACHEHEADER
