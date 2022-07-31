#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class NodeAlloc
{
    private:
    std::vector<int> allocVec_;

    public:
    void init(const char* s)
    {
        std::ifstream nodeAllocFile(s);
        int sz;
        nodeAllocFile >> sz;
        allocVec_.reserve(sz);
        int temp;
        while (nodeAllocFile >> temp)
        {
            allocVec_.push_back(temp);
        }
        nodeAllocFile.close();
    }

    int operator[](unsigned x) const
    {
        return allocVec_[x];
    }

    unsigned size() const
    {
        return allocVec_.size();
    }
};