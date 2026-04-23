#include "src/map.hpp"
#include <iostream>
#include <cassert>

class Integer {
public:
    static int counter;
    int val;
    
    Integer(int val) : val(val) {
        counter++;
    }

    Integer(const Integer &rhs) {
        val = rhs.val;
        counter++;
    }

    Integer& operator = (const Integer &rhs) {
        assert(false);
    }

    ~Integer() {
        counter--;
    }
};

int Integer::counter = 0;

class Compare {
public:
    bool operator () (const Integer &lhs, const Integer &rhs) const {
        return lhs.val < rhs.val;
    }
};

int main() {
    sjtu::map<Integer, std::string, Compare> map;
    std::cout << "Creating map..." << std::endl;
    
    std::cout << "Inserting 0..." << std::endl;
    map[Integer(0)] = "0";
    
    std::cout << "Inserting 1..." << std::endl;
    map[Integer(1)] = "1";
    
    std::cout << "Size: " << map.size() << std::endl;
    
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << it->first.val << " -> " << it->second << std::endl;
    }
    
    return 0;
}
