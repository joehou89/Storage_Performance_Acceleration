## LFU(least ferequently used)  
如何定义一批数据里的热点数据，条件如下：  
1.数据访问次数最多；  
2.数据访问时间最新；  
是根据每一个数据的访问次数和访问时间来判断的数据是否是热点数据，这一点和LRU的差别较大，所以是比较适合用在内存中。  
    
## 数据结构定义  
1.用于快速索引的表，使用std::unorderdmap<key, Node>;  
2.内存中用于存放真正写过的数据，但是这个数据的存放需要按照热点数据的规则来放，这里就不能够用list了，而是map或者set都可以，这两个时间复杂度差不多；  
  
## 结构体定义    
```c
//node定义结构体
struct Node {
    /*
    TODO:
    cnt:表示访问次数，可以用一个64位表示
    time:表示访问的最新时间
    */
    int cnt, time, key, value;

    Node(int _cnt, int _time, int _key, int _value):cnt(_cnt), time(_time), key(_key), value(_value){}
    
    bool operator < (const Node& rhs) const {
        return cnt == rhs.cnt ? time < rhs.time : cnt < rhs.cnt;
    }
};
```    
    
```c  
class LFUCache {
    // 缓存容量，时间戳
    int capacity, time;
    unordered_map<int, Node> key_table;
    set<Node> S;
public:
    ...
};
```       
    
## 优劣  
我看有的文章里讲到说LFU也不算是比较优秀的算法，但是至少针对热点数据的效率要好过LRU.但是细想，也的确未必是最优的，比如缓存的局部性原理具有一定的时间滞后性，但是存储空间的容量是一定的，  
可能存在说一部分数据的访问频率较高，但是不代表这些数据在未来一定会被访问到，历史的统计不代表未来的预测性，在容量一定的前提下，也可能会存在劣势。  
  


  
