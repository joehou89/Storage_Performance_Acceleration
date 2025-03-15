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
    LRUCache() {
      capacity = CACHE_CAPACITY;
      ghost_capacity = CACHE_CAPACITY;
    }

    LRUCache(uint64 capacity) {
      capacity = capacity;
      ghost_capacity = capacity;
    }

    LRUCache(uint64 capacity, uint64 transform_time) {
        capacity = capacity;
        ghost_capacity = capacity;
        transform_time = transform_time;
    }
    
    bool put(DataType data); // 输入元素
    void Add(DataType data); // 输入并且扩充一格
    bool Subtract();         // 删除末尾元素且空间缩减，返回是否成功
    // 其中，由于只是插入元素，则对于ghost的处理放在插入元素函数中即可
    bool check_ghost(DataType data); // 检查ghost中是否有对应元素,有则返回true
    void Show(bool show_ghost); // 显示所有元素

private:
    CacheList* cache = nullptr;
    std::unordered_map<DataType, CacheList*> map;
    int size = 0;
    int capacity = CACHE_CAPACITY;

    CacheList* ghost = nullptr;
    std::unordered_map<DataType, CacheList*> ghost_map;
    int ghost_size = 0;
    int ghost_capacity = CACHE_CAPACITY; // ghost链表的存储空间(固定不变的)

    void Insert(CacheList* L, bool is_ghost); // 直接插入节点
    void DeleteTail(bool is_ghost); // 删除末尾节点
    void DetachNode(CacheList* L, bool is_ghost); // 脱离节点,即从表中删除但是不释放空间
    int transform_time = CACHE_TRANSFROM_COUNT; // 默认从LRU到LFU转移阈值是3

    /*
    * 插入函数的要求: 如果size < capacity,直接插入
    * 如果capacity已满，则先删除末尾的元素并将其加入ghost（单独定义加ghost函数）
    * 然后再将元素插入本身中
    *
    * 此时调用加入ghost的函数,这个函数的结构和Insert相同(直接插头结点)
    * 然后ghost如果满了则删除最后一个元素
    */
};