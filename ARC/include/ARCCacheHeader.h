#pragma once 
#include <iostream>
#include <string>
#include <unordered_map>  // 使用c++自带的哈希表
#ifndef ARCCACHEHEADER
#define ARCCACHEHEADER   // 避免头文件重复定义
#define Init_Capacity 30
#define Init_Transfrom_Time 3

using namespace std;
typedef string DataType;
/*简介:
* 使用LFU和LRU结合的方法(也即A)，实现对词汇进行自动补全功能中的缓存列表的存储操作
* 在本程序中，使用哈希表和双向链表的存储结构，结合了链表的有序特性和哈希表的查找特性
* 双向循环链表是便于移除尾部的元素
* 基于LFU(Least Recently User,最近不常用算法)和LRU(最近最少使用算法)实现一固定长度链表的插入
* 使用函数get和put来进行词组的读入操作, 并且要求get和put需要以O(1)的时间复杂度运行
*/

// 缓存节点列表
typedef struct CacheList {
    DataType key;  
    int time = 0;  // 定义使用计数器，计算使用次数
    CacheList* prev;
    CacheList* next;
};

// LRU类部分
class LRUcache {
public:
    LRUcache() {
        this->capacity = Init_Capacity;
        this->ghost_capacity = Init_Capacity;
    }
    LRUcache(int capacity) {
        // 注意这里不够健壮,这是由于初始化的capacity可能小于0，但暂且不管健壮性
        this->capacity = capacity;
        this->ghost_capacity = capacity; // 对ghost分配同样的存储空间
        // 分配同样大小的空间
    }
    LRUcache(int capacity, int transform_time) {
        // 注意这里不够健壮,这是由于初始化的capacity可能小于0，但暂且不管健壮性
        this->capacity = capacity;
        this->ghost_capacity = capacity; // 对ghost分配同样的存储空间
        this->transform_time = transform_time;
        // 分配同样大小的空间
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
    int capacity = Init_Capacity;

    CacheList* ghost = nullptr;
    std::unordered_map<DataType, CacheList*> ghost_map;
    int ghost_size = 0;
    int ghost_capacity = Init_Capacity; // ghost链表的存储空间(固定不变的)

    void Insert(CacheList* L, bool is_ghost); // 直接插入节点
    void DeleteTail(bool is_ghost); // 删除末尾节点
    void DetachNode(CacheList* L, bool is_ghost); // 脱离节点,即从表中删除但是不释放空间
    int transform_time = Init_Transfrom_Time; // 默认从LRU到LFU转移阈值是3

    /*
    * 插入函数的要求: 如果size < capacity,直接插入
    * 如果capacity已满，则先删除末尾的元素并将其加入ghost（单独定义加ghost函数）
    * 然后再将元素插入本身中
    *
    * 此时调用加入ghost的函数,这个函数的结构和Insert相同(直接插头结点)
    * 然后ghost如果满了则删除最后一个元素
    */
};

// LFU类部分
class LFUcache {
public:
    LFUcache() {
        this->capacity = Init_Capacity;
        this->ghost_capacity = Init_Capacity;
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
    int capacity = Init_Capacity;
    
    int transform_time = Init_Transfrom_Time; // 需要和LRU内一致
    
    CacheList* ghost = nullptr;
    unordered_map<DataType, CacheList*> ghost_map;
    int ghost_size = 0;
    int ghost_capacity = Init_Capacity;
    
    void DetachNode(CacheList* L, bool is_ghost);
    void DeleteTail(bool is_ghost); // 删除尾部元素
    void Insert(CacheList* L, bool is_ghost);     // 有序插入元素
};

class ARCcache {
public:
    ARCcache() {
        this->capacity = Init_Capacity;
        this->Rcache = new LRUcache(Init_Capacity);
        this->Fcache = new LFUcache(Init_Capacity);
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
    int capacity = Init_Capacity;
    int transform_time = Init_Transfrom_Time;  // 初始化为初始转变次数
    // 初始转变次数是触发消息频次达到几次时， 就加入LFU的cache
    // 默认加入的是LRU的cache,但是当触发达到一定次数，就会加入LFU的cache
};


#endif // !ARCCACHEHEADER
