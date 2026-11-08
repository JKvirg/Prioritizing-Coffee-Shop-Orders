// CMSC 341 - Spring 2026 - Project 3
#include "shop.h"
Shop::Shop() { // empty object
    // Initalize to Default Values/Nullptr
    m_shopID = 0;
    m_heap = nullptr;
    m_size = 0;
    m_priorFunc = nullptr;
    m_heapType = NOTYPE;
    m_structure = NOSTRUCT;
    m_regPrior = 0;
}

Shop::Shop(prifn_t priFn, HEAPTYPE heapType,
    STRUCTURE structure, int regPrior,
    int id)
{
    if (id < SHOPMINID || id > SHOPMAXID) {
        throw out_of_range("Invalid Shop ID");
    }

    // Initalize Values
    m_priorFunc = priFn;
    m_heapType = heapType;
    m_structure = structure;
    m_regPrior = regPrior;
    m_shopID = id;

    // Default Values
    m_heap = nullptr;
    m_size = 0;
}

Shop::~Shop()
{
    clear();
}

void Shop::clear() {
    clearHelper(m_heap);

    // Reset Object to Default Values/Nullptr
    m_heap = nullptr;
    m_size = 0;

    m_priorFunc = nullptr;
    m_heapType = NOTYPE;
    m_structure = NOSTRUCT;
    m_regPrior = 0;
    m_shopID = 0;
}

Shop::Shop(const Shop& rhs)
{
    // Copy Variable Values
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_regPrior = rhs.m_regPrior;
    m_shopID = rhs.m_shopID;

    // Deep Copy of the Heap
    m_heap = copyHelper(rhs.m_heap);
}

Shop& Shop::operator=(const Shop& rhs) {
    // Case: Self Assignment
    if (this == &rhs) return *this;

    // Clear Current Object
    clear();

    // Copy Variable Values
    m_size = rhs.m_size;
    m_priorFunc = rhs.m_priorFunc;
    m_heapType = rhs.m_heapType;
    m_structure = rhs.m_structure;
    m_regPrior = rhs.m_regPrior;
    m_shopID = rhs.m_shopID;

    // Deep Copy
    m_heap = copyHelper(rhs.m_heap);

    return *this;
}

void Shop::mergeWithQueue(Shop& rhs) {
    // Case: Self Merge
    if (this == &rhs) {
        throw domain_error("Cannot Merge Heap with itself");
    }

    // Case: Different Priority Type/ Heap Type
    if (m_priorFunc != rhs.m_priorFunc || m_structure != rhs.m_structure || m_heapType != rhs.m_heapType) {
        throw domain_error("Incompatible Heaps");
    }

    // Merge Heaps
    m_heap = mergeHelper(m_heap, rhs.m_heap);

    // Update Size
    m_size += rhs.m_size;

    // Empty rhs heap
    rhs.m_heap = nullptr;
    rhs.m_size = 0;
}

bool Shop::insertOrder(const Order& order) {
    // Check Priority Func
    if (m_priorFunc == nullptr) return false;

    // Check Order for Invalid IDs
    if (order.m_customerID < MINCUSTID || order.m_customerID > MAXCUSTID) {
        return false;
    }
    if (order.m_orderID < MINORDERID || order.m_orderID > MAXORDERID) {
        return false;
    }

    // Create New Node
    Order* newNode = new Order(order);

    newNode->m_left = nullptr;
    newNode->m_right = nullptr;
    newNode->m_npl = 0;

    // Merge Node
    m_heap = mergeHelper(m_heap, newNode);

    // Update Size
    m_size += 1;

    return true;
}

int Shop::numOrders() const
{
    return m_size;
}

prifn_t Shop::getPriorityFn() const {
    return m_priorFunc;
}

Order Shop::getNextOrder() {
    // Case: Empty Heap
    if (m_heap == nullptr) throw out_of_range("Heap is Empty");

    // Save Root Order to Return Later
    // Also Save Pointers to its Children's Location
    Order topOrder = *m_heap;
    Order* leftChild = m_heap->m_left;
    Order* rightChild = m_heap->m_right;

    // Remove Root Node from the Heap Structure
    delete m_heap;

    // Merge Children into New Heap
    m_heap = mergeHelper(leftChild, rightChild);

    // Update Size
    m_size -= 1;

    return topOrder;
}

void Shop::setPriorityFn(prifn_t priFn, HEAPTYPE heapType) {
    // Save Heap
    Order* oldHeap = m_heap;

    // Reset Heap
    m_heap = nullptr;
    m_size = 0;

    // Update Priority Func
    m_priorFunc = priFn;
    m_heapType = heapType;

    // Rebuild
    rebuildHelper(oldHeap);
}

void Shop::setStructure(STRUCTURE structure) {
    // Save Heap
    Order* oldHeap = m_heap;

    // Reset Heap
    m_heap = nullptr;
    m_size = 0;

    // Update Structure Type
    m_structure = structure;

    // Rebuild
    rebuildHelper(oldHeap);
}

HEAPTYPE Shop::getHeapType() const {
    return m_heapType;
}

STRUCTURE Shop::getStructure() const {
    return m_structure;
}

void Shop::printOrdersQueue() const {
    // Print Info
    cout << "Contents of the queue:" << endl;

    printHelper(m_heap);
}

void Shop::dump() const {
    if (m_size == 0) {
        cout << "Empty heap.\n";
    }
    else {
        cout << "Shop " << m_regPrior << ": => ";
        dump(m_heap);
    }
    cout << endl;
}

void Shop::dump(Order* pos) const {
    if (pos != nullptr) {
        cout << "(";
        dump(pos->m_left);
        if (m_structure == SKEW)
            cout << m_priorFunc(*pos) << ":" << pos->m_orderID;
        else
            cout << m_priorFunc(*pos) << ":" << pos->m_orderID << ":" << pos->m_npl;
        dump(pos->m_right);
        cout << ")";
    }
}

ostream& operator<<(ostream& sout, const Order& order) {
    sout << "Order ID: " << order.getOrderID()
        << ", item: " << order.getItem()
        << ", count: " << order.getCount();
    return sout;
}

//////////////////////////////////////////////////////////////
Region::Region(int size) {
    // Initalize Heap
    m_capacity = size;
    m_size = 0;

    // Array is Size Plus 1 so Tree Math is Simple
    // i is position of Given Node
    // Its Parent = i/2
    // Its Left Child = 2i
    // Its Right Child = 2i + 1
    m_heap = new Shop[m_capacity + 1];
}
Region::~Region() {
    delete[] m_heap;
    m_heap = nullptr;
    m_size = 0;
    m_capacity = 0;
}
bool Region::addShop(Shop& aShop) {
    // Case: Array is Full
    if (m_size >= m_capacity) return false;

    // Insert Shop
    m_size += 1;
    m_heap[m_size] = aShop;

    // Bubble Up
    int index = m_size;
    while (index > 1) {
        int parent = index / 2;
        if (m_heap[index].m_regPrior < m_heap[parent].m_regPrior) {
            Shop temp = m_heap[index];
            m_heap[index] = m_heap[parent];
            m_heap[parent] = temp;

            index = parent;
        }
        else break;
    }

    return true;
}

bool Region::getShop(Shop& aShop) {
    // Case: Empty
    if (m_size <= 0) return false;

    // Return Root Node
    aShop = m_heap[1];

    // Move Last Element to Root
    m_heap[1] = m_heap[m_size];

    // Reduce Size
    m_size -= 1;

    // Bubble Down
    int index = 1;
    while (true) {
        int left = 2 * index;
        int right = (2 * index) + 1;
        int smallest = index;

        // Find Node with Highest Priority
        if (left <= m_size && m_heap[left].m_regPrior < m_heap[smallest].m_regPrior) {
            smallest = left;
        }
        if (right <= m_size && m_heap[right].m_regPrior < m_heap[smallest].m_regPrior) {
            smallest = right;
        }

        // Root is Highest Priority Thus Break
        if (smallest == index) break;

        // Swap
        Shop temp = m_heap[index];
        m_heap[index] = m_heap[smallest];
        m_heap[smallest] = temp;

        index = smallest;
    }

    return true;
}

bool Region::getNthShop(Shop& aShop, int n) {
    // Case: Empty
    if (n <= 0 || n > m_size) return false;

    // Array to Put in N amount of Shops to Restore Later
    Shop* temp = new Shop[n];

    // Extract N shops
    for (int i = 0; i < n; i++) {
        getShop(temp[i]);
    }

    // Restore all except Nth
    for (int i = 0; i < n - 1; i++) {
        addShop(temp[i]);
    }

    // Set aShop to Nth and Delete temp array
    aShop = temp[n - 1];
    delete[] temp;
    return true;
}

bool Region::setPriorityFn(prifn_t priFn, HEAPTYPE heapType, int n) {
    // This function converts the type of heap in the Nth highest priority Shop. If the operation is successful it returns true.
    
    // Case: Empty
    if (n <= 0 || n > m_size) return false;

    // Find Shop
    Shop tempShop;
    getNthShop(tempShop, n);

    // Set Priority Func
    tempShop.setPriorityFn(priFn, heapType);

    // Add Back the Modified Nth Shop
    addShop(tempShop);

    return true;
}

bool Region::setStructure(STRUCTURE structure, int n) {
    // Case: Empty
    if (n <= 0 || n > m_size) return false;

    // Find Shop
    Shop tempShop;
    getNthShop(tempShop, n);

    // Set Priority Func
    tempShop.setStructure(structure);

    // Add Back the Modified Nth Shop
    addShop(tempShop);

    return true;
}

bool Region::getOrder(Order& order) {
    // Case: Empty
    if (m_size == 0) return false;

    // Find Shop Containing Highest Priority Order
    Shop tempShop;
    while (m_size > 0) {
        getShop(tempShop);

        // Check If Shop is Empty
        if (tempShop.numOrders() > 0) {
            order = tempShop.getNextOrder();

            // Add Back the Shop
            addShop(tempShop);
            return true;
        }
    }
    // Return false if no shop with an order was Found
    return false;
}

void Region::dump() {
    dump(ROOTINDEX);
    cout << endl;
}

void Region::dump(int index) {
    if (index <= m_size) {
        cout << "(";
        dump(index * 2);
        cout << m_heap[index].m_regPrior;
        dump(index * 2 + 1);
        cout << ")";
    }
}