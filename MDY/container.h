#pragma once
//容器类，是其他类的基类，主要用于描述作为一个容器应该有的基本特征
//----------------------------------------------------

//容器应该是模板类,定义全局变量类型，

template <typename value_t>
struct container {
    
    using value_type = value_t;    //内部的类型名value_type是全局value_type的别名
    using pointer = value_type*;        //指针类型
    using reference = value_type&;      //左值引用类型

};