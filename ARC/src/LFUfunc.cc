#include "../include/ARCCacheHeader.h"

// 插入元素，其中插入的次数直接设为transform_time
void LFUcache::put(DataType data) {
    if (this->capacity == 0) return; // 不能加入元素
    if (this -> map.find(data)!= this->map.end()) {
        // 列表中有该元素
        CacheList* L = map[data];
        this -> DetachNode(L, false);
        L->time += 1;   // 增加使用次数标记
        Insert(L, false);
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
        Insert(L, false);
    }
}

// 插入函数, 将节点插入表中,插入方法是有序插入
void LFUcache::Insert(CacheList* L, bool is_ghost) {
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
        this->size += 1;
    }
    else{ // 将节点插入ghost
        if (!ghost) {
            this->ghost = L;
            ghost->next = ghost;
            ghost->prev = ghost;
        }
        else if (ghost->time < L->time) {
            CacheList *p = ghost->prev;
            L->next = ghost;
            L->prev = p;
            p->next = L;
            ghost->prev = L;
            this->ghost = L;  // 更新头结点
        }
        else {
            CacheList* pre = ghost;
            // 在循环链表中有序插入元素
            for (; pre->next != ghost && pre->next->time > L->time; pre = pre->next);
            // 将L插入到pre的尾部即可
            CacheList* q = pre->next;
            L->prev = pre;
            L->next = q;
            pre->next = L;
            q->prev = L;
        }
        this->ghost_size += 1;
    }
}

// 添加元素并且增加空间
void LFUcache::Add(DataType data) {
    this->capacity += 1;
    CacheList* L = new CacheList();
    L->key = data;
    L->time = 1;  // 此时调用的次数设为1
    this->map[data] = L;
    Insert(L, false);
}

// 删除尾元素
void LFUcache::DeleteTail(bool is_ghost) {
    if (!is_ghost) {
        CacheList* remove;
        if (!this->cache) throw("no element to delete");
        // 删除节点，哈希值并将其移动到ghost表中
        else if (this->size == 1) { //删除头结点
            remove = cache;
            this->cache = nullptr;
        }
        else { // 移除尾结点
            remove = cache->prev;
            CacheList* q = remove->prev;
            q->next = cache;
            cache->prev = q;
        }
        remove->prev = nullptr; remove->next = nullptr; remove->time = 0;
        map.erase(remove->key);
        if (this->ghost_size == this->ghost_capacity) DeleteTail(true);
        
        this -> ghost_map[remove->key] = remove;   // 加入到ghost_map 的哈希表中
        Insert(remove, true);
        this->size -= 1;
    }
    else { // 删除ghost中的结尾元素
        if (!this->ghost) {
            throw("no element to delete");
        }
        CacheList* remove;
        if (this->ghost_size == 1) {
            remove = ghost;
            this->ghost = nullptr;
        }
        else {
            remove = ghost->prev;
            CacheList* q = remove->prev;
            q->next = ghost;
            ghost->prev = q;
        }
        ghost_map.erase(remove->key);
        delete remove;  // 释放指针空间和哈希表空间
        this->ghost_size -= 1;
    }

}

// 主表时(put调用)往其他位置有序insert, ghost表时(仅有check_ghost调用)，向主表中insert
void LFUcache::DetachNode(CacheList*L, bool is_ghost) {
    // Detach操作相同,直接复制粘贴FRUfunc的代码
    if (!is_ghost) {
        if (this->size == 0) {
            throw runtime_error("nothing to detach!");
        }
        if (this->size == 1) {
            this->cache = nullptr;
        }
        else if (L == cache) { // 暂时清除头结点
            this->cache = L->next;
            cache->prev = L->prev;
            L->prev->next = cache;
        }
        else { // 取出中间节点
            CacheList* q = L->prev;
            q->next = L->next;
            L->next->prev = q;
        }
        L->prev = nullptr; L->next = nullptr;
        size -= 1;
    }
    else {
        ghost_map.erase(L->key);
        // 注意: 由于之后指针会往主list中放, 不释放指针
        if (this->ghost_size == 1) {
            this->ghost = nullptr;
        }
        else if (L == ghost) {
            // 删除头结点
            CacheList* p = this->ghost;
            this->ghost = L->next; // 重置
            ghost->prev = L->prev;
            L->prev->next = ghost;
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
        ghost_size -= 1;
    }
}

// 删除结尾元素并减少容量
bool LFUcache::Subtract() {
    if (this ->capacity == 0) return false;
    // 如果表满,则删除节点并放入ghost数组, 否则不用删除节点
    if (this ->size == this ->capacity) DeleteTail(false);
    this->capacity -= 1;
    return true;
};

// 在ghost中进行查找，如果有则删除对应ghost中的元素并返回true(接下来总函数会调用put)
bool LFUcache::check_ghost(DataType data) {
    if (ghost_map.find(data)!= ghost_map.end()) {
        this ->DetachNode(ghost_map[data], true);
        return true;
    }
    return false;
}

// 显示缓存内容
void LFUcache::Show(bool show_ghost) {
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
