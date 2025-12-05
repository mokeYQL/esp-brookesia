// ============================================================================//
// ESP-Brookesia 项目 C++ 学习文件 03_advanced_features.cpp
// 高级特性：智能指针、STL容器、模板、C++11特性
// ============================================================================//

#include <iostream>
#include <string>
#include <memory>       // 智能指针
#include <vector>       // 向量容器
#include <list>         // 链表容器
#include <map>          // 映射容器
#include <unordered_map>// 无序映射
#include <set>          // 集合容器
#include <algorithm>    // 算法库
#include <functional>   // 函数对象
#include <iterator>     // 迭代器
#include <numeric>      // 数值算法
#include <utility>      // 工具库（如std::move）

using namespace std;

// =========================================
// 1. 智能指针（Smart Pointers）
// =========================================
// 智能指针用于自动内存管理，避免内存泄漏
// 学习要点：unique_ptr、shared_ptr、weak_ptr、自定义删除器

class Resource {
private:
    string _name;
    
public:
    Resource(const string& name) : _name(name) {
        cout << "Resource created: " << _name << endl;
    }
    
    ~Resource() {
        cout << "Resource destroyed: " << _name << endl;
    }
    
    void use() {
        cout << "Using resource: " << _name << endl;
    }
    
    const string& get_name() const {
        return _name;
    }
};

// 示例1：std::unique_ptr（独占所有权）
void unique_ptr_example() {
    cout << "\n=== unique_ptr 示例 ===" << endl;
    
    // 方式1：通过make_unique创建（推荐）
    unique_ptr<Resource> res1 = make_unique<Resource>("res1");
    res1->use();
    
    // 方式2：通过构造函数创建
    unique_ptr<Resource> res2(new Resource("res2"));
    res2->use();
    
    // 所有权转移
    unique_ptr<Resource> res3 = move(res1);
    // res1->use(); // 错误：res1不再拥有资源
    res3->use();
    
    // 释放资源
    res3.reset(); // 显式释放资源
    
    // 创建数组
    unique_ptr<Resource[]> res_array(new Resource[3]{
        Resource("array1"),
        Resource("array2"),
        Resource("array3")
    });
    res_array[0].use();
    res_array[1].use();
    
    cout << "unique_ptr 示例结束" << endl;
}

// 示例2：std::shared_ptr（共享所有权）
void shared_ptr_example() {
    cout << "\n=== shared_ptr 示例 ===" << endl;
    
    // 方式1：通过make_shared创建（推荐，更高效）
    shared_ptr<Resource> res1 = make_shared<Resource>("shared1");
    cout << "res1 use_count: " << res1.use_count() << endl;
    
    // 方式2：通过构造函数创建
    shared_ptr<Resource> res2(new Resource("shared2"));
    
    // 共享所有权
    shared_ptr<Resource> res3 = res1;
    cout << "res1 use_count after copy: " << res1.use_count() << endl;
    cout << "res3 use_count: " << res3.use_count() << endl;
    
    // 通过引用计数观察生命周期
    { // 作用域开始
        shared_ptr<Resource> res4 = res1;
        cout << "Inside scope, use_count: " << res1.use_count() << endl;
    } // 作用域结束，res4被销毁
    cout << "Outside scope, use_count: " << res1.use_count() << endl;
    
    // 重置共享指针
    res1.reset();
    cout << "After res1.reset(), res3 use_count: " << res3.use_count() << endl;
    
    // 使用shared_ptr管理数组（C++17及以上支持）
    #if __cplusplus >= 201703L
    shared_ptr<Resource[]> shared_array = make_shared<Resource[]>(2);
    shared_array[0].use();
    #endif
    
    cout << "shared_ptr 示例结束" << endl;
}

// 示例3：std::weak_ptr（弱引用）
void weak_ptr_example() {
    cout << "\n=== weak_ptr 示例 ===" << endl;
    
    shared_ptr<Resource> shared_res = make_shared<Resource>("weak_target");
    cout << "shared_res use_count: " << shared_res.use_count() << endl;
    
    // 创建weak_ptr
    weak_ptr<Resource> weak_res = shared_res;
    cout << "After creating weak_ptr, use_count: " << shared_res.use_count() << endl;
    cout << "weak_res expired? " << (weak_res.expired() ? "yes" : "no") << endl;
    
    // 通过lock()获取shared_ptr（安全访问）
    if (auto temp_res = weak_res.lock()) {
        cout << "Locked weak_ptr, resource name: " << temp_res->get_name() << endl;
        cout << "After lock(), use_count: " << shared_res.use_count() << endl;
    }
    
    // 测试资源释放后weak_ptr的状态
    shared_res.reset();
    cout << "After shared_res.reset(), weak_res expired? " << (weak_res.expired() ? "yes" : "no") << endl;
    
    // 尝试锁定已过期的weak_ptr
    if (auto temp_res = weak_res.lock()) {
        temp_res->use(); // 不会执行，因为资源已释放
    } else {
        cout << "Failed to lock weak_ptr: resource already released" << endl;
    }
    
    cout << "weak_ptr 示例结束" << endl;
}

// 示例4：自定义删除器
void custom_deleter_example() {
    cout << "\n=== 自定义删除器示例 ===" << endl;
    
    // 自定义删除器函数
    auto custom_deleter = [](Resource* res) {
        cout << "Custom deleter called for: " << res->get_name() << endl;
        delete res;
    };
    
    // 使用自定义删除器的unique_ptr
    unique_ptr<Resource, decltype(custom_deleter)> res1(new Resource("custom1"), custom_deleter);
    
    // 使用自定义删除器的shared_ptr
    shared_ptr<Resource> res2(new Resource("custom2"), custom_deleter);
    
    cout << "Custom deleter example结束" << endl;
}

// =========================================
// 2. STL 容器和算法
// =========================================
// STL提供了丰富的容器和算法，用于高效的数据处理
// 学习要点：vector、list、map、set等容器的使用，以及常用算法

// 示例1：vector（动态数组）
void vector_example() {
    cout << "\n=== vector 示例 ===" << endl;
    
    // 创建vector
    vector<int> vec1;
    vector<int> vec2(5, 0); // 创建5个元素，初始值为0
    vector<int> vec3 = {1, 2, 3, 4, 5}; // C++11初始化列表
    
    // 基本操作
    vec1.push_back(10);
    vec1.push_back(20);
    vec1.push_back(30);
    
    cout << "vec1 size: " << vec1.size() << endl;
    cout << "vec1 capacity: " << vec1.capacity() << endl;
    cout << "vec1 empty? " << (vec1.empty() ? "yes" : "no") << endl;
    
    // 访问元素
    cout << "vec1[1]: " << vec1[1] << endl;
    cout << "vec1.at(2): " << vec1.at(2) << endl;
    cout << "vec1 front: " << vec1.front() << endl;
    cout << "vec1 back: " << vec1.back() << endl;
    
    // 遍历方式1：传统for循环
    cout << "遍历vec3（传统for）: ";
    for (size_t i = 0; i < vec3.size(); i++) {
        cout << vec3[i] << " ";
    }
    cout << endl;
    
    // 遍历方式2：范围for循环（C++11）
    cout << "遍历vec3（范围for）: ";
    for (int num : vec3) {
        cout << num << " ";
    }
    cout << endl;
    
    // 遍历方式3：迭代器
    cout << "遍历vec3（迭代器）: ";
    for (vector<int>::iterator it = vec3.begin(); it != vec3.end(); ++it) {
        cout << *it << " ";
    }
    cout << endl;
    
    // 算法：排序
    vector<int> vec4 = {5, 2, 8, 1, 9};
    sort(vec4.begin(), vec4.end());
    cout << "排序后vec4: ";
    for (int num : vec4) {
        cout << num << " ";
    }
    cout << endl;
    
    // 算法：查找
    auto find_it = find(vec3.begin(), vec3.end(), 3);
    if (find_it != vec3.end()) {
        cout << "找到元素3，索引: " << distance(vec3.begin(), find_it) << endl;
    }
    
    // 算法：累加
    int sum = accumulate(vec3.begin(), vec3.end(), 0);
    cout << "vec3累加和: " << sum << endl;
    
    // 删除元素
    vec1.pop_back(); // 删除最后一个元素
    vec3.erase(vec3.begin() + 1); // 删除索引1的元素
    vec3.erase(remove(vec3.begin(), vec3.end(), 4), vec3.end()); // 删除所有值为4的元素
    
    cout << "vector 示例结束" << endl;
}

// 示例2：list（双向链表）
void list_example() {
    cout << "\n=== list 示例 ===" << endl;
    
    list<int> my_list = {5, 1, 8, 3, 9};
    
    // 基本操作
    my_list.push_back(10);
    my_list.push_front(0);
    
    cout << "list size: " << my_list.size() << endl;
    
    // 遍历
    cout << "遍历list: ";
    for (int num : my_list) {
        cout << num << " ";
    }
    cout << endl;
    
    // list特有操作
    my_list.sort();
    cout << "排序后list: ";
    for (int num : my_list) {
        cout << num << " ";
    }
    cout << endl;
    
    my_list.reverse();
    cout << "反转后list: ";
    for (int num : my_list) {
        cout << num << " ";
    }
    cout << endl;
    
    // 插入和删除
    auto it = my_list.begin();
    advance(it, 2);
    my_list.insert(it, 100);
    
    it = my_list.begin();
    advance(it, 4);
    my_list.erase(it);
    
    cout << "操作后list: ";
    for (int num : my_list) {
        cout << num << " ";
    }
    cout << endl;
    
    cout << "list 示例结束" << endl;
}

// 示例3：map（有序映射）和unordered_map（无序映射）
void map_example() {
    cout << "\n=== map 示例 ===" << endl;
    
    // 创建map
    map<string, int> student_scores;
    
    // 插入元素
    student_scores["Alice"] = 95;
    student_scores["Bob"] = 87;
    student_scores.insert({"Charlie", 92});
    student_scores.insert(make_pair("David", 89));
    
    // 访问元素
    cout << "Alice's score: " << student_scores["Alice"] << endl;
    
    // 安全访问（避免插入默认值）
    auto alice_it = student_scores.find("Alice");
    if (alice_it != student_scores.end()) {
        cout << "Found Alice, score: " << alice_it->second << endl;
    }
    
    // 遍历map
    cout << "所有学生成绩: " << endl;
    for (const auto& pair : student_scores) {
        cout << pair.first << ": " << pair.second << endl;
    }
    
    // map大小
    cout << "学生数量: " << student_scores.size() << endl;
    
    // 删除元素
    student_scores.erase("Bob");
    cout << "删除Bob后学生数量: " << student_scores.size() << endl;
    
    cout << "\n=== unordered_map 示例 ===" << endl;
    
    // 创建unordered_map（哈希表实现，查找更快）
    unordered_map<string, string> capitals;
    capitals["China"] = "Beijing";
    capitals["USA"] = "Washington D.C.";
    capitals["Japan"] = "Tokyo";
    
    cout << "中国首都: " << capitals["China"] << endl;
    
    // 遍历unordered_map
    cout << "所有国家首都: " << endl;
    for (const auto& pair : capitals) {
        cout << pair.first << ": " << pair.second << endl;
    }
    
    cout << "map 示例结束" << endl;
}

// 示例4：set（有序集合）
void set_example() {
    cout << "\n=== set 示例 ===" << endl;
    
    // 创建set（自动排序，无重复元素）
    set<int> numbers = {5, 2, 8, 2, 9, 5, 1};
    
    cout << "set大小: " << numbers.size() << endl; // 输出5（去重后）
    
    // 遍历set
    cout << "set元素: ";
    for (int num : numbers) {
        cout << num << " ";
    }
    cout << endl; // 输出：1 2 5 8 9
    
    // 插入元素
    numbers.insert(3);
    numbers.insert(7);
    
    // 查找元素
    auto find_it = numbers.find(5);
    if (find_it != numbers.end()) {
        cout << "找到元素5" << endl;
    }
    
    // 检查元素是否存在
    if (numbers.count(10) > 0) {
        cout << "元素10存在" << endl;
    } else {
        cout << "元素10不存在" << endl;
    }
    
    // 删除元素
    numbers.erase(2);
    
    cout << "set 示例结束" << endl;
}

// =========================================
// 3. 模板（Templates）
// =========================================
// 模板实现泛型编程，提高代码复用性
// 学习要点：函数模板、类模板、模板特化、可变参数模板

// 示例1：函数模板
// 基本函数模板
 template<typename T>
T add(const T& a, const T& b) {
    return a + b;
}

// 函数模板重载
template<typename T>
T add(const T& a, const T& b, const T& c) {
    return a + b + c;
}

// 示例2：类模板
 template<typename T>
class Stack {
private:
    vector<T> _elements;
    
public:
    // 压栈
    void push(const T& element) {
        _elements.push_back(element);
    }
    
    // 出栈
    void pop() {
        if (!empty()) {
            _elements.pop_back();
        }
    }
    
    // 获取栈顶元素
    T& top() {
        return _elements.back();
    }
    
    const T& top() const {
        return _elements.back();
    }
    
    // 检查栈是否为空
    bool empty() const {
        return _elements.empty();
    }
    
    // 获取栈大小
    size_t size() const {
        return _elements.size();
    }
    
    // 打印栈元素
    void print() const {
        cout << "Stack elements: [";
        for (const auto& elem : _elements) {
            cout << elem << " ";
        }
        cout << "]" << endl;
    }
};

// 示例3：模板特化
// 为string类型特化Stack的print方法
template<> 
void Stack<string>::print() const {
    cout << "String Stack elements: [";
    for (const auto& elem : _elements) {
        cout << '\"' << elem << '\"' << " ";
    }
    cout << "]" << endl;
}

// 示例4：可变参数模板（C++11）
// 基本可变参数模板函数
template<typename... Args>
void print_args(const Args&... args) {
    // 折叠表达式（C++17）
    (cout << ... << (cout << ", " << args)) << endl;
}

// 递归可变参数模板
void recursive_print() {
    // 递归终止条件
    cout << endl;
}

template<typename T, typename... Args>
void recursive_print(const T& first, const Args&... rest) {
    cout << first;
    if constexpr (sizeof...(rest) > 0) {
        cout << ", ";
    }
    recursive_print(rest...);
}

// =========================================
// 4. C++11 及以上特性
// =========================================
// 学习要点：自动类型推导、lambda表达式、右值引用、范围for循环等

// 示例1：自动类型推导（auto和decltype）
void auto_decltype_example() {
    cout << "\n=== auto和decltype示例 ===" << endl;
    
    // auto自动推导类型
    auto i = 42; // int
    auto d = 3.14; // double
    auto s = "hello";
    auto vec = vector<int>{1, 2, 3};
    auto map = map<string, int>{{"a", 1}, {"b", 2}};
    
    cout << "auto推导结果: " << endl;
    cout << "i = " << i << " (type: " << typeid(i).name() << ")" << endl;
    cout << "d = " << d << " (type: " << typeid(d).name() << ")" << endl;
    
    // decltype获取表达式类型
    decltype(i) j = 100; // j是int类型
    decltype(vec) another_vec; // another_vec是vector<int>类型
    
    // decltype(auto)（C++14）
    auto get_vec = []() { return vector<int>{1, 2, 3}; };
    decltype(auto) vec_copy = get_vec();
    cout << "vec_copy size: " << vec_copy.size() << endl;
    
    cout << "auto和decltype示例结束" << endl;
}

// 示例2：lambda表达式
void lambda_example() {
    cout << "\n=== lambda表达式示例 ===" << endl;
    
    // 基本lambda
    auto add = [](int a, int b) { return a + b; };
    cout << "3 + 5 = " << add(3, 5) << endl;
    
    // 带捕获列表的lambda
    int base = 100;
    auto add_to_base = [base](int num) { return base + num; };
    cout << "base + 20 = " << add_to_base(20) << endl;
    
    // 引用捕获
    auto modify_base = [&base](int num) { base += num; };
    modify_base(50);
    cout << "修改后的base = " << base << endl;
    
    // 隐式捕获
    auto implicit_capture = [=](int num) { return base + num; }; // 值捕获所有变量
    auto implicit_ref_capture = [&]() { base++; }; // 引用捕获所有变量
    implicit_ref_capture();
    cout << "base after implicit ref capture: " << base << endl;
    
    // mutable lambda（允许修改值捕获的变量）
    int counter = 0;
    auto increment = [counter]() mutable { return ++counter; };
    cout << "第一次调用increment: " << increment() << endl;
    cout << "第二次调用increment: " << increment() << endl;
    cout << "counter原值: " << counter << endl; // 仍为0，因为是值捕获
    
    // lambda在算法中的应用
    vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    vector<int> even_nums;
    
    copy_if(nums.begin(), nums.end(), back_inserter(even_nums), 
             [](int num) { return num % 2 == 0; });
    
    cout << "偶数: ";
    for (int num : even_nums) {
        cout << num << " ";
    }
    cout << endl;
    
    // 排序：降序
    sort(nums.begin(), nums.end(), [](int a, int b) { return a > b; });
    cout << "降序排序后: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << endl;
    
    cout << "lambda表达式示例结束" << endl;
}

// 示例3：右值引用和移动语义
void move_semantics_example() {
    cout << "\n=== 移动语义示例 ===" << endl;
    
    // 模拟一个大对象
    class BigObject {
    private:
        vector<int> _data;
        
    public:
        BigObject(size_t size) : _data(size, 0) {
            cout << "BigObject构造：" << _data.size() << " 个元素" << endl;
        }
        
        // 拷贝构造函数
        BigObject(const BigObject& other) : _data(other._data) {
            cout << "BigObject拷贝构造" << endl;
        }
        
        // 移动构造函数（C++11）
        BigObject(BigObject&& other) noexcept : _data(move(other._data)) {
            cout << "BigObject移动构造" << endl;
        }
        
        // 移动赋值运算符
        BigObject& operator=(BigObject&& other) noexcept {
            if (this != &other) {
                _data = move(other._data);
                cout << "BigObject移动赋值" << endl;
            }
            return *this;
        }
        
        size_t size() const { return _data.size(); }
    };
    
    // 测试移动构造
    cout << "\n--- 测试移动构造 ---" << endl;
    BigObject obj1(1000000);
    BigObject obj2 = move(obj1); // 调用移动构造函数
    cout << "obj1 size after move: " << obj1.size() << endl;
    cout << "obj2 size: " << obj2.size() << endl;
    
    // 测试移动赋值
    cout << "\n--- 测试移动赋值 ---" << endl;
    BigObject obj3(500000);
    BigObject obj4(100000);
    obj4 = move(obj3); // 调用移动赋值运算符
    cout << "obj3 size after move: " << obj3.size() << endl;
    cout << "obj4 size: " << obj4.size() << endl;
    
    // 标准库中的移动语义
    cout << "\n--- 标准库移动语义 ---" << endl;
    string s1 = "Hello, world!";
    string s2 = move(s1);
    cout << "s2: " << s2 << endl;
    cout << "s1: \"" << s1 << "\" (s1已被移动，内容不确定)" << endl;
    
    cout << "移动语义示例结束" << endl;
}

// 示例5：constexpr（编译期常量）
void constexpr_example() {
    cout << "\n=== constexpr示例 ===" << endl;
    
    // constexpr变量
    constexpr int MAX_SIZE = 1024;
    constexpr double PI = 3.1415926535;
    
    // constexpr函数（C++11，C++14后更灵活）
    constexpr int factorial(int n) {
        return n <= 1 ? 1 : n * factorial(n - 1);
    }
    
    constexpr int fact5 = factorial(5);
    cout << "5! = " << fact5 << endl;
    
    // 编译期计算数组大小
    constexpr int arr_size = 5;
    int arr[factorial(arr_size)];
    cout << "数组大小: " << sizeof(arr) / sizeof(arr[0]) << endl;
    
    cout << "constexpr示例结束" << endl;
}

// =========================================
// 主函数：测试所有高级特性
// =========================================
int main() {
    cout << "=========================================" << endl;
    cout << "ESP-Brookesia C++ 高级特性学习" << endl;
    cout << "=========================================" << endl;
    
    // 智能指针测试
    unique_ptr_example();
    shared_ptr_example();
    weak_ptr_example();
    custom_deleter_example();
    
    // STL容器测试
    vector_example();
    list_example();
    map_example();
    set_example();
    
    // 模板测试
    cout << "\n=== 模板示例 ===" << endl;
    cout << "add(2, 3) = " << add(2, 3) << endl;
    cout << "add(2, 3, 4) = " << add(2, 3, 4) << endl;
    cout << "add(1.5, 2.5) = " << add(1.5, 2.5) << endl;
    
    // 测试Stack类模板
    Stack<int> int_stack;
    int_stack.push(10);
    int_stack.push(20);
    int_stack.push(30);
    int_stack.print();
    cout << "栈顶元素: " << int_stack.top() << endl;
    int_stack.pop();
    int_stack.print();
    
    // 测试string类型的Stack
    Stack<string> string_stack;
    string_stack.push("hello");
    string_stack.push("world");
    string_stack.print();
    
    // 可变参数模板测试
    cout << "\n可变参数模板测试: " << endl;
    print_args(1, 2, 3, 4, 5);
    recursive_print("apple", "banana", "cherry");
    
    // C++11特性测试
    auto_decltype_example();
    lambda_example();
    move_semantics_example();
    constexpr_example();
    
    cout << "\n=========================================" << endl;
    cout << "高级特性学习完成！" << endl;
    cout << "=========================================" << endl;
    
    return 0;
}

// ============================================================================//
// 代码学习要点总结
// ============================================================================//
/*
1. 智能指针：
   - unique_ptr：独占所有权，不能复制，只能移动
   - shared_ptr：共享所有权，使用引用计数，线程安全
   - weak_ptr：弱引用，不增加引用计数，用于解决循环引用问题
   - make_unique/make_shared：推荐的创建方式，更高效和安全
   - 自定义删除器：用于特殊资源的释放

2. STL容器：
   - vector：动态数组，随机访问高效
   - list：双向链表，插入删除高效
   - map：有序映射，基于红黑树
   - unordered_map：无序映射，基于哈希表
   - set：有序集合，自动去重
   - 容器适配器：stack、queue、priority_queue

3. STL算法：
   - 排序：sort、stable_sort
   - 查找：find、binary_search
   - 数值：accumulate、count
   - 修改：transform、replace
   - 遍历：for_each

4. 模板编程：
   - 函数模板：实现泛型函数
   - 类模板：实现泛型类
   - 模板特化：为特定类型提供特殊实现
   - 可变参数模板：处理任意数量的参数
   - 折叠表达式：C++17特性，简化可变参数处理

5. C++11 及以上特性：
   - auto：自动类型推导
   - decltype：获取表达式类型
   - lambda表达式：匿名函数，简化函数对象
   - 右值引用和移动语义：提高性能，减少拷贝
   - constexpr：编译期计算
   - 范围for循环：简化遍历
   - nullptr：空指针常量
   - 列表初始化：统一的初始化语法
*/
