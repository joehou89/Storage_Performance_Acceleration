class LRUCache
{
public:

  /*双向链表*/
  class DlinkedNode 
  {
    public:
      int _key;
      int _value;
      DlinkedNode *_prev;
      DlinkedNode *_next;
      bool _virtual_node;

    public:
      DlinkedNode():_key(0), _value(0), _prev(nullptr), _next(nullptr), _virtual_node(true)
      {
        // std::cout << "DlinkedNode()" << std::endl;
      }
      DlinkedNode(int key, int value): _key(key), _value(value), _prev(nullptr), _next(nullptr), _virtual_node(false)
      {
        // std::cout << "DlinkedNode(), key: " << key << ", value: " << value << std::endl;
      }
  };

  /*哈希表*/
  std::unordered_map<int, DlinkedNode *> hash_table;
  int size;     /*缓存元素个数*/
  int capacity; /*容量大小*/
  DlinkedNode *head, *tail;
  

public:
  LRUCache(int num): size(0), capacity(num)
  {
    /*使用虚拟头部和虚拟尾部节点*/
    head = new DlinkedNode();
    tail = new DlinkedNode();

    /*虚拟头节点和虚拟尾节点互联*/
    head->_next = tail;
    head->_prev = nullptr;
    tail->_prev = head;
    tail->_next = nullptr;
  }

  int get(int key)
  {
    std::unordered_map<int, DlinkedNode *>::iterator it = hash_table.find(key);
    if (it == hash_table.end())
    {
      return -1;
    }
    DlinkedNode *node = it->second;
    cache_move_to_head(node);
    return node->_value;
  }

  void put(int key, int value)
  {
    std::unordered_map<int, DlinkedNode *>::iterator it;
    it = hash_table.find(key);
    if (it == hash_table.end())
    {
      DlinkedNode *new_node = new DlinkedNode(key, value);
      size++;
      if (size > capacity)
      {
        DlinkedNode remove_node = cache_remove_tail();
        hash_table.erase(remove_node._key);
        size--;
      }

      /*插入元素*/
      hash_table.insert({key, new_node});
      cache_add_to_head(new_node);
    }
    else
    {
      it->second->_value = value;
      cache_move_to_head(it->second);
    }
  }

private:
  void cache_add_to_head(DlinkedNode *node)
  {
    node->_prev = head;
    node->_next = head->_next;
    head->_next->_prev = node;
    head->_next = node;
  }

  void cache_remove_node(DlinkedNode *node)
  {
    node->_prev->_next = node->_next;
    node->_next->_prev = node->_prev;
  }

  void cache_move_to_head(DlinkedNode *node)
  {
    cache_remove_node(node);
    cache_add_to_head(node);
  }

  DlinkedNode cache_remove_tail()
  {
    DlinkedNode *res = tail->_prev;
    cache_remove_node(res);
    return *res;
  }

public:
  void cache_print()
  {
    DlinkedNode *p = this->head;
    while (p != nullptr)
    {
      if (!p->_virtual_node)
      {
          std::cout << "key: " << p->_key << ", value: " << p->_value << std::endl;
      }
      p = p->_next;
    }
    return;
  }

};
