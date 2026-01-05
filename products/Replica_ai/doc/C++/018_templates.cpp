/*
 * 知识点：模板
 * 源项目：e:\3.esp-brookesia\esp-brookesia\core\brookesia_core\systems\base\esp_brookesia_base_app.hpp 第85行
 * C++ Primer中文版（第五版）：第16章 模板与泛型编程 - 16.1 定义模板
 * Effective+C++中文版（第三版）：条款41 了解隐式接口和编译期多态
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

// 1. 函数模板
 template <typename T>
T maxValue(T a, T b) {
    return (a > b) ? a : b;
}

// 2. 类模板
 template <typename T>
class Vector {
public:
    Vector() : _size(0), _capacity(2), _data(new T[2]) {
        std::cout << "Vector default constructor" << std::endl;
    }
    
    Vector(size_t capacity) : _size(0), _capacity(capacity), _data(new T[capacity]) {
        std::cout << "Vector constructor with capacity: " << capacity << std::endl;
    }
    
    ~Vector() {
        std::cout << "Vector destructor, size: " << _size << ", capacity: " << _capacity << std::endl;
        delete[] _data;
    }
    
    // 成员函数模板
    void push_back(const T& value) {
        if (_size >= _capacity) {
            resize(_capacity * 2);
        }
        _data[_size++] = value;
    }
    
    T& operator[](size_t index) {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }
    
    const T& operator[](size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }
    
    size_t size() const {
        return _size;
    }
    
    size_t capacity() const {
        return _capacity;
    }
    
    // 模板特化示例：针对const char*类型的特化
    void push_back(const char* value) {
        push_back(std::string(value));
    }
    
private:
    void resize(size_t new_capacity) {
        std::cout << "Vector resizing from " << _capacity << " to " << new_capacity << std::endl;
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < _size; i++) {
            new_data[i] = _data[i];
        }
        delete[] _data;
        _data = new_data;
        _capacity = new_capacity;
    }
    
    T* _data;
    size_t _size;
    size_t _capacity;
};

// 3. 模板特化：针对特定类型的特殊实现
 template <>
class Vector<bool> {
public:
    Vector() : _size(0), _capacity(8), _data(new uint8_t[1]) {
        *_data = 0;
        std::cout << "Vector<bool> default constructor" << std::endl;
    }
    
    ~Vector() {
        std::cout << "Vector<bool> destructor, size: " << _size << std::endl;
        delete[] _data;
    }
    
    void push_back(bool value) {
        if (_size >= _capacity) {
            resize(_capacity * 2);
        }
        
        size_t byte_index = _size / 8;
        size_t bit_index = _size % 8;
        
        if (value) {
            _data[byte_index] |= (1 << bit_index);
        } else {
            _data[byte_index] &= ~(1 << bit_index);
        }
        
        _size++;
    }
    
    bool operator[](size_t index) const {
        if (index >= _size) {
            throw std::out_of_range("Index out of range");
        }
        
        size_t byte_index = index / 8;
        size_t bit_index = index % 8;
        
        return (_data[byte_index] & (1 << bit_index)) != 0;
    }
    
    size_t size() const {
        return _size;
    }
    
private:
    void resize(size_t new_capacity) {
        std::cout << "Vector<bool> resizing from " << _capacity << " to " << new_capacity << std::endl;
        
        size_t new_byte_count = (new_capacity + 7) / 8;
        uint8_t* new_data = new uint8_t[new_byte_count]();
        
        size_t old_byte_count = (_capacity + 7) / 8;
        for (size_t i = 0; i < old_byte_count; i++) {
            new_data[i] = _data[i];
        }
        
        delete[] _data;
        _data = new_data;
        _capacity = new_capacity;
    }
    
    uint8_t* _data;
    size_t _size;
    size_t _capacity;
};

// 4. 模板参数默认值
 template <typename T = int, size_t N = 10>
class Array {
public:
    Array() {
        std::cout << "Array constructor, size: " << N << std::endl;
    }
    
    T& operator[](size_t index) {
        if (index >= N) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }
    
    const T& operator[](size_t index) const {
        if (index >= N) {
            throw std::out_of_range("Index out of range");
        }
        return _data[index];
    }
    
    size_t size() const {
        return N;
    }
    
private:
    T _data[N];
};

// 5. 可变参数模板
 template <typename... Args>
void printAll(Args... args) {
    (std::cout << ... << args) << std::endl;
}

// 6. 模板与继承
 template <typename T>
class Base {
public:
    Base() : _value(T()) {
        std::cout << "Base constructor" << std::endl;
    }
    
    virtual ~Base() {
        std::cout << "Base destructor" << std::endl;
    }
    
    virtual void display() const {
        std::cout << "Base display: " << _value << std::endl;
    }
    
protected:
    T _value;
};

 template <typename T>
class Derived : public Base<T> {
public:
    Derived(T value) {
        std::cout << "Derived constructor" << std::endl;
        this->_value = value;
    }
    
    ~Derived() {
        std::cout << "Derived destructor" << std::endl;
    }
    
    void display() const override {
        std::cout << "Derived display: " << this->_value << std::endl;
    }
};

// 7. 模板别名（typedef的现代替代）
template <typename T>
using Vec = std::vector<T>;

// 8. 模板元编程：编译期计算
 template <int N>
struct Factorial {
    static constexpr int value = N * Factorial<N - 1>::value;
};

 template <>
struct Factorial<0> {
    static constexpr int value = 1;
};

// 编译期计算结果
constexpr int FACTORIAL_5 = Factorial<5>::value;
constexpr int FACTORIAL_10 = Factorial<10>::value;

int main() {
    std::cout << "=== Templates Examples ===" << std::endl;
    
    std::cout << "\n1. Function Templates:" << std::endl;
    // 函数模板的自动类型推导
    int max_int = maxValue(10, 20);
    double max_double = maxValue(3.14, 2.71);
    std::string max_string = maxValue(std::string("apple"), std::string("banana"));
    
    std::cout << "Max int: " << max_int << std::endl;
    std::cout << "Max double: " << max_double << std::endl;
    std::cout << "Max string: " << max_string << std::endl;
    
    std::cout << "\n2. Class Templates:" << std::endl;
    // 使用类模板
    Vector<int> intVec;
    intVec.push_back(10);
    intVec.push_back(20);
    intVec.push_back(30);
    
    std::cout << "intVec size: " << intVec.size() << ", capacity: " << intVec.capacity() << std::endl;
    std::cout << "intVec[0]: " << intVec[0] << ", intVec[1]: " << intVec[1] << std::endl;
    
    // 使用字符串类型
    Vector<std::string> stringVec(5);
    stringVec.push_back("Calculator");
    stringVec.push_back("Game 2048");
    stringVec.push_back("Settings");
    stringVec.push_back("Browser");
    stringVec.push_back("Music Player");
    stringVec.push_back("Video Player");  // 触发扩容
    
    std::cout << "stringVec size: " << stringVec.size() << ", capacity: " << stringVec.capacity() << std::endl;
    for (size_t i = 0; i < stringVec.size(); i++) {
        std::cout << "  stringVec[" << i << "]: " << stringVec[i] << std::endl;
    }
    
    // 测试const char*特化
    stringVec.push_back("New App");  // 调用特化版本
    std::cout << "After push_back(const char*): stringVec.size() = " << stringVec.size() << std::endl;
    
    std::cout << "\n3. Template Specialization:" << std::endl;
    // 使用特化的Vector<bool>
    Vector<bool> boolVec;
    boolVec.push_back(true);
    boolVec.push_back(false);
    boolVec.push_back(true);
    boolVec.push_back(true);
    
    std::cout << "boolVec size: " << boolVec.size() << std::endl;
    for (size_t i = 0; i < boolVec.size(); i++) {
        std::cout << "  boolVec[" << i << "]: " << (boolVec[i] ? "true" : "false") << std::endl;
    }
    
    std::cout << "\n4. Array Template with Default Parameters:" << std::endl;
    // 使用带默认参数的模板
    Array<> intArray;  // 使用默认类型int和默认大小10
    intArray[0] = 100;
    intArray[1] = 200;
    std::cout << "intArray[0]: " << intArray[0] << ", intArray[1]: " << intArray[1] << std::endl;
    std::cout << "intArray size: " << intArray.size() << std::endl;
    
    // 自定义类型和大小
    Array<std::string, 5> stringArray;
    stringArray[0] = "Hello";
    stringArray[1] = "World";
    std::cout << "stringArray[0]: " << stringArray[0] << ", stringArray[1]: " << stringArray[1] << std::endl;
    
    std::cout << "\n5. Variadic Templates:" << std::endl;
    // 使用可变参数模板
    printAll("Hello", ", ", "world", "!", " Value:", 42);
    
    std::cout << "\n6. Templates with Inheritance:" << std::endl;
    // 使用模板继承
    Base<int> baseInt;
    baseInt.display();
    
    Derived<double> derivedDouble(3.14);
    derivedDouble.display();
    
    // 多态
    Base<double>* basePtr = new Derived<double>(2.71);
    basePtr->display();
    delete basePtr;
    
    std::cout << "\n7. Template Aliases:" << std::endl;
    // 使用模板别名
    Vec<int> vec1;
    vec1.push_back(1);
    vec1.push_back(2);
    vec1.push_back(3);
    
    std::cout << "vec1 size: " << vec1.size() << std::endl;
    for (int val : vec1) {
        std::cout << "  " << val << std::endl;
    }
    
    std::cout << "\n8. Template Metaprogramming:" << std::endl;
    // 编译期计算结果
    std::cout << "Factorial of 5: " << FACTORIAL_5 << std::endl;
    std::cout << "Factorial of 10: " << FACTORIAL_10 << std::endl;
    
    return 0;
}