#pragma once

#include <vector>

// Pool is a vector of object of type T.
// use vector for memory contiguity
// vector의 wrapper기 때문에 가급적 vector의 기본 메서드를 변화시키지 않는 것이 좋다.
template <typename T>
class Pool {
private:
    std::vector<T> data;

public:
    Pool(int size = 100) {
        Resize(size);
    };
    virtual ~Pool() = default;

    T& operator[](unsigned int index) {
        return data[index];
    }

    bool isEmpty() const {
        return data.empty();
    }
    int GetSize() const {
        return data.size();
    }
    void Resize(int n) {
        data.resize(n);
    }
    void Clear() {
        data.clear();
    }
    void Add(const T object) {
        data.push_back(object);
    }
    void Set(int index, const T object) {
        data[index] = object;
    }
    T& Get(int index) {
        return static_cast<T&>(data[index]);
    }
};
