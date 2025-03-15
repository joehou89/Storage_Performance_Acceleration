#include "../include/ARCCacheHeader.h"

// put 是直接向缓存空间输入的函数: 如果次数大于3返回true, 否则返回false
bool LRUcache::put(DataType data) {
    // 对于另一个把这个空间全部占用掉的情况，此时capacity=0,不插入
    if (this->capacity == 0) return false;

    if (map.find(data)!= map.end()) { // 如果找到, 则将节点移到头部, 并将
        CacheList* L = map[data]; // 注意: 如果map[data]->time > 3, 则需要添加到LFUcache中
        L->time += 1;
        // 删除之后，在头部添加相应的节点
        DetachNode(L, false); // 从节点中移除
        Insert(L, false);     // 把这个节点放在头结点(第一个访问的节点)
        if (L->time >= transform_time) return true;  // 在LFU中加入一次
    }
    else { // map中没有找到
        CacheList* L = new CacheList(); // 新建缓存节点
        map[data] = L;  // 将节点存储在哈希表中
        if (this->size == this->capacity) {
            DeleteTail(false);
            // 删除节点，探后放进ghost中
        }
        L->key = data;
        L->time = 1;    // 使用次数为1
        Insert(L, false); // 在缓存中插入节点
    }
    return false;
}

// 使用头插方法，仅插入节点, 不考虑删除
void LRUcache::Insert(CacheList *L, bool is_ghost) {
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
void LRUcache::DetachNode(CacheList* L, bool is_ghost) {
    // 注意需要判断是否为ghost，虽然代码相似，但是一个是size-1, 另一个ghost_size-1
    if (!is_ghost) { // 暂时移除指针，不抹除哈希表中的元素
        if (this->size == 0) {
            throw runtime_error("nothing to detach!");
        }
        if (this->size == 1) {
            this -> cache = nullptr;
        }
        else if (L == cache) { // 暂时清除头结点
            this -> cache = L->next;
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
        this->ghost_map.erase(L->key);
        // 注意: 由于之后指针会往主list中放, 不释放指针
        if (this->ghost_size == 1) {
            this -> ghost = nullptr;
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

// 删除尾部元素并删除哈希表对应元素, 如果是删除主表的元素，则加入ghost中
void LRUcache::DeleteTail(bool is_ghost) {
    if (!is_ghost) {
        CacheList* remove;
        if (!this->cache) throw("no element to delete");
        else if (this->size == 1) {
            remove = cache;
            this -> cache = nullptr; // 更新头结点
        }
        else {
            // 移除 cache的prev节点即尾结点
            remove = cache->prev;
            CacheList* q = remove->prev;
            q->next = cache;
            cache->prev = q;
        }
        // 重整remove节点
        remove->prev = nullptr; remove->next = nullptr; remove->time = 0;
        // 清除哈希表中的元素
        map.erase(remove->key);

        if (this -> ghost_size == this -> ghost_capacity) DeleteTail(true);
        // 对于ghost缓存满，则先移除元素, 再插入到ghost数组中, 不满则不移除元素
        // 然后将元素加入ghost中,显然ghost中是没有对应的项的
        this -> ghost_map[remove->key] = remove;
        Insert(remove, true);
        this->size -= 1;
    }
    else { // 删除ghost数组中的尾部并移除
        if (!this->ghost) throw("no element to delete");
        CacheList* remove;
        if (this->ghost_size == 1) { 
            // 对于ghost中有一个元素并被取出时，删除头结点
            remove = this -> ghost;
            this->ghost = nullptr;
        }
        else {
            remove = ghost->prev;
            CacheList* q = remove->prev;
            q->next = ghost;
            ghost->prev = q;
        }
        ghost_map.erase(remove->key); // 删除哈希表元素
        delete remove; // 释放存储空间
        this -> ghost_size -= 1;
    }
}

// 插入元素，并且扩容一格(注意需要判断最大空间)
void LRUcache::Add(DataType data) {
    // 这个对应的是在ghost中找到的情况，需要删除ghost中对应的节点
    this->capacity += 1;
    CacheList* L = new CacheList();
    L->key = data;
    L->time = 1;
    this->map[data] = L; // 直接塞入哈希表
    Insert(L, false);    // 插入节点 
}

// 删除最后一个节点，并且容量缩减一格, 返回是否成功缩减subtract
bool LRUcache::Subtract() {
    if (this->capacity == 0) return false;
    // 如果表满,则删除节点并放入ghost数组, 否则不用删除节点
    if (this -> size == this ->capacity) DeleteTail(false);  
    this->capacity -= 1;
    return true;
}

// 在ghost中进行查找，如果有则删除对应ghost中的元素并返回true(接下来总函数会调用put)
bool LRUcache::check_ghost(DataType data) {
    // 注意使用find而不使用ghost_map[data]; 因为这样会新增元素
    if (this->ghost_map.find(data)!= this ->ghost_map.end()) {
        DetachNode(ghost_map[data], true);
        return true;
    }
    return false;
}

void LRUcache::Show(bool show_ghost) {
    CacheList* p = this->cache;
    cout << "=============== Last recently used =========" << endl;
    if (!p) {
        cout << "----- empty list : nothing to show -----" << endl;
        return;
    }
    for (; p->next!= cache;p = p->next) {
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

