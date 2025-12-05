// ============================================================================//
// ESP-Brookesia 项目 C++ 学习文件 04_best_practices.cpp
// 最佳实践：RAII、虚析构函数、接口设计、资源管理、类设计
// ============================================================================//

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

// =========================================
// 1. RAII 原则（Resource Acquisition Is Initialization）
// =========================================
// RAII是C++最核心的资源管理原则
// 学习要点：资源在构造时获取，在析构时释放，确保资源正确管理

// 示例1：基本RAII类
class FileHandler {
private:
    FILE* _file;
    string _filename;
    
public:
    // 构造时获取资源
    FileHandler(const string& filename, const string& mode) 
        : _filename(filename) {
        _file = fopen(filename.c_str(), mode.c_str());
        if (_file == nullptr) {
            throw runtime_error("Failed to open file: " + filename);
        }
        cout << "File opened: " << filename << endl;
    }
    
    // 析构时释放资源
    ~FileHandler() {
        if (_file != nullptr) {
            fclose(_file);
            cout << "File closed: " << _filename << endl;
        }
    }
    
    // 禁止拷贝构造和赋值（避免资源重复释放）
    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    
    // 允许移动构造和赋值
    FileHandler(FileHandler&& other) noexcept 
        : _file(other._file), _filename(move(other._filename)) {
        other._file = nullptr; // 避免原对象析构时释放资源
        cout << "FileHandler moved: " << _filename << endl;
    }
    
    FileHandler& operator=(FileHandler&& other) noexcept {
        if (this != &other) {
            // 释放当前资源
            if (_file != nullptr) {
                fclose(_file);
            }
            
            // 转移资源所有权
            _file = other._file;
            _filename = move(other._filename);
            other._file = nullptr;
            
            cout << "FileHandler move assigned: " << _filename << endl;
        }
        return *this;
    }
    
    // 提供访问资源的方法
    FILE* get() const {
        return _file;
    }
    
    // 写入文件的方法
    size_t write(const string& content) {
        if (_file == nullptr) {
            throw runtime_error("File is not open");
        }
        return fwrite(content.c_str(), 1, content.size(), _file);
    }
};

// 示例2：RAII锁管理
class MutexLock {
private:
    mutex& _mutex;
    bool _locked;
    
public:
    // 构造时加锁
    explicit MutexLock(mutex& mtx) : _mutex(mtx), _locked(false) {
        _mutex.lock();
        _locked = true;
        cout << "Mutex locked" << endl;
    }
    
    // 析构时解锁
    ~MutexLock() {
        if (_locked) {
            _mutex.unlock();
            _locked = false;
            cout << "Mutex unlocked" << endl;
        }
    }
    
    // 禁止拷贝
    MutexLock(const MutexLock&) = delete;
    MutexLock& operator=(const MutexLock&) = delete;
    
    // 允许移动（可选，需谨慎实现）
    MutexLock(MutexLock&& other) noexcept 
        : _mutex(other._mutex), _locked(other._locked) {
        other._locked = false; // 原对象不再负责解锁
    }
    
    // 手动解锁（可选）
    void unlock() {
        if (_locked) {
            _mutex.unlock();
            _locked = false;
            cout << "Mutex manually unlocked" << endl;
        }
    }
    
    // 手动加锁（可选）
    void lock() {
        if (!_locked) {
            _mutex.lock();
            _locked = true;
            cout << "Mutex manually locked" << endl;
        }
    }
};

// 示例3：使用RAII管理动态内存
class DynamicArray {
private:
    int* _data;
    size_t _size;
    
public:
    DynamicArray(size_t size) : _size(size) {
        _data = new int[size](); // 初始化所有元素为0
        cout << "DynamicArray created, size: " << size << endl;
    }
    
    ~DynamicArray() {
        delete[] _data;
        cout << "DynamicArray destroyed" << endl;
    }
    
    // 禁止拷贝，允许移动
    DynamicArray(const DynamicArray&) = delete;
    DynamicArray& operator=(const DynamicArray&) = delete;
    
    DynamicArray(DynamicArray&& other) noexcept 
        : _data(other._data), _size(other._size) {
        other._data = nullptr;
        other._size = 0;
    }
    
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] _data;
            _data = other._data;
            _size = other._size;
            other._data = nullptr;
            other._size = 0;
        }
        return *this;
    }
    
    // 访问元素
    int& operator[](size_t index) {
        if (index >= _size) {
            throw out_of_range("Index out of range");
        }
        return _data[index];
    }
    
    size_t size() const {
        return _size;
    }
};

// =========================================
// 2. 虚析构函数的重要性
// =========================================
// 学习要点：多态基类必须声明虚析构函数，否则会导致内存泄漏

// 反例：没有虚析构函数的基类
class BadBase {
private:
    int* _data;
    
public:
    BadBase() {
        _data = new int[100];
        cout << "BadBase constructor" << endl;
    }
    
    // 非虚析构函数 - 错误！
    ~BadBase() {
        delete[] _data;
        cout << "BadBase destructor" << endl;
    }
    
    virtual void do_something() {
        cout << "BadBase::do_something" << endl;
    }
};

class BadDerived : public BadBase {
private:
    string* _name;
    
public:
    BadDerived() {
        _name = new string("BadDerived");
        cout << "BadDerived constructor" << endl;
    }
    
    ~BadDerived() {
        delete _name;
        cout << "BadDerived destructor" << endl;
    }
    
    void do_something() override {
        cout << "BadDerived::do_something" << endl;
    }
};

// 正例：有虚析构函数的基类
class GoodBase {
private:
    int* _data;
    
public:
    GoodBase() {
        _data = new int[100];
        cout << "GoodBase constructor" << endl;
    }
    
    // 虚析构函数 - 正确！
    virtual ~GoodBase() {
        delete[] _data;
        cout << "GoodBase destructor" << endl;
    }
    
    virtual void do_something() {
        cout << "GoodBase::do_something" << endl;
    }
};

class GoodDerived : public GoodBase {
private:
    string* _name;
    
public:
    GoodDerived() {
        _name = new string("GoodDerived");
        cout << "GoodDerived constructor" << endl;
    }
    
    ~GoodDerived() {
        delete _name;
        cout << "GoodDerived destructor" << endl;
    }
    
    void do_something() override {
        cout << "GoodDerived::do_something" << endl;
    }
};

// 演示虚析构函数的重要性
void demonstrate_virtual_destructor() {
    cout << "\n=== 演示虚析构函数的重要性 ===" << endl;
    
    cout << "\n1. 使用非虚析构函数的基类（会导致内存泄漏）:" << endl;
    BadBase* bad_ptr = new BadDerived();
    bad_ptr->do_something();
    delete bad_ptr; // 只调用BadBase析构函数，BadDerived的析构函数不会被调用，导致内存泄漏
    
    cout << "\n2. 使用虚析构函数的基类（正确释放）:" << endl;
    GoodBase* good_ptr = new GoodDerived();
    good_ptr->do_something();
    delete good_ptr; // 先调用GoodDerived析构函数，再调用GoodBase析构函数，正确释放所有资源
}

// =========================================
// 3. 接口设计最佳实践
// =========================================
// 学习要点：让接口容易被正确使用，不易被误用；使用RAII；避免不必要的拷贝

// 反例：糟糕的接口设计
class BadInterface {
private:
    vector<int>* _data;
    bool _owns_data;
    
public:
    // 构造函数：需要调用者管理内存所有权
    BadInterface(vector<int>* data, bool owns_data = false) 
        : _data(data), _owns_data(owns_data) {
        if (_data == nullptr) {
            _data = new vector<int>();
            _owns_data = true;
        }
    }
    
    ~BadInterface() {
        if (_owns_data) {
            delete _data;
        }
    }
    
    // 非const方法返回非const指针，允许外部修改内部状态
    vector<int>* get_data() {
        return _data;
    }
    
    // 需要调用者手动释放资源的方法
    void release_data() {
        _owns_data = false;
    }
};

// 正例：良好的接口设计
class GoodInterface {
private:
    unique_ptr<vector<int>> _data;
    
public:
    // 构造函数：RAII管理资源
    GoodInterface() : _data(make_unique<vector<int>>()) {
    }
    
    explicit GoodInterface(size_t initial_size) 
        : _data(make_unique<vector<int>>(initial_size, 0)) {
    }
    
    // 拷贝构造：深拷贝
    GoodInterface(const GoodInterface& other) 
        : _data(make_unique<vector<int>>(*other._data)) {
    }
    
    // 移动构造：高效转移所有权
    GoodInterface(GoodInterface&& other) noexcept = default;
    
    // 拷贝赋值：深拷贝
    GoodInterface& operator=(const GoodInterface& other) {
        if (this != &other) {
            *_data = *other._data;
        }
        return *this;
    }
    
    // 移动赋值：高效转移
    GoodInterface& operator=(GoodInterface&& other) noexcept = default;
    
    // const方法返回const引用，禁止外部修改内部状态
    const vector<int>& get_data() const {
        return *_data;
    }
    
    // 提供安全的修改方法
    void add_element(int value) {
        _data->push_back(value);
    }
    
    // 返回拷贝而非指针，避免内存管理问题
    vector<int> copy_data() const {
        return *_data;
    }
    
    // 清晰的状态查询方法
    bool is_empty() const {
        return _data->empty();
    }
    
    size_t size() const {
        return _data->size();
    }
};

// 示例：Builder模式（用于复杂对象构建）
class ComplexObject {
private:
    string _name;
    int _version;
    bool _debug_mode;
    size_t _max_connections;
    string _log_file;
    
    // 私有构造函数，只能通过Builder创建
    ComplexObject(const string& name, int version, bool debug_mode, 
                 size_t max_connections, const string& log_file) 
        : _name(name), _version(version), _debug_mode(debug_mode),
          _max_connections(max_connections), _log_file(log_file) {
    }
    
public:
    // Builder类
    class Builder {
    private:
        // 必须参数
        string _name;
        
        // 可选参数（有默认值）
        int _version = 1;
        bool _debug_mode = false;
        size_t _max_connections = 10;
        string _log_file = "app.log";
        
    public:
        // 构造Builder时必须提供name
        explicit Builder(string name) : _name(move(name)) {
        }
        
        // 链式调用的设置方法
        Builder& version(int v) {
            _version = v;
            return *this;
        }
        
        Builder& debug_mode(bool mode) {
            _debug_mode = mode;
            return *this;
        }
        
        Builder& max_connections(size_t connections) {
            _max_connections = connections;
            return *this;
        }
        
        Builder& log_file(string file) {
            _log_file = move(file);
            return *this;
        }
        
        // 构建对象
        ComplexObject build() {
            return ComplexObject(_name, _version, _debug_mode, 
                                _max_connections, _log_file);
        }
    };
    
    // 获取属性的方法
    const string& get_name() const { return _name; }
    int get_version() const { return _version; }
    bool is_debug_mode() const { return _debug_mode; }
    size_t get_max_connections() const { return _max_connections; }
    const string& get_log_file() const { return _log_file; }
    
    void print_info() const {
        cout << "ComplexObject Info:" << endl;
        cout << "  Name: " << _name << endl;
        cout << "  Version: " << _version << endl;
        cout << "  Debug Mode: " << (_debug_mode ? "Yes" : "No") << endl;
        cout << "  Max Connections: " << _max_connections << endl;
        cout << "  Log File: " << _log_file << endl;
    }
};

// =========================================
// 4. 构造/析构/赋值运算的最佳实践
// =========================================
// 学习要点：了解编译器自动生成的函数；正确实现五大函数

class WellDesignedClass {
private:
    string _name;
    unique_ptr<int[]> _data;
    size_t _size;
    mutex _mutex; // 不可拷贝的成员
    
public:
    // 1. 默认构造函数
    WellDesignedClass() : _name("default"), _size(0) {
        cout << "WellDesignedClass default constructor" << endl;
    }
    
    // 2. 带参数的构造函数
    explicit WellDesignedClass(const string& name, size_t size = 0) 
        : _name(name), _size(size) {
        if (size > 0) {
            _data = make_unique<int[]>(size);
            // 初始化数据
            for (size_t i = 0; i < size; ++i) {
                _data[i] = static_cast<int>(i);
            }
        }
        cout << "WellDesignedClass constructor: " << name << endl;
    }
    
    // 3. 拷贝构造函数（深拷贝）
    WellDesignedClass(const WellDesignedClass& other) 
        : _name(other._name), _size(other._size) {
        if (other._data != nullptr) {
            _data = make_unique<int[]>(_size);
            // 深拷贝数据
            for (size_t i = 0; i < _size; ++i) {
                _data[i] = other._data[i];
            }
        }
        cout << "WellDesignedClass copy constructor" << endl;
    }
    
    // 4. 移动构造函数（转移所有权）
    WellDesignedClass(WellDesignedClass&& other) noexcept 
        : _name(move(other._name)), 
          _data(move(other._data)), 
          _size(other._size) {
        other._size = 0; // 重置源对象状态
        cout << "WellDesignedClass move constructor" << endl;
    }
    
    // 5. 拷贝赋值运算符（深拷贝）
    WellDesignedClass& operator=(const WellDesignedClass& other) {
        if (this != &other) {
            // 拷贝名称
            _name = other._name;
            
            // 深拷贝数据
            _size = other._size;
            if (other._data != nullptr) {
                auto new_data = make_unique<int[]>(_size);
                for (size_t i = 0; i < _size; ++i) {
                    new_data[i] = other._data[i];
                }
                _data = move(new_data);
            } else {
                _data.reset();
            }
        }
        cout << "WellDesignedClass copy assignment" << endl;
        return *this;
    }
    
    // 6. 移动赋值运算符（转移所有权）
    WellDesignedClass& operator=(WellDesignedClass&& other) noexcept {
        if (this != &other) {
            // 转移所有权
            _name = move(other._name);
            _data = move(other._data);
            _size = other._size;
            
            // 重置源对象状态
            other._size = 0;
        }
        cout << "WellDesignedClass move assignment" << endl;
        return *this;
    }
    
    // 7. 析构函数
    ~WellDesignedClass() {
        cout << "WellDesignedClass destructor: " << _name << endl;
        // unique_ptr会自动释放_data，无需手动操作
    }
    
    // 禁用不必要的函数（如果需要）
    // WellDesignedClass(const WellDesignedClass&) = delete;
    // WellDesignedClass& operator=(const WellDesignedClass&) = delete;
    
    void print_info() const {
        cout << "WellDesignedClass: " << _name << endl;
        cout << "  Size: " << _size << endl;
        if (_data != nullptr) {
            cout << "  Data: [";
            for (size_t i = 0; i < min(_size, static_cast<size_t>(5)); ++i) {
                cout << _data[i] << " ";
            }
            if (_size > 5) {
                cout << "...";
            }
            cout << "]" << endl;
        }
    }
};

// =========================================
// 5. 资源管理最佳实践
// =========================================
// 学习要点：以对象管理资源；优先使用智能指针；避免使用裸指针

// 示例1：使用智能指针管理资源
class ResourceManager {
private:
    // 使用unique_ptr管理独占资源
    unique_ptr<Resource> _unique_resource;
    
    // 使用shared_ptr管理共享资源
    shared_ptr<Resource> _shared_resource;
    
    // 使用weak_ptr避免循环引用
    weak_ptr<Resource> _weak_resource;
    
public:
    ResourceManager() {
        // 使用make_unique和make_shared创建智能指针（推荐）
        _unique_resource = make_unique<Resource>("unique_resource");
        _shared_resource = make_shared<Resource>("shared_resource");
        _weak_resource = _shared_resource;
        
        cout << "ResourceManager constructed" << endl;
    }
    
    ~ResourceManager() {
        cout << "ResourceManager destructed" << endl;
        // 智能指针会自动释放资源，无需手动操作
    }
    
    void use_resources() {
        _unique_resource->use();
        _shared_resource->use();
        
        // 使用weak_ptr前检查是否过期
        if (auto locked = _weak_resource.lock()) {
            locked->use();
        } else {
            cout << "Weak resource has expired" << endl;
        }
    }
    
    // 转移资源所有权的方法
    unique_ptr<Resource> release_unique_resource() {
        return move(_unique_resource);
    }
    
    // 共享资源的访问方法
    shared_ptr<Resource> get_shared_resource() const {
        return _shared_resource;
    }
};

// 示例2：线程安全的资源管理
class ThreadSafeCounter {
private:
    mutable mutex _mutex; // mutable允许const方法修改
    atomic<int> _count{0}; // 原子变量，线程安全
    
public:
    // 线程安全的增加操作
    void increment() {
        _count.fetch_add(1, memory_order_relaxed);
    }
    
    // 线程安全的减少操作
    void decrement() {
        _count.fetch_sub(1, memory_order_relaxed);
    }
    
    // 线程安全的获取操作
    int get() const {
        return _count.load(memory_order_relaxed);
    }
    
    // 使用互斥锁保护复杂操作
    void complex_operation() {
        lock_guard<mutex> lock(_mutex);
        // 复杂操作，需要互斥保护
        int current = _count.load(memory_order_relaxed);
        // 模拟复杂计算
        this_thread::sleep_for(chrono::milliseconds(10));
        _count.store(current * 2, memory_order_relaxed);
    }
};

// =========================================
// 6. 类设计最佳实践
// =========================================
// 学习要点：单一职责原则；封装；最小接口；不可变性

// 示例：遵循单一职责原则的类设计
class Logger {
private:
    string _log_file;
    ofstream _file_stream;
    mutex _mutex;
    
public:
    explicit Logger(const string& log_file = "app.log") 
        : _log_file(log_file) {
        _file_stream.open(log_file, ios::app);
        if (!_file_stream.is_open()) {
            cerr << "Failed to open log file: " << log_file << endl;
        }
    }
    
    ~Logger() {
        if (_file_stream.is_open()) {
            _file_stream.close();
        }
    }
    
    // 禁止拷贝，允许移动
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    Logger(Logger&& other) noexcept 
        : _log_file(move(other._log_file)), 
          _file_stream(move(other._file_stream)) {
    }
    
    Logger& operator=(Logger&& other) noexcept {
        if (this != &other) {
            _log_file = move(other._log_file);
            _file_stream = move(other._file_stream);
        }
        return *this;
    }
    
    // 线程安全的日志记录方法
    template<typename... Args>
    void log(const string& message, Args&&... args) {
        lock_guard<mutex> lock(_mutex);
        
        // 格式化时间
        auto now = chrono::system_clock::now();
        auto time_t = chrono::system_clock::to_time_t(now);
        
        // 写入日志到控制台
        cout << "[" << ctime(&time_t) << "] " << message;
        
        // 写入日志到文件
        if (_file_stream.is_open()) {
            _file_stream << "[" << ctime(&time_t) << "] " << message;
            _file_stream.flush();
        }
    }
    
    void info(const string& message) {
        log("INFO: " + message + "\n");
    }
    
    void error(const string& message) {
        log("ERROR: " + message + "\n");
    }
    
    void debug(const string& message) {
        #ifdef DEBUG
        log("DEBUG: " + message + "\n");
        #endif
    }
};

// =========================================
// 7. 模板编程最佳实践
// =========================================
// 学习要点：了解隐式接口和编译期多态；避免过度模板化；使用SFINAE

// 示例1：简单的模板函数
// 隐式接口：T必须支持+运算符
 template<typename T>
T add(const T& a, const T& b) {
    return a + b;
}

// 示例2：类型安全的模板函数
 template<typename T>
void safe_copy(T* dest, const T* src, size_t count) {
    static_assert(is_trivially_copyable<T>::value, "T must be trivially copyable");
    memcpy(dest, src, count * sizeof(T));
}

// 示例3：模板类的特化
 template<typename T>
class TypeTraits {
private:
    static void test(int) {
        // 匹配算术类型
    }
    
    static void test(double) {
        // 匹配浮点类型
    }
    
    static void test(const void*) {
        // 匹配指针类型
    }
    
public:
    static const bool is_arithmetic = is_arithmetic<T>::value;
    static const bool is_floating_point = is_floating_point<T>::value;
    static const bool is_pointer = is_pointer<T>::value;
    static const bool is_const = is_const<T>::value;
    
    static string get_type_name() {
        if constexpr (is_arithmetic<T>) {
            if constexpr (is_floating_point<T>) {
                return "floating_point";
            } else {
                return "arithmetic";
            }
        } else if constexpr (is_pointer<T>) {
            return "pointer";
        } else if constexpr (is_same<T, string>::value) {
            return "string";
        } else {
            return "unknown";
        }
    }
};

// =========================================
// 主函数：测试所有最佳实践
// =========================================
int main() {
    cout << "=========================================" << endl;
    cout << "ESP-Brookesia C++ 最佳实践学习" << endl;
    cout << "=========================================" << endl;
    
    // 1. RAII原则测试
    cout << "\n1. RAII原则测试：" << endl;
    cout << "-----------------------------------------" << endl;
    try {
        // 文件RAII测试
        FileHandler file("test.txt", "w");
        file.write("Hello, RAII!\n");
        file.write("This is a test file.\n");
        
        // 移动语义测试
        FileHandler file2 = move(file);
        file2.write("Appended by moved file handler.\n");
        
        // 动态数组RAII测试
        DynamicArray arr(5);
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = static_cast<int>(i * 2);
        }
        cout << "DynamicArray values: [";
        for (size_t i = 0; i < arr.size(); ++i) {
            cout << arr[i] << " ";
        }
        cout << "]" << endl;
        
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    
    // 2. 虚析构函数测试
    cout << "\n2. 虚析构函数测试：" << endl;
    cout << "-----------------------------------------" << endl;
    demonstrate_virtual_destructor();
    
    // 3. 接口设计测试
    cout << "\n3. 接口设计测试：" << endl;
    cout << "-----------------------------------------" << endl;
    
    // Builder模式测试
    ComplexObject obj1 = ComplexObject::Builder("app1")
        .version(2)
        .debug_mode(true)
        .max_connections(100)
        .log_file("app1_debug.log")
        .build();
    obj1.print_info();
    
    ComplexObject obj2 = ComplexObject::Builder("app2").build();
    obj2.print_info();
    
    // 4. 构造/析构/赋值测试
    cout << "\n4. 构造/析构/赋值测试：" << endl;
    cout << "-----------------------------------------" << endl;
    WellDesignedClass obj3("obj3", 10);
    WellDesignedClass obj4 = obj3; // 拷贝构造
    WellDesignedClass obj5;
    obj5 = obj3; // 拷贝赋值
    WellDesignedClass obj6 = move(obj5); // 移动构造
    WellDesignedClass obj7;
    obj7 = move(obj6); // 移动赋值
    
    obj3.print_info();
    obj4.print_info();
    obj7.print_info();
    
    // 5. 资源管理测试
    cout << "\n5. 资源管理测试：" << endl;
    cout << "-----------------------------------------" << endl;
    ResourceManager resource_manager;
    resource_manager.use_resources();
    
    // 6. 线程安全测试
    cout << "\n6. 线程安全测试：" << endl;
    cout << "-----------------------------------------" << endl;
    ThreadSafeCounter counter;
    
    // 创建多个线程同时访问计数器
    vector<thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&counter]() {
            for (int j = 0; j < 1000; ++j) {
                counter.increment();
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    cout << "Final counter value: " << counter.get() << endl;
    
    // 7. 模板编程测试
    cout << "\n7. 模板编程测试：" << endl;
    cout << "-----------------------------------------" << endl;
    
    // 测试模板函数
    cout << "add(1, 2) = " << add(1, 2) << endl;
    cout << "add(1.5, 2.5) = " << add(1.5, 2.5) << endl;
    cout << "add(string(\"hello\"), string(\" world\")) = " << add(string("hello"), string(" world")) << endl;
    
    // 测试类型 traits
    cout << "TypeTraits<int>::get_type_name(): " << TypeTraits<int>::get_type_name() << endl;
    cout << "TypeTraits<double>::get_type_name(): " << TypeTraits<double>::get_type_name() << endl;
    cout << "TypeTraits<int*>::get_type_name(): " << TypeTraits<int*>::get_type_name() << endl;
    cout << "TypeTraits<string>::get_type_name(): " << TypeTraits<string>::get_type_name() << endl;
    
    cout << "\n=========================================" << endl;
    cout << "最佳实践学习完成！" << endl;
    cout << "=========================================" << endl;
    
    return 0;
}

// ============================================================================//
// 代码学习要点总结
// ============================================================================//
/*
1. RAII 原则：
   - 资源在构造时获取，在析构时释放
   - 确保资源正确管理，避免内存泄漏
   - 禁止拷贝或实现正确的拷贝语义
   - 允许移动语义，提高性能
   - 适用于文件、锁、内存、网络连接等各种资源

2. 虚析构函数：
   - 多态基类必须声明虚析构函数
   - 确保通过基类指针删除派生类对象时，所有析构函数都被正确调用
   - 避免内存泄漏和未定义行为

3. 接口设计最佳实践：
   - 让接口容易被正确使用，不易被误用
   - 使用RAII管理资源，避免手动资源管理
   - 返回值优先于输出参数
   - 使用const修饰符保护不变状态
   - 采用Builder模式构建复杂对象
   - 避免使用裸指针，优先使用智能指针

4. 构造/析构/赋值运算：
   - 了解编译器自动生成的函数：默认构造、拷贝构造、拷贝赋值、移动构造、移动赋值、析构
   - 正确实现五大函数，或显式禁用不需要的函数
   - 遵循"规则三/五"：如果需要自定义拷贝构造、拷贝赋值或析构函数中的任何一个，
     通常需要自定义所有三个（C++11前）；C++11后则为五个

5. 资源管理：
   - 以对象管理资源（RAII）
   - 优先使用智能指针：unique_ptr（独占所有权）、shared_ptr（共享所有权）、weak_ptr（避免循环引用）
   - 使用make_unique和make_shared创建智能指针（更高效、更安全）
   - 避免使用裸指针，尤其是在资源管理中
   - 线程安全的资源访问

6. 类设计最佳实践：
   - 单一职责原则：一个类只负责一个功能
   - 封装：隐藏内部实现细节，提供清晰的接口
   - 最小接口：只暴露必要的方法
   - 不可变性：尽可能使对象不可变
   - 线程安全：根据需要提供线程安全的接口
   - 异常安全：确保异常发生时资源不泄漏

7. 模板编程最佳实践：
   - 了解隐式接口和编译期多态
   - 避免过度模板化，保持代码可读性
   - 使用SFINAE和constexpr实现编译期决策
   - 利用类型 traits 进行类型检查和转换
   - 提供模板特化以优化特定类型
   - 使用static_assert进行编译期断言

8. 其他最佳实践：
   - 优先使用标准库，避免重复造轮子
   - 使用constexpr进行编译期计算
   - 采用现代C++特性：auto、lambda表达式、移动语义等
   - 编写清晰、简洁、可维护的代码
   - 遵循项目的代码风格和命名规范
*/
