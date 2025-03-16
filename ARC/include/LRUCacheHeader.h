/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      LFUCacheHeader.h
工程代码: LFUCache
模块名字: LFUCache缓存
创建时间: 2025-03-15
    作者: joehou89
    描述: LFUCache类头文件
--------------------------------------------------------------------------------
*******************************************************************************/

#include <iostream>
#include <string>
#include <unordered_map>
#include "infra_comp.h"

// LRU类部分
class LRUCache {
public:
    LRUCache():capacity(CACHE_CAPACITY),ghost_capacity(CACHE_CAPACITY) {
    }
    LRUCache(uint64 capacity):capacity(capacity),ghost_capacity(capacity) {
    }
    LRUCache(uint64 capacity, uint64 transform_time):capacity(capacity),ghost_capacity(capacity),
        transform_time(transform_time) {
    }  

    void cache_insert(DataType data);                       // 插入元素
    void cache_insert_and_extend(DataType data);            // 插入并且扩充一格
    bool cache_evict_and_subtract();                        // 删除末尾元素且空间缩减，返回是否成功
    bool cache_check_ghost(DataType data);                  // 检查ghost中是否有对应元素,有则返回true
    void cache_show(bool show_ghost);                       // 显示所有元素

private:
    CacheList* _cache = nullptr;
    CacheList* _ghost = nullptr;
    uint64 _capacity = CACHE_CAPACITY;
    int ghost_capacity = CACHE_CAPACITY; // ghost链表的存储空间(固定不变的)
    uint64 _size = 0;
    uint64 ghost_size = 0;
    std::unordered_map<DataType, CacheList*> _map;
    std::unordered_map<DataType, CacheList*> _ghost_map;
    
    void insert(CacheList* L, bool is_ghost);                    // 直接插入节点
    void delete_tail_listnode(bool is_ghost);                    // 删除末尾节点
    void detach_listnode(CacheList* L, bool is_ghost);           // 脱离节点,即从表中删除但是不释放空间
    uint64 transform_time = CACHE_TRANSFROM_COUNT;                  // 默认从LRU到LFU转移阈值是3
};