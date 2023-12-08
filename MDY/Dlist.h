#pragma once
//同样采用双向链表-----该头文件就是对dlist头文件的一个简化
//注意，dlist应当只用于管理而非创建
#include <iostream>
#include <exception>
#include <initializer_list>
#include <string>
#include "container.h"
using namespace std;

//-----------------------------------------------------------------------------
template < typename value_t >
class dlist: public container < value_t > {
//-----------------------------------------------------公有的类型定义
public:
    using value_type = typename container<value_t>::value_type;
    using pointer = typename container<value_t>::pointer;
    using reference = typename container<value_t>::reference;
    using callback = void (value_type &);

    //考虑到有些函数要访问node，暂时放在public段
    struct node {
        //这样每个对象内部可以自带属性，包括其路径，名字等。
        value_type data;//参与操作的是类对象obj，可以是table，也可以是database，还可以是一个map
        node *next = nullptr, *prior = nullptr; //指针域
        //调用该构造函数通过传参直接创建一个带有前驱，后继，数据的结点
        node(node *n, node *p, value_type d) : data(d), next(n), prior(p) {}

        node() {
            next = nullptr;
        }
    };
private:

    node head;
    node tail;    //头尾结点
    size_t length;      //节点数

    using nodeptr_t = node*;    //定义节点指针类型
    using range = nodeptr_t;

//内部的操作，用于提高代码的重用性

    //获取当前结点的内容
    reference _get(const nodeptr_t p) try {
        if (empty()) //链表空时抛出异常
            throw std::out_of_range("empty list");
        return p->data;
    } catch (std::out_of_range &e) { //捕获异常
        std::cout << e.what() << std::endl;
        exit(1);
    }

    //将节点p插入到节点pre之后--此时并未给出p的next和prior，会在调用该函数声明结点p时显示给出
    void _push(const nodeptr_t pre, const nodeptr_t p) noexcept {
        pre->next->prior = p;
        pre->next = p;

        ++length;
    }

    //在节点pre之后删除一个节点
    void _pop(const nodeptr_t pre) try {
        if (empty())
            throw std::out_of_range("empty list");

        auto p = pre->next;
        pre->next = p->next;
        p->next->prior = pre;
        delete p;

        --length;
    } catch (std::out_of_range &e) {
        std::cout << e.what() << std::endl;
        exit(1);
    }

    //删除整个链表
    constexpr void _destroy() noexcept {
        for (auto p = head.next; p != &tail; p = head.next) {
            head.next = p->next;
            delete p;
        }

    }

    //初始化链表l-----一般只在构造函数时调用
    void _init() noexcept {
        length = 0;
        head.next = &tail;
        tail.prior = &head;
    }

public:
//----------------------------------------------类的构造函数和析构函数
    //默认构造函数
    dlist() {
        _init();
    }

    //重载的构造函数，使之可以一次性创建多个数据-----压入的数据类型也是由模板给出，体现了模板的优点
    dlist(const std::initializer_list<value_type> &l) noexcept : dlist() {
        for (auto &v : l)
            //这里使用尾插法，可同时插入多个结点
            push_back(v);
    }
    //下面只是多加了几个构造函数,使之能支持更多的链表初始化,不用管

//----------------------------------------------------------

    //复制控制1：复制构造函数
    //采用深复制策略
    //参数必须是本类型的常量左值引用
    dlist(const dlist &l) noexcept : dlist() {
        for (auto p = l.head.next; p != &l.tail; p = p->next)
            push_back(p->data);
    }

    //复制控制2：转移复制构造函数
    //this链表准备接管l链表的资源
    //参数必须是本类型的非常量右值引用
    dlist(dlist &&l) noexcept : head(l.head), tail(l.tail), length(l.length) {
        l.head.next->prior = &head; //l链表的第一个节点的前向指针现在指向this链表的head
        l.tail.prior->next = &tail; //l链表的最后一个节点的后向指针现在指向this链表的tail
        l._init();  //l链表置为空链表。现在l的资源已经全部转移给了this，l可以安全的销毁了。
    }

    //复制控制3：重载的赋值运算符函数
    constexpr dlist &operator=(const dlist &l) {
        _destroy(); //释放this对象原有的资源
        _init();    //初始化this对象

        for (auto p = l.head.next; p != &l.tail; p = p->next)
            push_back(p->data);

        return *this;  //函数返回后，l立即失效，但它的资源已经转移给了this
    }

    //复制控制4：重载的转移赋值运算符函数
    //思路：交换this和l链表的资源，使this保持l的资源，而用l去释放this原来的资源。
    constexpr dlist &operator=(dlist &&l) {
        //交换两个链表的头节点的后向指针，使它们各自指向正确的后继（第一个）节点
        std::swap(head.next, l.head.next);
        //交换两个链表的第一个节点的前向指针，使它们各自指向正确的头节点
        std::swap(head.next->prior, l.head.next->prior);

        //交换两个链表的尾节点的前向指针，使它们各自指向正确的前驱（最后一个）节点
        std::swap(tail.prior, l.tail.prior);
        //交换两个链表的最后一个节点的后向指针，使它们各自指向正确的尾节点
        std::swap(tail.prior->next, l.tail.prior->next);

        std::swap(length, l.length);

        return *this; //函数返回后，l立即失效，它会释放this链表原来的资源
    }

//----------------------------------------------------------
    //destructor
    ~dlist() noexcept {
        _destroy();
    }

//----------------------------------类的抽象方法
    //作为链表，应该含有：

    //查找结点，a表示第几个结点，找到该节点并返回指向该节点的指针
    nodeptr_t find(int a) {
        int flag = 1;
        auto p = head.next;
        for (; flag < a; p = p->next, ++flag)
            ;
        return p;
    }
    
    //重载的find函数，用于查找对应名字的结点并返回指向该节点的指针
    nodeptr_t find(string str) {
        size_t flag = 1;
        bool flag_1 = true;
        auto p = head.next;
        for (; (p->data).get_name() != str && flag_1 == true ; p = p->next, flag++) {
            //表示遍历后无此结点
            if (flag == this->length) {
                flag_1 = false;
                return nullptr;
            }
        }

        return p;
    }

    //链表的头插
    void push_front(const value_type &v) {
        _push(&head, new node(head.next, &head, v));
    }

    //链表的尾插
    void push_back(const value_type &v) {
        _push(tail.prior, new node(&tail, tail.prior, v));
    }

    //得到指定第num结点的数据
    value_type get(int num) {
        //运用查找函数找到第num个结点
        auto p = find(num);
        return _get(p);
    }

    //重载的查找函数
    value_type get(string str) {
        //运用查找函数找到name为str的结点
        auto p = find(str);
        if(p==nullptr){
            cout<<"object not exist, please try again"<<endl;
            value_type m;
            m.set_flag_false();
            return m;
        }
        return _get(p);
    }
    //在链表l头部获取数据
    value_type front() {
        return _get(head.next);
    }

    //在链表l尾部获取数据
    value_type back() {
        return _get(tail.prior);
    }

//------一般database和MDYS都不要求在指定结点插入，只有table类需要在指定行或列之间插入数据-------

    //4.链表指定在节点num后插入
    void push(int num, const value_type &v) {

        //运用查找函数，找到该结点
        auto p = find(num);
        _push(p, new node(&tail, tail.prior, v));

    }

    //5.链表指定删除第num号节点
    void pop(int num) {
        //运用查找函数，找到该结点
        auto p = find(num - 1);
        //因为_pop函数是删除参数后面的一个结点，所有num-1
        _pop(p);

    }

    //重载的pop函数，链表指定删除名字域为str的结点
    void pop(const string &str) {
        //运用查找函数，找到该结点
        auto p = find(str);
        //因为_pop函数是删除参数后面的一个结点，所有num-1
        _pop(p->prior);
    }

    //判断链表l是否为空。常成员。
    constexpr bool empty() {
        return length == 0;
    }

    //重载的empty函数，用于判断具有某属性的结点是否在链表内
    bool empty(const string str) {
        bool flag = true;
        auto p = head.next;
        for (; p != &tail; p = p->next) {
            if ((p->data.get_name()).compare(str) == 0) {
                flag = false;
                break;
            }
        }
        return flag;
    }

    void show_all_node() {
        for (auto p = head.next; p != &tail; p = p->next) {
            cout << p->data.get_name() << endl;
        }
    }

    //从头到尾遍历链表。参数f是一个回调函数
    void traverse(callback &&f) {
        for (auto p = head.next; p != &tail; p = p->next)
            f(p->data.get_name()); //调用回调函数
    }

    node *get_head() {
        auto p = &head;
        return p;
    }

    node *get_tail() {
        auto p = &tail;
        return p;
    }

    size_t get_length(){
        return this->length;
    }
};