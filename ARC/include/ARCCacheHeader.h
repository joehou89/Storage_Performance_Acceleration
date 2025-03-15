/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      ARCCacheHeader.h
工程代码: ARC
模块名字: ARC缓存
创建时间: 2025-03-15
    作者: joehou89
    描述: ARC类头文件
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



// LFU类部分
class LFUcache {
public:
    LFUcache() {
        this->capacity = CACHE_CAPACITY;
        this->ghost_capacity = CACHE_CAPACITY;
    }
    LFUcache(int capacity) {
        // 注意这里不够健壮,这是由于初始化的capacity可能小于0，但暂且不管健壮性s
        this->capacity = capacity;
        this->ghost_capacity = capacity;
        // 分配同样大小的空间
    }
    LFUcache(int capacity, int transform_time) {
        // 注意这里不够健壮,这是由于初始化的capacity可能小于0，但暂且不管健壮性s
        this->capacity = capacity;
        this->ghost_capacity = capacity;
        this->transform_time = transform_time;
        // 分配同样大小的空间
    }
    void put(DataType data); // 向LFU内增加缓存元素
    void Add(DataType data); 
    bool Subtract();
    bool check_ghost(DataType data); // 检查ghost中是否有对应元素,有则返回true
    void Show(bool show_ghost);
private:
    CacheList* cache = nullptr;
    unordered_map<DataType, CacheList*> map;
    int size = 0;
    int capacity = CACHE_CAPACITY;
    
    int transform_time = CACHE_TRANSFROM_COUNT; // 需要和LRU内一致
    
    CacheList* ghost = nullptr;
    unordered_map<DataType, CacheList*> ghost_map;
    int ghost_size = 0;
    int ghost_capacity = CACHE_CAPACITY;
    
    void DetachNode(CacheList* L, bool is_ghost);
    void DeleteTail(bool is_ghost); // 删除尾部元素
    void Insert(CacheList* L, bool is_ghost);     // 有序插入元素
};

class ARCcache {
public:
    ARCcache() {
        this->capacity = CACHE_CAPACITY;
        this->Rcache = new LRUcache(CACHE_CAPACITY);
        this->Fcache = new LFUcache(CACHE_CAPACITY);
    }
    ARCcache(int capacity) {
        this->Rcache = new LRUcache(capacity);
        this->Fcache = new LFUcache(capacity);
        this->capacity = capacity;
    }
    ARCcache(int capacity, int transform_time) {
        this->Rcache = new LRUcache(capacity,transform_time);
        this->Fcache = new LFUcache(capacity,transform_time);
        this->capacity = capacity;
        this->transform_time = transform_time;
    }
    void put(DataType data);
    void Show(bool show_ghost);
private: 
    // 一共有两个缓存指针,  分别存储Rcache和Fcache
    LRUcache* Rcache = nullptr;
    LFUcache* Fcache = nullptr;
    int size = 0;
    int capacity = CACHE_CAPACITY;
    int transform_time = CACHE_TRANSFROM_COUNT;  // 初始化为初始转变次数
    // 初始转变次数是触发消息频次达到几次时， 就加入LFU的cache
    // 默认加入的是LRU的cache,但是当触发达到一定次数，就会加入LFU的cache
};


#endif // !ARCCACHEHEADER
