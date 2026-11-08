// CMSC 341 - Spring 2026 - Project 3
#include "shop.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>
using namespace std;

enum RANDOM { UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE };
class Random {
public:
    Random() {}
    Random(int min, int max, RANDOM type = UNIFORMINT, int mean = 50, int stdev = 20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL) {
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor 
            m_normdist = normal_distribution<>(mean, stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = mt19937(10);// 10 is the fixed seed value
            m_unidist = uniform_int_distribution<>(min, max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = mt19937(10);// 10 is the fixed seed value
            m_uniReal = uniform_real_distribution<double>((double)min, (double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = mt19937(m_device());
        }
    }
    void setSeed(int seedNum) {
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = mt19937(seedNum);
    }
    void init(int min, int max) {
        m_min = min;
        m_max = max;
        m_type = UNIFORMINT;
        m_generator = mt19937(10);// 10 is the fixed seed value
        m_unidist = uniform_int_distribution<>(min, max);
    }
    void getShuffle(vector<int>& array) {
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i <= m_max; i++) {
            array.push_back(i);
        }
        shuffle(array.begin(), array.end(), m_generator);
    }

    void getShuffle(int array[]) {
        // this function provides a list of all values between min and max
        // in a random order, this function guarantees the uniqueness
        // of every value in the list
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i <= m_max; i++) {
            temp.push_back(i);
        }
        shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it = temp.begin(); it != temp.end(); it++) {
            array[i] = *it;
            i++;
        }
    }

    int getRandNum() {
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if (m_type == NORMAL) {
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while (result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT) {
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum() {
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = floor(result * 100.0) / 100.0;
        return result;
    }

    string getRandString(int size) {
        // the parameter size specifies the length of string we ask for
        // to use ASCII char the number range in constructor must be set to 97 - 122
        // and the Random type must be UNIFORMINT (it is default in constructor)
        string output = "";
        for (int i = 0;i < size;i++) {
            output = output + (char)getRandNum();
        }
        return output;
    }

    int getMin() { return m_min; }
    int getMax() { return m_max; }
private:
    int m_min;
    int m_max;
    RANDOM m_type;
    random_device m_device;
    mt19937 m_generator;
    normal_distribution<> m_normdist;//normal distribution
    uniform_int_distribution<> m_unidist;//integer uniform distribution
    uniform_real_distribution<double> m_uniReal;//real uniform distribution

};

// the followings are sample priority functions to be used by Shop class
// Priority functions compute an integer priority for a order.
int priorityFn1(const Order& order);// works with a MAXHEAP
int priorityFn2(const Order& order);// works with a MINHEAP

class Tester {
public:
    // Heap Property Checker
    bool checkHeapProperty(Order* node, prifn_t priFn, HEAPTYPE heapType, STRUCTURE structureType) {
        // Case: Empty
        if (node == nullptr) return true;

        // Case: Leftist
        if (structureType == LEFTIST) {
            int leftNPL;
            int rightNPL;

            if (node->m_left == nullptr) {
                leftNPL = -1;
            }
            else {
                leftNPL = node->m_left->m_npl;
            }

            if (node->m_right == nullptr) {
                rightNPL = -1;
            }
            else {
                rightNPL = node->m_right->m_npl;
            }

            if (leftNPL < rightNPL) return false;
        }

        // Check Left Child of Node
        if (node->m_left != nullptr) {
            if (heapType == MINHEAP && priFn(*node) > priFn(*(node->m_left))) {
                return false;
            }
            if (heapType == MAXHEAP && priFn(*node) < priFn(*(node->m_left))) {
                return false;
            }
        }

        // Check Right Child of Node
        if (node->m_right != nullptr) {
            if (heapType == MINHEAP && priFn(*node) > priFn(*(node->m_right))) {
                return false;
            }
            if (heapType == MAXHEAP && priFn(*node) < priFn(*(node->m_right))) {
                return false;
            }
        }

        // Make The Children of Node the Root of Subtree and Check
        return checkHeapProperty(node->m_left, priFn, heapType, structureType) && 
            checkHeapProperty(node->m_right, priFn, heapType, structureType);
    }

    // Test Functions for Shop Class
    // Test Default Constructor
    bool testDefaultConstructorShop() {
        Shop shop;

        // Check for Default Values
        if (shop.m_shopID != 0) {
            cout << "FAILED: shopID not equal to 0.\n";
            return false;
        }
        if (shop.m_heap != nullptr) {
            cout << "FAILED: heap not equal to nullptr.\n";
            return false;
        }
        if (shop.m_size != 0) {
            cout << "FAILED: size not equal to 0.\n";
            return false;
        }
        if (shop.m_priorFunc != nullptr) {
            cout << "FAILED: priorFunc not equal to nullptr.\n";
            return false;
        }
        if (shop.m_heapType != NOTYPE) {
            cout << "FAILED: heapType not equal to \"NOTYPE\".\n";
            return false;
        }
        if (shop.m_structure != NOSTRUCT) {
            cout << "FAILED: structure not equal to \"NOSTRUCT\".\n";
            return false;
        }
        if (shop.m_regPrior != 0) {
            cout << "FAILED: regPrior not equal to 0.\n";
            return false;
        }

        return true;
    }
    // Test Copy Constructor
    bool testCopyConstructorDefaultShop() {
        Shop original;

        Shop copy(original);

        // Check Values of Original
        if (original.m_shopID != 0) {
            cout << "FAILED: shopID not equal to 0.\n";
            return false;
        }
        if (original.m_heap != nullptr) {
            cout << "FAILED: heap not equal to nullptr.\n";
            return false;
        }
        if (original.m_size != 0) {
            cout << "FAILED: size not equal to 0.\n";
            return false;
        }
        if (original.m_priorFunc != nullptr) {
            cout << "FAILED: priorFunc not equal to nullptr.\n";
            return false;
        }
        if (original.m_heapType != NOTYPE) {
            cout << "FAILED: heapType not equal to \"NOTYPE\".\n";
            return false;
        }
        if (original.m_structure != NOSTRUCT) {
            cout << "FAILED: structure not equal to \"NOSTRUCT\".\n";
            return false;
        }
        if (original.m_regPrior != 0) {
            cout << "FAILED: regPrior not equal to 0.\n";
            return false;
        }

        // Check Values of Copy
        if (copy.m_shopID != 0) {
            cout << "FAILED: shopID not equal to 0.\n";
            return false;
        }
        if (copy.m_heap != nullptr) {
            cout << "FAILED: heap not equal to nullptr.\n";
            return false;
        }
        if (copy.m_size != 0) {
            cout << "FAILED: size not equal to 0.\n";
            return false;
        }
        if (copy.m_priorFunc != nullptr) {
            cout << "FAILED: priorFunc not equal to nullptr.\n";
            return false;
        }
        if (copy.m_heapType != NOTYPE) {
            cout << "FAILED: heapType not equal to \"NOTYPE\".\n";
            return false;
        }
        if (copy.m_structure != NOSTRUCT) {
            cout << "FAILED: structure not equal to \"NOSTRUCT\".\n";
            return false;
        }
        if (copy.m_regPrior != 0) {
            cout << "FAILED: regPrior not equal to 0.\n";
            return false;
        }

        return true;
    }
    bool testCopyConstructorNormalShop() {
        Shop original(priorityFn2, MINHEAP, SKEW, 1, SHOPMINID);

        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID, MINORDERID);
        Order o2(ICEDTEA, DOZEN, TIER6, MAXPOINTS, MAXCUSTID, MAXORDERID);

        original.insertOrder(o1);
        original.insertOrder(o2);

        Shop copy(original);

        if (copy.m_size != original.m_size) {
            cout << "FAILED: Sizes not the same.\n";
            return false;
        }
        if (copy.m_heap == original.m_heap) {
            cout << "FAILED: Not a Deep Copy.\n";
            return false;
        }

        return true;
    }

    // Test Assignment Operator
    bool testAssignmentNormalShop() {
        Shop original(priorityFn2, MINHEAP, SKEW, 1, SHOPMINID);

        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID, MINORDERID);
        Order o2(ICEDTEA, DOZEN, TIER6, MAXPOINTS, MAXCUSTID, MAXORDERID);

        original.insertOrder(o1);
        original.insertOrder(o2);

        Shop copy;
        copy = original;

        // Check Values
        if (copy.m_shopID != original.m_shopID) {
            cout << "FAILED: shopID not equal to original.\n";
            return false;
        }
        if (copy.m_size != original.m_size) {
            cout << "FAILED: size not equal to original.\n";
            return false;
        }
        if (copy.m_priorFunc != original.m_priorFunc) {
            cout << "FAILED: priorFunc not equal to original.\n";
            return false;
        }
        if (copy.m_heapType != original.m_heapType) {
            cout << "FAILED: heapType not equal to original.\n";
            return false;
        }
        if (copy.m_structure != original.m_structure) {
            cout << "FAILED: structure not equal to original.\n";
            return false;
        }
        if (copy.m_regPrior != original.m_regPrior) {
            cout << "FAILED: regPrior not equal to original.\n";
            return false;
        }
        if (copy.m_size != original.m_size) {
            cout << "FAILED: size not equal to original.\n";
            return false;
        }

        return true;
    }
    bool testAssignmentSelfShop() {
        Shop shop(priorityFn2, MINHEAP, SKEW, 1, SHOPMINID);

        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID, MINORDERID);
        Order o2(ICEDTEA, DOZEN, TIER6, MAXPOINTS, MAXCUSTID, MAXORDERID);

        shop.insertOrder(o1);
        shop.insertOrder(o2);

        Shop copy;
        shop = shop;

        // Check for Default Values
        if (shop.m_shopID != SHOPMINID) {
            cout << "FAILED: shopID not equal to SHOPMINID.\n";
            return false;
        }
        if (shop.m_heap == nullptr) {
            cout << "FAILED: heap equal to nullptr.\n";
            return false;
        }
        if (shop.m_priorFunc != priorityFn2) {
            cout << "FAILED: priorFunc not equal to priorityFn2.\n";
            return false;
        }
        if (shop.m_heapType != MINHEAP) {
            cout << "FAILED: heapType not equal to \"MINHEAP\".\n";
            return false;
        }
        if (shop.m_structure != SKEW) {
            cout << "FAILED: structure not equal to \"SKEW\".\n";
            return false;
        }
        if (shop.m_regPrior != 1) {
            cout << "FAILED: regPrior not equal to 1.\n";
            return false;
        }
        if (shop.m_size != 2) {
            cout << "FAILED: regPrior not equal to 2.\n";
            return false;
        }

        return true;
    }
    
    // Test Insert Order Func
    bool testInsertOrderMinHeap() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        
        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 300;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }
        
        // Check Size
        if (shop.m_size != NUM_INSERTS) {
            cout << "FAILED: Size is not the expect amount of \"NUM_INSERTS\"\n";
            return false;
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop.m_heap, shop.getPriorityFn(), shop.m_heapType, shop.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testInsertOrderMaxHeap() {
        // Data Generation
        Shop shop(priorityFn2, MAXHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 300;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        // Check Size
        if (shop.m_size != NUM_INSERTS) {
            cout << "FAILED: Size is not the expect amount of \"NUM_INSERTS\"\n";
            return false;
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop.m_heap, shop.getPriorityFn(), shop.m_heapType, shop.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testInsertOrderInvalidPriority() {
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        // Too Small
        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID -1 , MINORDERID);
        // Too Big
        Order o2(COFFEE, PAIR, TIER2, MINPOINTS, MAXCUSTID, MAXORDERID + 1);
        // Both
        Order o3(COFFEE, DOZEN, TIER3, MINPOINTS, MINCUSTID - 1, MINORDERID - 1);

        bool result1 = shop.insertOrder(o1);
        bool result2 = shop.insertOrder(o2);
        bool result3 = shop.insertOrder(o3);

        // Validate
        if (result1 == true || result2 == true || result3 == true || shop.m_size != 0) {
            cout << "FAILED: Atleast One of the orders was inserted.\n";
            return false;
        }

        return true;
    }

    // Test Get Next Order Func
    bool testGetNextOrderNormal() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID + 1, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 10;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID, MINORDERID);
        shop.insertOrder(o1);

        Order check = shop.getNextOrder();

        int expectedPriority = priorityFn2(o1);
        int returnedPriority = priorityFn2(check);
        // Check the Returned order Priority
        if (returnedPriority != expectedPriority) {
            cout << "FAILED: Returned Order is not the expected one.\n";
            return false;
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop.m_heap, shop.getPriorityFn(), shop.m_heapType, shop.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testGetNextOrderEmpty() {
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        bool result = false;
        try {
            shop.getNextOrder();
        }
        catch (out_of_range) {
            result = true;
        }

        if (result == false) {
            cout << "FAILED: Supposedly Returned an Order despite empty Heap.\n";
            return false;
        }
        return true;
    }
    bool testGetNextOrderMinHeap() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 300;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        // Track Removed Priority
        int prevPriority = -1;
        while (shop.numOrders() > 0) {
            Order current = shop.getNextOrder();
            int currPriority = priorityFn2(current);
            
            if (prevPriority != -1 && currPriority < prevPriority) {
                cout << "FAILED: Previous Priority was Bigger.\n";
                return false;
            }

            prevPriority = currPriority;
        }

        return true;
    }
    bool testGetNextOrderMaxHeap() {
        // Data Generation
        Shop shop(priorityFn2, MAXHEAP, SKEW, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 300;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        // Track Removed Priority
        int prevPriority = -1;
        while (shop.numOrders() > 0) {
            Order current = shop.getNextOrder();
            int currPriority = priorityFn2(current);

            if (prevPriority != -1 && currPriority > prevPriority) {
                cout << "FAILED: Previous Priority was Smaller.\n";
                return false;
            }

            prevPriority = currPriority;
        }

        return true;
    }

    // Test Merge with Queue Func
    bool testMergeNormal() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID + 1);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 10;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }
        const int NUM_INSERTS2 = 15;
        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS2; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop2.insertOrder(o);
        }

        shop.mergeWithQueue(shop2);

        if (shop.m_size != NUM_INSERTS + NUM_INSERTS2) {
            cout << "FAILED: Heap is not the expected size.\n";
            return false;
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop.m_heap, shop.getPriorityFn(), shop.m_heapType, shop.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testMergeDiffPriority() {
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn1, MINHEAP, LEFTIST, 2, SHOPMINID);

        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID, MINORDERID);
        Order o2(COFFEE, PAIR, TIER2, MINPOINTS, MAXCUSTID, MAXORDERID);

        shop.insertOrder(o1);
        shop2.insertOrder(o2);
        bool result = false;
        try {
            shop.mergeWithQueue(shop2);
        }
        catch (domain_error) {
            result = true;
        }
        
        if (result == false) {
            cout << "FAILED: Invalid Shops were merged.\n";
            return false;
        }
        if (shop.m_size != 1 || shop2.m_size != 1) {
            cout << "FAILED: Size of Heap Changed.\n";
            return false;
        }

        return true;
    }
    bool testMergeEmptyWithNormal() {
        // Data Generation
        Shop shop(priorityFn2, MAXHEAP, SKEW, 1, SHOPMINID);
        Shop shop2(priorityFn2, MAXHEAP, SKEW, 2, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 300;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        shop2.mergeWithQueue(shop);

        if (shop2.m_size != NUM_INSERTS) {
            cout << "FAILED: Heap is not the expected Size.\n";
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop2.m_heap, shop2.getPriorityFn(), shop2.m_heapType, shop2.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }

    // Test Clear Func
    bool testClearNormal() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 10;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        shop.clear();

        if (shop.m_size != 0) {
            cout << "FAILED: Size is not 0 after clearing.\n";
            return false;
        }
        if (shop.m_heap != nullptr) {
            cout << "FAILED: Heap is not pointing to nullptr.\n";
            return false;
        }
        if (shop.m_priorFunc != nullptr || shop.m_heapType != NOTYPE || shop.m_structure != NOSTRUCT ||
            shop.m_regPrior != 0 || shop.m_shopID != 0) {
            cout << "FAILED: Default Values were not restored.\n";
            return false;
        }

        return true;
    }

    // Test Num Orders Func
    bool testNumOrders() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 5;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        if (shop.numOrders() != 5) {
            cout << "FAILED: Number of Orders is not the expected Amount.\n";
            return false;
        }

        return true;
    }

    // Test Getter Funcs
    bool testGetPriority() {
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        if (shop.getPriorityFn() != priorityFn2) {
            cout << "FAILED: Returned Priority Function is not the Expected Function.\n";
            return false;
        }
        return true;
    }
    bool testGetHeapType() {
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        if (shop.getHeapType() != MINHEAP) {
            cout << "FAILED: Returned Heap Type is not the Expected Type.\n";
            return false;
        }
        return true;
    }
    bool testGetStructureType() {
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        if (shop.getStructure() != LEFTIST) {
            cout << "FAILED: Returned Structure Type is no the Expected Type.\n";
            return false;
        }
        return true;
    }

    // Test Setter Func
    bool testSetPriority() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 10;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        shop.setPriorityFn(priorityFn1, MAXHEAP);

        if (shop.m_priorFunc != priorityFn1) {
            cout << "FAILED: Not the Expected Priority Function.\n";
            return false;
        }
        if (shop.m_heapType != MAXHEAP) {
            cout << "FAILED: Not the Expected Heap Type.\n";
            return false;
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop.m_heap, shop.getPriorityFn(), shop.m_heapType, shop.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testSetStructure() {
        // Data Generation
        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);

        Random orderIDGen(MINORDERID, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 10;

        // Insert NUM_INSERTS Amount of Orders
        for (int i = 0; i < NUM_INSERTS; i++) {
            Order o(static_cast<ITEM>(itemGen.getRandNum()),
                static_cast<COUNT>(countGen.getRandNum()),
                static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                pointsGen.getRandNum(),
                customerIDGen.getRandNum(),
                orderIDGen.getRandNum()
            );

            shop.insertOrder(o);
        }

        shop.setStructure(SKEW);

        if (shop.m_structure != SKEW) {
            cout << "FAILED: Not the Expected Structure Type.\n";
            return false;
        }

        // Check Heap Property
        bool result = checkHeapProperty(shop.m_heap, shop.getPriorityFn(), shop.m_heapType, shop.m_structure);
        if (result == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }

    // REGION CLASS TEST FUNCTIONS
    // Test Constructor
    bool testConstrcutorRegion() {
        Region shopsQueue(5);

        if (shopsQueue.m_capacity != 5) {
            cout << "FAILED: Capacity is not the expected value.\n";
            return false;
        }
        if (shopsQueue.m_size != 0) {
            cout << "FAILED: Size is not the expected value.\n";
            return false;
        }

        return true;
    }

    // Test Add Shop Func
    bool testAddShopNormal() {
        Region shopsQueue(20);

        Random shopGen(10, 30);
        Random rndShopID(SHOPMINID, SHOPMAXID);

        for (int i = 0; i < 10; i++) {
            int rndShop = shopGen.getRandNum();
            int shopID = rndShopID.getRandNum();
            // create a Shop object
            Shop aShop(priorityFn2, MINHEAP, SKEW, rndShop, shopID);

            bool result = shopsQueue.addShop(aShop);
            if (result == false) {
                cout << "FAILED: Shop was not added.";
                return false;
            }
        }

        return true;
    }

    // Test Get Shop Func
    bool testGetShop() {
        Region shopsQueue(20);

        Random shopGen(11, 30);
        Random rndShopID(SHOPMINID, SHOPMAXID);

        for (int i = 0; i < 10; i++) {
            int rndShop = shopGen.getRandNum();
            int shopID = rndShopID.getRandNum();
            // create a Shop object
            Shop aShop(priorityFn2, MINHEAP, SKEW, rndShop, shopID);

            shopsQueue.addShop(aShop);
        }

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        bool result2 = shopsQueue.addShop(shop);

        Shop result;
        shopsQueue.getShop(result);
        if (result.m_regPrior != 1) {
            cout << "FAILED: Return Shop is not the Expect Shop.\n";
            return false;
        }

        if (result2 == false) {
            cout << "FAILED: Function Did Not Return a Shop.\n";
            return false;
        }

        return true;
    }
    bool testGetShopEmpty() {
        Region shopsQueue(1);

        Shop result;
        bool result2 = shopsQueue.getShop(result);

        if (result.m_regPrior != 0) {
            cout << "FAILED: Test Shop was Changed.\n";
            return false;
        }
        if (result2 != false) {
            cout << "FAILED: Function Did not Return False.\n";
            return false;
        }

        return true;
    }

    // Test Get Nth Shop Func
    bool testGetNthShop() {
        Region shopsQueue(20);

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID);
        Shop shop3(priorityFn2, MINHEAP, LEFTIST, 3, SHOPMINID);
        Shop shop4(priorityFn2, MINHEAP, LEFTIST, 4, SHOPMINID);
        shopsQueue.addShop(shop);
        shopsQueue.addShop(shop2);
        shopsQueue.addShop(shop3);
        shopsQueue.addShop(shop4);

        Shop result;
        bool result2 = shopsQueue.getNthShop(result, 3);

        if (result2 != true) {
            cout << "FAILED: Function returned false.\n";
            return false;
        }
        if (result.m_regPrior != 3) {
            cout << "FAILED: Function returned Unexpected Shop.\n";
            return false;
        }

        return true;
    }
    bool testGetNthShopEmpty() {
        Region shopsQueue(1);

        Shop result;
        bool result2 = shopsQueue.getNthShop(result, 10);

        if (result.m_regPrior != 0) {
            cout << "FAILED: Test Shop was Changed.\n";
            return false;
        }
        if (result2 != false) {
            cout << "FAILED: Function Did not Return False.\n";
            return false;
        }

        return true;
    }

    // Test Get Order Func
    bool testGetOrder() {
        Region shopsQueue(20);

        Random shopGen(11, 30);
        Random rndShopID(SHOPMINID, SHOPMAXID);
        Random orderIDGen(MINORDERID + 1, MAXORDERID);
        Random customerIDGen(MINCUSTID, MAXCUSTID);
        Random pointsGen(MINPOINTS, MAXPOINTS);
        Random countGen(ONE, DOZEN);
        Random membershipGen(0, 5);
        Random itemGen(0, 5);

        const int NUM_INSERTS = 10;

        for (int i = 0; i < NUM_INSERTS; i++) {
            int rndShop = shopGen.getRandNum();
            int shopID = rndShopID.getRandNum();
            // create a Shop object
            Shop aShop(priorityFn2, MINHEAP, SKEW, rndShop, shopID);
            // Insert NUM_INSERTS Amount of Orders
            for (int i = 0; i < NUM_INSERTS; i++) {
                Order o(static_cast<ITEM>(itemGen.getRandNum()),
                    static_cast<COUNT>(countGen.getRandNum()),
                    static_cast<MEMBERSHIP>(membershipGen.getRandNum()),
                    pointsGen.getRandNum(),
                    customerIDGen.getRandNum(),
                    orderIDGen.getRandNum()
                );
                aShop.insertOrder(o);
            }

            shopsQueue.addShop(aShop);
        }

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID);
        // Tests Removing this Shop that Won't Have any Orders
        // and Grabbing the Next Highest Prio Shop
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID + 1);

        Order o1(COFFEE, ONE, TIER1, MINPOINTS, MINCUSTID, MINORDERID);
        Order o2(COFFEE, ONE, TIER6, MINPOINTS, MINCUSTID, MINORDERID + 1);
        shop.insertOrder(o1);
        shop.insertOrder(o2);
        shopsQueue.addShop(shop);
        shopsQueue.addShop(shop2);

        // Validation
        Order result;
        bool result2 = shopsQueue.getOrder(result);

        if (result2 != true) {
            cout << "FAILED: Get Order Didn't Return a Shop.\n";
            return false;
        }
        if (result.m_orderID != MINORDERID) {
            cout << "FAILED: Returned Order is not the expected Order.\n";
            return false;
        }
        if (shopsQueue.m_size != NUM_INSERTS + 1) {
            cout << "FAILED: Lost an Unexpected amount of Shops is the process of getting Order.\n";
            return false;
        }
        
        return true;
    }

    // Test Setters
    bool testSetPriorityRegion() {
        Region shopsQueue(20);

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID);
        Shop shop3(priorityFn2, MINHEAP, LEFTIST, 3, SHOPMINID);
        Shop shop4(priorityFn2, MINHEAP, LEFTIST, 4, SHOPMINID);
        shopsQueue.addShop(shop);
        shopsQueue.addShop(shop2);
        shopsQueue.addShop(shop3);
        shopsQueue.addShop(shop4);

        bool result = shopsQueue.setPriorityFn(priorityFn1, MAXHEAP, 2);

        Shop result2;
        shopsQueue.getNthShop(result2, 2);

        if (result != true) {
            cout << "FAILED: Set Priority Function Did not Return True.\n";
            return false;
        }
        if (result2.m_heapType != MAXHEAP || result2.m_priorFunc != priorityFn1) {
            cout << "FAILED: Shop does not contain the Expected Values for Heap Type or Priority Function.\n";
            return false;
        }
        // Verify Heap Property
        bool result3 = checkHeapProperty(result2.m_heap, result2.getPriorityFn(), result2.m_heapType, result2.m_structure);
        if (result3 == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testSetPriorityInvalidRegion() {
        Region shopsQueue(20);

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID);
        Shop shop3(priorityFn2, MINHEAP, LEFTIST, 3, SHOPMINID);
        Shop shop4(priorityFn2, MINHEAP, LEFTIST, 4, SHOPMINID);
        shopsQueue.addShop(shop);
        shopsQueue.addShop(shop2);
        shopsQueue.addShop(shop3);
        shopsQueue.addShop(shop4);

        bool result = shopsQueue.setPriorityFn(priorityFn1, MAXHEAP, 5);

        if (result == true) {
            cout << "FAILED: Set Priority Function Returned True.\n";
            return false;
        }
        return true;
    }
    bool testSetStructureRegion() {
        Region shopsQueue(20);

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID);
        Shop shop3(priorityFn2, MINHEAP, LEFTIST, 3, SHOPMINID);
        Shop shop4(priorityFn2, MINHEAP, LEFTIST, 4, SHOPMINID);
        shopsQueue.addShop(shop);
        shopsQueue.addShop(shop2);
        shopsQueue.addShop(shop3);
        shopsQueue.addShop(shop4);

        bool result = shopsQueue.setStructure(SKEW, 2);

        Shop result2;
        shopsQueue.getNthShop(result2, 2);

        if (result != true) {
            cout << "FAILED: Set Priority Function Did not Return True.\n";
            return false;
        }
        if (result2.m_structure != SKEW) {
            cout << "FAILED: Shop does not contain the Expected Values for Structure Type.\n";
            return false;
        }
        // Verify Heap Property
        bool result3 = checkHeapProperty(result2.m_heap, result2.getPriorityFn(), result2.m_heapType, result2.m_structure);
        if (result3 == false) {
            cout << "FAILED: Heap Property Check.\n";
            return false;
        }

        return true;
    }
    bool testSetStructureInvalidRegion() {
        Region shopsQueue(20);

        Shop shop(priorityFn2, MINHEAP, LEFTIST, 1, SHOPMINID);
        Shop shop2(priorityFn2, MINHEAP, LEFTIST, 2, SHOPMINID);
        Shop shop3(priorityFn2, MINHEAP, LEFTIST, 3, SHOPMINID);
        Shop shop4(priorityFn2, MINHEAP, LEFTIST, 4, SHOPMINID);
        shopsQueue.addShop(shop);
        shopsQueue.addShop(shop2);
        shopsQueue.addShop(shop3);
        shopsQueue.addShop(shop4);

        bool result = shopsQueue.setStructure(SKEW, 5);

        if (result == true) {
            cout << "FAILED: Set Priority Function Returned True.\n";
            return false;
        }
        return true;
    }

private:
};

int main() {
    Tester tester;
    // Test Shop Class
    // Test Default Constructor
    if (tester.testDefaultConstructorShop() == true) cout << "PASSED: Test of SHOP Default Constructor.\n";
    // Test Copy Constructor
    if (tester.testCopyConstructorDefaultShop() == true) cout << "PASSED: Test of SHOP Copy Constructor Default.\n";
    if (tester.testCopyConstructorNormalShop() == true) cout << "PASSED: Test of SHOP Copy Constructor Normal.\n";
    // Test Assignment Operator
    if (tester.testAssignmentNormalShop() == true) cout << "PASSED: Test of SHOP Assignment Normal.\n";
    if (tester.testAssignmentSelfShop() == true) cout << "PASSED: Test of SHOP Assignment Self.\n";
    // Test Insert Order Func
    if (tester.testInsertOrderMinHeap() == true) cout << "PASSED: Test of SHOP Insert Order MIN heap.\n";
    if (tester.testInsertOrderMaxHeap() == true) cout << "PASSED: Test of SHOP Insert Order MAX heap.\n";
    if (tester.testInsertOrderInvalidPriority() == true) cout << "PASSED: Test of SHOP Insert Order INVALID.\n";
    // Test Get Next Order Func
    if (tester.testGetNextOrderNormal() == true) cout << "PASSED: Test of SHOP Get Next Order Normal.\n";
    if (tester.testGetNextOrderEmpty() == true) cout << "PASSED: Test of SHOP Get Next Order Empty.\n";
    if (tester.testGetNextOrderMinHeap() == true) cout << "PASSED: Test of SHOP Get Next Order MIN Heap.\n";
    if (tester.testGetNextOrderMaxHeap() == true) cout << "PASSED: Test of SHOP Get Next Order MAX Heap.\n";
    // Test Merge Func
    if (tester.testMergeNormal() == true) cout << "PASSED: Test of SHOP Merge Normal.\n";
    if (tester.testMergeDiffPriority() == true) cout << "PASSED: Test of SHOP Merge Diff Priority.\n";
    if (tester.testMergeEmptyWithNormal() == true) cout << "PASSED: Test of SHOP Merge Empty With Normal.\n";
    // Test Clear Func
    if (tester.testClearNormal() == true) cout << "PASSED: Test of SHOP Clear Normal.\n";
    // Test NumOrder Func
    if (tester.testNumOrders() == true) cout << "PASSED: Test of SHOP NumOrders.\n";
    // Test Getter Funcs
    if (tester.testGetPriority() == true) cout << "PASSED: Test of SHOP Get Priority.\n";
    if (tester.testGetHeapType() == true) cout << "PASSED: Test of SHOP Get Heap Type.\n";
    if (tester.testGetStructureType() == true) cout << "PASSED: Test of SHOP Get Structure Type.\n";
    // Test Setter Funcs
    if (tester.testSetPriority() == true) cout << "PASSED: Test of SHOP Set Priority.\n";
    if (tester.testSetStructure() == true) cout << "PASSED: Test of SHOP Set Structure.\n";

    // Test Region Class
    // Test Constructor
    if (tester.testConstrcutorRegion() == true) cout << "PASSED: Test of REGION Constructor.\n";
    // Test Add Shop Func
    if (tester.testAddShopNormal() == true) cout << "PASSED: Test of REGION Add Shop.\n";
    // Test Get Shop Func
    if (tester.testGetShop() == true) cout << "PASSED: Test of REGION Get Shop.\n";
    if (tester.testGetShopEmpty() == true) cout << "PASSED: Test of REGION Get Shop Empty.\n";
    // Test Get Nth Shop Func
    if (tester.testGetNthShop() == true) cout << "PASSED: Test of REGION Get Nth Shop.\n";
    if (tester.testGetNthShopEmpty() == true) cout << "PASSED: Test of REGION Get Nth Shop Empty.\n";
    // Test Get Order Func
    if (tester.testGetOrder() == true) cout << "PASSED: Test of REGION Get Order.\n";
    // Test Setter Funcs
    if (tester.testSetPriorityRegion() == true) cout << "PASSED: Test of REGION Set Priority.\n";
    if (tester.testSetPriorityInvalidRegion() == true) cout << "PASSED: Test of REGION Set Priority Invalid.\n";
    if (tester.testSetStructureRegion() == true) cout << "PASSED: Test of REGION Set Structure.\n";
    if (tester.testSetStructureInvalidRegion() == true) cout << "PASSED: Test of REGION Set Structure Invalid.\n";

    return 0;
}

int priorityFn1(const Order& order) {
    //this function works with a MAXHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [0-5003]
    //the highest priority would be 3+5000 = 5003
    //the lowest priority would be 0+0 = 0 => 1 
    //the larger value means the higher priority
    int priority = static_cast<int>(order.getCount()) + order.getPoints();
    if (priority == 0) priority = 1;
    return priority;
}

int priorityFn2(const Order& order) {
    //this function works with a MINHEAP
    //priority value is determined based on some criteria
    //priority value falls in the range [0-10]
    //the highest priority would be 0+0 = 0 => 1
    //the lowest priority would be 5+5 =10
    //the smaller value means the higher priority
    int priority = static_cast<int>(order.getItem()) + static_cast<int>(order.getMemebership());
    if (priority == 0) priority = 1;
    return priority;
}
