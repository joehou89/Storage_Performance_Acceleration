#include "../include/ARCCacheHeader.h"

// 插入元素，其中插入的次数直接设为transform_time
void LFUcache::cache_insert(DataType data) {
    if (this->capacity == 0) return; // 不能加入元素
    if (this -> map.find(data)!= this->map.end()) {
        // 列表中有该元素
        CacheList* L = map[data];
        this -> DetachNode(L, false);
        L->time += 1;   // 增加使用次数标记
        insert(L, false);
        cout << "after insert:" << L->time << endl;
    }
    else { // 没有找到, 考虑删除或者插入元素
        CacheList* L = new CacheList();
        if (this->size == this->capacity) {
            DeleteTail(false); // 删除一个元素并加入ghost中
        }
        L->key = data;
        L->time = this -> transform_time;
        
        cout << "putting : " << L->key << "the time is " <<  this -> transform_time << endl;
        this->map[data] = L;
        insert(L, false);
    }
}

// 插入函数, 将节点插入表中,插入方法是有序插入
void LFUcache::insert(CacheList* L, bool is_ghost) {
    if (!is_ghost) {
        // 有序插入算法
        if (!cache) { // 空则建立结点
            this -> cache = L;
            cache->next = cache;
            cache->prev = cache;
        }
        else if (cache->time < L->time) { // 插入到头结点上
            CacheList* p = cache->prev;
            L->next = cache;
            L->prev = p;
            p->next = L;
            cache->prev = L;
            this->cache = L; // 更新头结点
        }
        else {
            CacheList* pre = cache;
            // 在循环链表中有序插入元素 -> 此处有不足，如果使用双哈希表可以得到O(1)的复杂度
            for (; pre->next != cache && pre ->next-> time > L->time; pre = pre->next);
            // 最后停在最末端元素上,将L插入到pre的尾部即可
            CacheList* q = pre->next;
            L->prev = pre;
            L->next = q;
            pre->next = L;
            q->prev = L;
        }
        this->size++;
    }
    else{ // 将节点插入ghost
        if (!_ghost) {
            _ghost = L;
            _ghost->next = _ghost;
            _ghost->prev = _ghost;
        }
        else if (_ghost->time < L->time) {
            CacheList *p = _ghost->prev;
            L->next = _ghost;
            L->prev = p;
            p->next = L;
            _ghost->prev = L;
            _ghost = L;  // 更新头结点
        }
        else {
            CacheList* pre = _ghost;
            // 在循环链表中有序插入元素
            for (; pre->next != _ghost && pre->next->time > L->time; pre = pre->next);
            // 将L插入到pre的尾部即可
            CacheList* q = pre->next;
            L->prev = pre;
            L->next = q;
            pre->next = L;
            q->prev = L;
        }
        _ghost_size++;
    }
}

// 添加元素并且增加空间
void LFUcache::cache_insert_and_expend(DataType data) {
    _capacity += 1;
    CacheList* L = new CacheList();
    L->key = data;
    L->time = 1;  // 此时调用的次数设为1
    this->map[data] = L;
    insert(L, false);
}

// 删除尾元素
void LFUcache::delete_tail_listnode(bool is_ghost) {
    if (!is_ghost) {
        CacheList* remove = nullptr;
        if (!_cache) throw("no element to delete");
        // 删除节点，哈希值并将其移动到ghost表中
        else if (_size == 1) { //删除头结点
            remove = _cache;
            _cache = nullptr;
        }
        else { // 移除尾结点
            remove = _cache->prev;
            CacheList* q = remove->prev;
            q->next = _cache;
            _cache->prev = q;
        }
        remove->prev = nullptr;
        remove->next = nullptr;
        remove->time = 0;
        _map.erase(remove->key);
        if (_ghost_size == _ghost_capacity) delete_tail_listnode(true);
        
        _ghost_map[remove->key] = remove;   // 加入到ghost_map 的哈希表中
        insert(remove, true);
        _size--;
    }
    else { // 删除ghost中的结尾元素
        if (!_ghost) {
            throw("no element to delete");
        }
        CacheList* remove;
        if (_ghost_size == 1) {
            remove = _ghost;
            _ghost = nullptr;
        }
        else {
            remove = _ghost->prev;
            CacheList* q = remove->prev;
            q->next = _ghost;
            _ghost->prev = q;
        }
        _ghost_map.erase(remove->key);
        delete remove;  // 释放指针空间和哈希表空间
        _ghost_size--;
    }

}

// 主表时(put调用)往其他位置有序insert, ghost表时(仅有check_ghost调用)，向主表中insert
void LFUcache::detach_listnode(CacheList*L, bool is_ghost) {
    // Detach操作相同,直接复制粘贴FRUfunc的代码
    if (!is_ghost) {
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
        L->prev = nullptr; L->next = nullptr;
        size--;
    }
    else {
        _ghost_map.erase(L->key);
        // 注意: 由于之后指针会往主list中放, 不释放指针
        if (_ghost_size == 1) {
            _ghost = nullptr;
        }
        else if (L == _ghost) {
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

// 删除结尾元素并减少容量
bool LFUcache::cache_evict_and_subtract() {
    if (_capacity == 0) return false;
    // 如果表满,则删除节点并放入ghost数组, 否则不用删除节点
    if (_size == _capacity) delete_tail_listnode(false);
    _capacity--;
    return true;
};

// 在ghost中进行查找，如果有则删除对应ghost中的元素并返回true(接下来总函数会调用put)
bool LFUcache::check_ghost(DataType data) {
    if (ghost_map.find(data)!= ghost_map.end()) {
        detach_listnode(ghost_map[data], true);
        return true;
    }
    return false;
}

// 显示缓存内容
void LFUcache::cache_show(bool show_ghost) {
    CacheList* p = this->cache;
    cout << "=============== Last frequently used =========" << endl;
    if (!p) {
        cout << "----- empty list : nothing to show -----" << endl;
        return;
    }
    for (; p->next != cache; p = p->next) {
        cout << "key : " << p->key << " , time : " << p->time << endl;
    }
    cout << "key : " << p->key << " , time : " << p->time << endl;
    if (show_ghost) {
        cout << "------------ghost is ----------------" << endl;
        CacheList* p = ghost;
        if (!p) return;
        for (; p->next != ghost; p = p->next) {
            cout << "key : " << p->key << " , time : " << p->time << endl;
        }
        cout << "key : " << p->key << " , time : " << p->time << endl;
    }
    cout << "=============== end ====================" << endl;
}
