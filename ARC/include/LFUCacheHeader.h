/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      LFUCacheHeader.h
工程代码: LFU
模块名字: LFU缓存
创建时间: 2025-03-15
    作者: joehou89
    描述: LFU类头文件
--------------------------------------------------------------------------------
*******************************************************************************/
#pragma once 
#include <iostream>
#include <string>
#include <unordered_map>

#ifndef ARC_CACHE_HEADER_H
#define ARC_CACHE_HEADER_H
#define CACHE_CAPACITY 30
#define CACHE_TRANSFROM_COUNT 3
using namespace std;

typedef unsigned long uint64;


// LFU类部分
class LFUCache {
public:
    LFUCache():_capacity(CACHE_CAPACITY),_ghost_capacity(CACHE_CAPACITY) {
    }
    LFUCache(uint64 capacity):_capacity(capacity),_ghost_capacity(capacity) {
    }
    LFUCache(uint64 capacity, uint64 transform_time):_capacity(capacity),_ghost_capacity(capacity),
        _transform_time(transform_time) {
    }

    void cache_insert(DataType data);
    void cache_insert_and_extend(DataType data);
    bool cache_evict_and_subtract();
    bool cache_check_ghost(DataType data);                  // 检查ghost中是否有对应元素,有则返回true
    void cache_show(bool show_ghost);

private:
    CacheList* _cache = nullptr;
    CacheList* _ghost = nullptr;
    unordered_map<DataType, CacheList*> _map;
    unordered_map<DataType, CacheList*> _ghost_map;
    uint64 _size = 0;
    uint64 _ghost_size = 0;
    uint64 _capacity = CACHE_CAPACITY;
    uint64 _ghost_capacity = CACHE_CAPACITY;
    int _transform_time = CACHE_TRANSFROM_COUNT;             // 需要和LRU内一致

private:
    void detach_listnode(CacheList* L, bool is_ghost);
    void delete_tail_listnode(bool is_ghost);                         // 删除尾部元素
    void insert(CacheList* L, bool is_ghost);               // 有序插入元素
};

#endif
