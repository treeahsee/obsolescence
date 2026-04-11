# Advanced Indexing
| Tree  | Strength |
| ------ | -------- |
| Trie | String Search | 
| Inverted index | full text search across many docs |
| R-Tree | Spatial data|
| Learned Index | ML to learn indexes |

## Trie
- Trie
    - Comes from word reTRIEval
    - Design for prefix queries
    - A Tries is a tree where each node contains a single or set of characters between keys. Each path represents a string.
- Patricia Trie
    - more space efficiient
    - merge sequence of single characters nodes in to a single node. Nodes can contain multiple characters or larger substrings.
    - Nodes split upon instertion if there are common substrings. 
        - insert hello 
        - root  node -> hello
        - insert helium
        - root node -> hel -> ium and hel -> lo
    - Retrival root node to leaf 

### Limitations of B+ Tree
- Node traversal across string requires traversing lengthing redundant key causing slow search

### Trie Implementation
``` C++
class PatriciaNode {
    public:
        bool is EndOfWord;
        int value;
        std::map<std::string, PatriciaNode *> children;

        PatriciaNode() : isEndOfWord(false){}
};
```

``` C++
void insertHelper(PatriciaNode *node, const std::string &word, size_T index) {
    //Termination case: If the whole word has been processed
    if (index == word.length()) {
        node->isEndOfWord = true;
        return;
    }
    //Prepare the substring from the current index to the end
    std::string remanining = word.substr(index)
}
```
Insertion: Identifying Common Prefixes
``` C++
for (auto &child: node ->children) {
    // Calculate common prefix length
    const std::string &key = child.first;
    PatriciaNode *childNode = child.second;
    size_t commonLength = 0;
    while (commonLength < key.length() && commonLength < remanining.length() && key[commonLength] == remaning[commonLength]) {
        commonLength++;
    }
}
```

``` C++
if (commonLength > 0) {
if (commonLength < key.length()) {
// Split the child's key
std :: string newKey = key.substr(0, commonLength); // "hel"
std :: string oldKey = key.substr(commonLength); // "lo"
PatriciaNode *newChildNode = new PatriciaNode();
newChildNode->children[oldKey] = childNode;
node->children[newKey] = newChildNode;
node->children.erase(key);
childNode = newChildNode;

// remainder: "ium"
insertHelper(childNode, word, index + commonLength, value);
return;
```

### Complexity Comparison of Trie and B+ Trie
- L :  Avg. Length of the key
- N : Number of keys


| Complexity | Patricia Trie | B+ Tree |
| ------ | ----- | ----- |
| Insertion Time | O(L) | O(log N) |
| Search Time | O(L) | O(Log N) |
| Space Complexity | O(N * L) | O(N) |

### Qualitivate Comparison of Trie and B+ Tree
| Use | Patricia Trie | B+ Tree |
| --- | --- | --- |
| Target Use Case | Fast prefix queries | Fast range queries |
| Memory Efficiency | Less efficient with dissimilar keys |  More efficient stores many keys per node | 
| Range Queries | Supported but not efficient | Highly efficient|

## Inverted Index
- Map words to the positions in which they appear in a document.
- proximity search distance between words

| Word | Position | 
| --- | --- |
| russia | 10, 20 |
| napolean | 25 |

Library Search 

| Word | Position | 
| --- | --- |
| russia | (1, 10) , (1, 20) |
| napolean | (1, 25), (2, 30), (3, 50) |

- internals is usually a hash table

``` C++
std::unordered_map <
    std::string, // key
    std::unordered_map<int, std::vector<int>> // {doc # {position #}}
>
```

### Core Functions
 - addDocument
    ``` C++
    void addDocument(int docID, const std :: string &content) {
        documents.push_back(content);
        std :: vector<std :: string> words = split(toLower(content));
        for (size_t i = 0; i < words.size(); ++i) {
            index[words[i]][docID].push_back(i);
        }
    }
    ```
 - getDocuments
    ``` C++
    std :: unordered_set<int> getDocuments(
    const std :: string &word) {
    std :: string lowerWord = toLower(word);
    if (index.find(lowerWord) != index.end()) {
        std :: unordered_set<int> docIDs;
        for (const auto &entry : index[lowerWord]) {
            doclDs.insert(entry.first);
            return docIDs;
        }
    return {};
    }
    ```
 - proximitySearch
    ``` C++
    std :: unordered_set<int> proximitySearch(const std :: string &word1, const std :: string &word2, int k) {
    if (index.find(lowerWord1) != index.end() && index.find(lowerWord2) != index.end()) {
        for (const auto &entry1 : index[lowerWord1]) {
            int doclD = entry1.first;
            if (index[lowerWord2].find(docID) != index[lowerWord2].end()) {
                const auto &positions1 = entry1.second;
                const auto &positions2 = index[lowerWord2].at(docID);
                for (int pos1 : positions1) { 
                    for (int pos2 : positions2) {
                        if (std :: abs(pos1 - pos2) <= k) { result.insert(docID); break; }
                    }
                }
            }
        }
    }
    ```
### Benefits
- Efficient - Fast Lookup for indivdual words
- Complexity - Proximity search and Boolean queries
- Scalable - scales to alrge document collections

### Challenges with Inverted Index
- Large Storage Footprint
- Compression : Delta Encoding
- Disk-Based Storage Management

### Web-Scale Search
- Sharding
- Replication

### Distributed Inverted Index
- Word based sharding
    - shard 1 words 1-100, replicas server A,B
    - shard 2 words 101 -- 200, replicas servers C,D

## Rtree
- Well suited for spatial indexing and nearest neighbhor query

### Limitations of B+ Tree
- Limited in handling multi dimensial data

## R-Tree: A Multidemnsional Index
- We can represent as a point and group heirarchaly in clusters
- Balanced tree
- Data types
    - points
    - rectangles
- Nodes
    - leaf nodes
        - points
    - inner nodes
        - child nodes and their bounding rectangles
    - root node
        - top most inner node
        - entry point
- Point
    ``` C++
    struct Point {
        float x,y;
        Point(float x, float y) : x(x), y(y) {}
    };
    ```
- Rectangle
    ``` C++
    struct Rectangle {
        float minX, minY,maxX,maxY;
        Rectangle(float minX, float minY, float maxX, float maxY)
            : minx(minX), minY(minY), maxX(maxX), maxY(maxY) {}

        bool contains(const Point &p);
        bool intersects(const Rectangle &other) const;
    }
    ```
- Rectangle: Contains
    ``` C++
    bool contains(const Point &p) {
        return (p.x >= minX && p.x <= maxX
                && p.y >= minY && p.y <= maxY)
    }
    ```
- Rectangle: Intersects
    ``` C++
    bool intersects(const Rectangle &other) conts {
        return !(other.mixX > maxX || other.maxX < minX
                || other.minY > maxY || other.maxY < minY>)
    }
    ```

- Inserts
    - Find best child node based on least enlargment of bounding rectangle
    ``` C++
    void insert(RTreeNode * node, const Point &point, const Rectangle &rect) {
    ...
        else {
            int bestChild = chooseBestChild(node, rect);
            insert(node->children[bestChild], point, rect);
            node->childrenRectangles[bestChild].expand(rect);
        }
    }
    ```

- Choose Best Child
    ``` C++
    int chooseBestChild(RTreeNode * node, const Rectangle &rect) {
    int bestChild = 0;
        for (size_t i = 0; i < node->children.size(); ++i) {
            Rectangle enlarged = node->childrenRectangles[i];
            enlarged.expand(rect);
            float enlargement = ...
            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                bestChild = i;
            }
            return bestChild;
        }

    }
    ```
- Node Spitting
    - Pick seeds which are points in the bounding box that are furthest apart from eachother

### R-Tree Supported Queries and Applications
- Multi dimensional Range Query
    - salary > 100k  age > 35
- Nearest Neighbhor
- Applications
    - Find nearest driver to rider location
    - Find nearest resteraunt to user location
    - game engines

### ND R-Tree
- generalizes R-Tree to n-dimensions

## Learned Indexed
- All nodes are ML models
- Reduce memory usage and faster lookups

### Limitations of B+ Tree
- Fixed Structure
- Memory Overhead
- Rigid Paritioning
