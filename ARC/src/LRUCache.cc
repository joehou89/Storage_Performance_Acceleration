/*******************************************************************************
 Copyright (c) 2025, Hangzhou Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                                      LRUCache.cc
工程代码: LRU
模块名字: LRU缓存
创建时间: 2025-03-17
    作者: joehou89
    描述: LRU类实现
--------------------------------------------------------------------------------
*******************************************************************************/


#include "ARCCacheHeader.h"

// put 是直接向缓存空间输入的函数: 如果次数大于3返回true, 否则返回false
bool LRUCache::cache_insert(DataType data) {
    // 对于另一个把这个空间全部占用掉的情况，此时capacity=0,不插入
    if (_capacity == 0) return false;

    if (_map.find(data)!= _map.end()) { // 如果找到, 则将节点移到头部, 并将
        CacheList* L = _map[data]; // 注意: 如果map[data]->time > 3, 则需要添加到LFUcache中
        L->time++;
        // 删除之后，在头部添加相应的节点
        detach_listnode(L, false); // 从节点中移除
        insert(L, false);     // 把这个节点放在头结点(第一个访问的节点)
        if (L->time >= transform_time) return true;  // 在LFU中加入一次
    }
    else { // map中没有找到
        CacheList* L = new CacheList(); // 新建缓存节点
        map[data] = L;  // 将节点存储在哈希表中
        if (_size == _capacity) {
            delete_tail_listnode(false);
            // 删除节点，探后放进ghost中
        }
        L->key = data;
        L->time = 1;    // 使用次数为1
        insert(L, false); // 在缓存中插入节点
    }
    return false;
}

// 使用头插方法，仅插入节点, 不考虑删除
void LRUCache::insert(CacheList *L, bool is_ghost) {
    if (!L) return;
    if (is_ghost) {
        if (!this->ghost) {
            ghost = L;
            ghost->next = ghost;
            ghost->prev = ghost;
        }
        else {
            CacheList* q = ghost->prev;
            L->next = ghost;
            L->prev = q;
            q->next = L; 
            ghost->prev = L;
            this->ghost = L;
        }
        this->ghost_size += 1;
    }
    else { // 向主表内插入
        if (!cache) { // 对于头结点为空的情况
            cache = L;
            cache->next = cache;
            cache->prev = cache;
        }
        else {// 由于是头结点访问时间最近，所以使用头插方法, 插入完之后重新给head节点赋值
            CacheList* q = cache->prev;
            L->next = cache;
            L->prev = q;
            cache->prev = L;
            q->next = L;
            this->cache = L;
        }
        this->size += 1;
        // 插入完毕之后，头结点是最近访问的元素
    }
};

// 对于主List是让L暂时取出链表不释放指针(已经置空前后指针)，之后调用插入函数进行头插
// 而对于ghost_list是删除哈希表元素，不释放指针便于后续往主List中插入
void LRUCache::detach_listnode(CacheList* L, bool is_ghost) {
    // 注意需要判断是否为ghost，虽然代码相似，但是一个是size-1, 另一个ghost_size-1
    if (!is_ghost) { // 暂时移除指针，不抹除哈希表中的元素
        if (_size == 0) {
            throw runtime_error("nothing to detach!");
        }
        if (_size == 1) {
            _cache = nullptr;
        }
        else if (L == _cache) { // 暂时清除头结点
            _cache = L->next;
            _cache->prev = L->prev;
            L->prev->next = _cache;
        }
        else { // 取出中间节点
            CacheList* q = L->prev;
            q->next = L->next;
            L->next->prev = q;
        }
        L->prev = nullptr;
        L->next = nullptr;
        _size--;
    }
    else {
        _ghost_map.erase(L->key);
        // 注意: 由于之后指针会往主list中放, 不释放指针
        if (_ghost_size == 1) {
            _ghost = nullptr;
        }
        else if (L == ghost) { 
            // 删除头结点
            CacheList* p = _ghost;
            _ghost = L->next; // 重置
            _ghost->prev = L->prev;
            L->prev->next = _ghost;
        }
        else { // 连接前后节点
            CacheList* q = L->prev;
            q->next = L->next;
            L->next->prev = q;
        }
        // 重置L指针以便后续插入
        L->prev = nullptr;
        L->next = nullptr;
        L->time = 1;            // 将time重置为1
        _ghost_size--;
    }
}

// 删除尾部元素并删除哈希表对应元素, 如果是删除主表的元素，则加入ghost中
void LRUCache::delete_tail_listnode(bool is_ghost) {
    if (!is_ghost) {
        CacheList* remove;
        if (!_cache) throw("no element to delete");
        else if (_size == 1) {
            remove = _cache;
            _cache = nullptr; // 更新头结点
        }
        else {
            // 移除 cache的prev节点即尾结点
            remove = _cache->prev;
            CacheList* q = remove->prev;
            q->next = _cache;
            _cache->prev = q;
        }
        // 重整remove节点
        remove->prev = nullptr;
        remove->next = nullptr;
        remove->time = 0;
        // 清除哈希表中的元素
        _map.erase(remove->key);

        if (_ghost_size == _ghost_capacity) {
            delete_tail_listnode(true);
        }
        // 对于ghost缓存满，则先移除元素, 再插入到ghost数组中, 不满则不移除元素
        // 然后将元素加入ghost中,显然ghost中是没有对应的项的
        _ghost_map[remove->key] = remove;
        insert(remove, true);
        _size--;
    }
    else { // 删除ghost数组中的尾部并移除
        if (!_ghost) throw("no element to delete");
        CacheList* remove;
        if (_ghost_size == 1) { 
            // 对于ghost中有一个元素并被取出时，删除头结点
            remove = _ghost;
            _ghost = nullptr;
        }
        else {
            remove = _ghost->prev;
            CacheList* q = remove->prev;
            q->next = _ghost;
            _ghost->prev = q;
        }
        _ghost_map.erase(remove->key); // 删除哈希表元素
        delete remove; // 释放存储空间
        _ghost_size--;
    }
}

// 插入元素，并且扩容一格(注意需要判断最大空间)
void LRUCache::cache_insert_and_extend(DataType data) {
    // 这个对应的是在ghost中找到的情况，需要删除ghost中对应的节点
    _capacity++;
    CacheList* L = new CacheList();
    L->key = data;
    L->time = 1;
    _map[data] = L; // 直接塞入哈希表
    insert(L, false);    // 插入节点 
}

// 删除最后一个节点，并且容量缩减一格, 返回是否成功缩减subtract
bool LRUCache::cache_evict_and_subtract() {
    if (_capacity == 0) return false;
    // 如果表满,则删除节点并放入ghost数组, 否则不用删除节点
    if (_size == _capacity) {
        delete_tail_listnode(false);
    }
    _capacity--;
    return true;
}

// 在ghost中进行查找，如果有则删除对应ghost中的元素并返回true(接下来总函数会调用put)
bool LRUCache::cache_check_ghost(DataType data) {
    // 注意使用find而不使用ghost_map[data]; 因为这样会新增元素
    if (_ghost_map.find(data)!= _ghost_map.end()) {
        detach_listnode(_ghost_map[data], true);
        return true;
    }
    return false;
}

void LRUCache::cache_show(bool show_ghost) {
    CacheList* p = _cache;
    cout << "=============== Last recently used =========" << endl;
    if (!p) {
        cout << "----- empty list : nothing to show -----" << endl;
        return;
    }
    for (; p->next!= _cache;p = p->next) {
        cout << "key : " << p->key << " , time : " << p->time << endl;
    }
    cout << "key : " << p->key << " , time : " << p->time << endl;
    if (show_ghost) {
        cout << "------------ghost is ----------------" << endl;
        CacheList* p = _ghost; 
        if (!p) return;
        for (; p->next != _ghost; p = p->next) {
            cout << "key : " << p->key << " , time : " << p->time << endl;
        }
        cout << "key : " << p->key << " , time : " << p->time << endl;
    }
    cout << "=============== end ====================" << endl;
}

