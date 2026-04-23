#include "map.hpp"
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
    
    for (int i = 0; i < 100; ++i) {
        std::string string = "";
        for (int number = i; number; number /= 10) {
            char digit = '0' + number % 10;
            string = digit + string;
        }
        map[Integer(i)] = string;
    }
    
    std::cout << "Size: " << map.size() << std::endl;
    
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << it->first.val << " -> " << it->second << std::endl;
    }
    
    return 0;
}
