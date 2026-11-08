// CMSC 341 - Spring 2026 - Project 3
#ifndef SHOP_H
#define SHOP_H
#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;
class Grader;   // forward declaration (for grading purposes)
class Tester;   // forward declaration
class Shop;     // forward declaration
class Order;    // forward declaration
class Region;   // forward declaration
#define ROOTINDEX 1
#define EMPTY Order("",1,0)
#define DEFAULTORDERID  0
const int SHOPMINID = 100001;
const int SHOPMAXID = 999999;
const int MINCUSTID = 100001;// minimum customer ID
const int MAXCUSTID = 999999;// maximum customer ID
// use a different seed in the Random class to get different series of numbers 
const int MINORDERID = 100001;// minimum order ID
const int MAXORDERID = 999999;// maximum order ID
// TIER1 is the most valuable customer
enum MEMBERSHIP { TIER1, TIER2, TIER3, TIER4, TIER5, TIER6 };// use with MinHeap
// COFFEE is the most profitable item
enum ITEM { COFFEE, LATTE, SOFTDRINK, MILK, WATER, ICEDTEA };// use with MinHeap
enum COUNT { ONE, PAIR, HALFDOZEN, DOZEN };// use with MaxHeap
const int MINPOINTS = 0; // the points colleted so far, use with MaxHeap
const int MAXPOINTS = 5000; // the points colleted so far, use with MaxHeap

enum HEAPTYPE { MINHEAP, MAXHEAP, NOTYPE };
enum STRUCTURE { SKEW, LEFTIST, NOSTRUCT };
// Priority function pointer type
typedef int (*prifn_t)(const Order&);

class Order {
    // stores a customer's order
public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    friend class Shop;
    Order(ITEM item = COFFEE, COUNT count = ONE,
        MEMBERSHIP membership = TIER5, int points = 0,
        int customerID = 0, int orderID = 0)
    {
        m_item = item; m_count = count; m_membership = membership;
        m_points = points; m_customerID = customerID; m_orderID = orderID;
        m_left = nullptr; m_right = nullptr; m_npl = 0;
    }
    ITEM getItem() const { return m_item; }
    int getOrderID() const { return m_orderID; }
    COUNT getCount() const { return m_count; }
    int getCustomerID() const { return m_customerID; }
    MEMBERSHIP getMemebership() const { return m_membership; }
    int getPoints() const { return m_points; }
    void setItem(ITEM item) { m_item = item; }
    void setOrderID(int id) { m_orderID = id; }
    void setCount(COUNT count) { m_count = count; }
    void setCustomerID(int id) { m_customerID = id; }
    void setMembership(MEMBERSHIP membership) { m_membership = membership; }
    void setPoints(int points) { m_points = points; }
    void setNPL(int npl) { m_npl = npl; }
    int getNPL() const { return m_npl; }
    // Overloaded assignment operator
    const Order& operator=(const Order& rhs) {
        if (this != &rhs) {
            m_item = rhs.m_item;
            m_count = rhs.m_count;
            m_membership = rhs.m_membership;
            m_points = rhs.m_points;
            m_customerID = rhs.m_customerID;
            m_orderID = rhs.m_orderID;
        }
        return *this;
    }
    string getTierString() const {
        string result = "UNKNOWN";
        switch (m_membership)
        {
        case TIER1: result = "Tier 1"; break;
        case TIER2: result = "Tier 2"; break;
        case TIER3: result = "Tier 3"; break;
        case TIER4: result = "Tier 4"; break;
        case TIER5: result = "Tier 5"; break;
        case TIER6: result = "Tier 6"; break;
        default: break;
        }
        return result;
    }
    string getItemString() const {
        string result = "UNKNOWN";
        switch (m_item)
        {
        case COFFEE: result = "Coffee"; break;
        case LATTE: result = "Latte"; break;
        case SOFTDRINK: result = "Soft Drink"; break;
        case MILK: result = "Milk"; break;
        case WATER: result = "Water"; break;
        case ICEDTEA: result = "Iced Tea"; break;
        default: break;
        }
        return result;
    }
    string getCountString() const {
        string result = "UNKNOWN";
        switch (m_count)
        {
        case ONE: result = "1"; break;
        case PAIR: result = "2"; break;
        case HALFDOZEN: result = "6"; break;
        case DOZEN: result = "12"; break;
        default: break;
        }
        return result;
    }
    // Overloaded insertion operator
    friend ostream& operator<<(ostream& sout, const Order& order);

private:
    int m_customerID;// a unique ID number identifying the customer
    int m_orderID;   // a unique ID number identifying the order
    int m_points;   // points collected by customer
    MEMBERSHIP m_membership; // the customer membership tier
    ITEM m_item;        // the ordered item
    COUNT m_count;      // the count of ordered item

    Order* m_right;    // right child
    Order* m_left;     // left child
    int m_npl;          // null path length for leftist heap

};

class Shop {
    // stores the skew/leftist heap, minheap/maxheap
public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes
    friend class Region;

    Shop();
    Shop(prifn_t priFn, HEAPTYPE heapType,
        STRUCTURE structure, int regPrior,
        int id);
    ~Shop();
    Shop(const Shop& rhs);
    Shop& operator=(const Shop& rhs);
    bool insertOrder(const Order& order);
    Order getNextOrder(); // Return the highest priority order
    void mergeWithQueue(Shop& rhs);
    void clear();
    int numOrders() const; // Return number of orders in queue
    void printOrdersQueue() const; // Print the queue using preorder traversal
    prifn_t getPriorityFn() const;
    // Set a new priority function. Must rebuild the heap!!!
    void setPriorityFn(prifn_t priFn, HEAPTYPE heapType);
    HEAPTYPE getHeapType() const;
    STRUCTURE getStructure() const;
    // Set a new data structure (skew/leftist). Must rebuild the heap!!!
    void setStructure(STRUCTURE structure);
    void dump() const; // For debugging purposes

private:
    int m_shopID;           // unique identifier
    Order* m_heap;         // Pointer to root of skew heap
    int m_size;             // Current size of the heap
    prifn_t m_priorFunc;    // Function to compute priority
    HEAPTYPE m_heapType;    // either a MINHEAP or a MAXHEAP
    STRUCTURE m_structure;  // skew heap or leftist heap
    int m_regPrior;         // this holds the priority of the shop

    void dump(Order* pos) const; // helper function for dump

    // Clear Helper
    void clearHelper(Order* node) {
        if (node == nullptr) return;

        clearHelper(node->m_left);
        clearHelper(node->m_right);

        delete node;
    }

    Order* copyHelper(const Order* node) {
        // Case: Empty Root Node
        if (node == nullptr) return nullptr;

        // Copy Node Data
        Order* newNode = new Order(*node);

        // Traverse Left and Right of Node
        // Copy to Left and Right Pointers of the Copied Parent
        newNode->m_left = copyHelper(node->m_left);
        newNode->m_right = copyHelper(node->m_right);

        return newNode;
    }

    Order* mergeHelper(Order* h1, Order* h2) {
        // Case: One of the Nodes are Empty
        if (h1 == nullptr) return h2;
        if (h2 == nullptr) return h1;

        // Determine Priority Type
        bool h1Higher = true;
        if (m_heapType == MINHEAP) {
            h1Higher = (m_priorFunc(*h1) <= m_priorFunc(*h2));
        }
        else {
            h1Higher = (m_priorFunc(*h1) >= m_priorFunc(*h2));
        }

        if (h1Higher == false) {
            Order* temp = h1;
            h1 = h2;
            h2 = temp;
        }

        // Merge H1->m_right with H2
        h1->m_right = mergeHelper(h1->m_right, h2);

        // Structure Handling Depending on If Skew or Leftist
        if (m_structure == SKEW) { // Case: Skew
            // Swap Children Nodes
            Order* temp = h1->m_right;
            h1->m_right = h1->m_left;
            h1->m_left = temp;
        }
        // Case: Leftist
        else {
            int leftNPL = 0;
            int rightNPL = 0;

            if (h1->m_left == nullptr) leftNPL = -1;
            else leftNPL = h1->m_left->m_npl;

            if (h1->m_right == nullptr) rightNPL = -1;
            else rightNPL = h1->m_right->m_npl;

            if (leftNPL < rightNPL) {
                Order* temp = h1->m_left;
                h1->m_left = h1->m_right;
                h1->m_right = temp;
            }

            // Update NPL
            if (h1->m_right == nullptr) h1->m_npl = 0;
            else h1->m_npl = h1->m_right->m_npl + 1;
        }

        return h1;
    }

    void rebuildHelper(Order* node) {
        // Case: Empty Node
        if (node == nullptr) return;

        // Save Children Nodes
        Order* leftChild = node->m_left;
        Order* rightChild = node->m_right;

        // Reset Node
        node->m_left = nullptr;
        node->m_right = nullptr;
        node->m_npl = 0;

        // Rebuild Heap
        m_heap = mergeHelper(m_heap, node);
        m_size += 1;

        rebuildHelper(leftChild);
        rebuildHelper(rightChild);
    }

    void printHelper(Order* node) const {
        // Case: Empty Heap
        if (node == nullptr) return;

        // Print Info
        cout << "[" << m_priorFunc(*node) << "] Order ID: " << node->m_orderID;
        cout << ", item: " << node->getItemString() << ", count: " << node->m_count << endl;

        // Preorder Traversal to Print Info of Nodes Children
        printHelper(node->m_left);
        printHelper(node->m_right);
    }
};

class Region {
public:
    friend class Grader; // for grading purposes
    friend class Tester; // for testing purposes

    Region(int size);
    ~Region();
    bool addShop(Shop& aShop); // enqueue function
    bool getShop(Shop& aShop); // dequeue function
    bool getOrder(Order& anOrder);
    bool getNthShop(Shop& aShop, int n);
    void dump(); // For debugging purposes
    // change priority function for the Nth highest priority shop
    bool setPriorityFn(prifn_t priFn, HEAPTYPE heapType, int n);
    // change structure for the Nth highest  priority shop
    bool setStructure(STRUCTURE structure, int n);
private:
    Shop* m_heap;              // Array to hold the heap
    int m_capacity;             // size of array
    int m_size;                 // Current size of the heap

    void dump(int index);       // helper function for dump
};
#endif