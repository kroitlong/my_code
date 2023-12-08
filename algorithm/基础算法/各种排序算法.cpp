#include <bits/stdc++.h>
#include <random>
using namespace std;
/*
 @brief 一般的冒泡排序,逐层遍历容器，每次需要遍历的数量比上一次少1
        ，每次遍历过程中通过比较交换相邻元素，将较大的元素逐个转移
        到本次循环的最后一位。时间复杂度为O(N^2).
*/
template <typename T>
void bubble_sort_n(vector<T> &list)
{
    for (auto i = 0; i < list.size() - 1; i++)
    {
        for (auto j = 0; j < list.size() - i - 1; j++)
        {
            if (list[j] > list[j + 1])
                swap(list[j], list[j + 1]);
        }
    }
}

/*
 @brief 可以及时终止的冒泡排序，逐层遍历容器，每次需要遍历的数量比上一次少1
        ，每次遍历过程中通过比较交换相邻元素，将较大的元素逐个转移到本次循环
        的最后一位。与一般的冒泡排序相比，当某一层遍历过程中没有发生交换，则
        说明容器有序，不再继续遍历。时间复杂度为O(N^2).

*/
template <typename T>
void bubble_sort_s(vector<T> &list)
{
    for (auto i = 0; i < list.size() - 1; i++)
    {
        bool is_order = true;
        for (auto j = 0; j < list.size() - i - 1; j++)
        {
            if (list[j] > list[j + 1])
            {
                swap(list[j], list[j + 1]);
                is_order = false;
            }
        }
        if (is_order)
            return;
    }
}

/*
 @brief 选择排序，与冒泡排序相比，并非逐个元素比较，而是直接调用函数max_element
        获得容器中指向最大值元素的迭代器，并将其与本次循环过程的最后一位元素交换位
        置，同样是逐层循环，每次循环遍历元素数量比上一次少1，与冒泡排序的时间复杂度
        同为O(N^2)。
*/
template <typename T>
void selection_sort_n(vector<T> &list)
{
    for (size_t i = list.size(); i > 0; i--)
    {
        auto max_ele = max_element(list.begin(), list.begin() + i);
        swap(*max_ele, *(list.begin() + i - 1));
    }
}

/*
 @brief 可以及时终止的选择排序，在内层循环遍历时如果发现内层已经有序则停止排序。
        与冒泡排序的时间复杂度同为O(N^2)。
*/
template <typename T>
void selection_sort_s(vector<T> &list)
{
    bool sorted = false;
    for (size_t i = list.size(); i > 0 && !sorted; i--)
    {
        int max_index = 0;
        sorted = true;
        for (size_t j = 1; j < i; j++)
            if (list[j] > list[max_index])
                max_index = j;
            else
                sorted = false;
        swap(list[max_index], list[i - 1]);
    }
}

/*
 @brief 插入排序,使用多层循环，每次一循环产生一个有序队列，下一次循环将新的元素插入
        到有序队列中适当的位置，时间复杂度为O(N^2).
*/
template <typename T>
void insertion_sort(vector<T> &list)
{
    for (size_t i = 1; i < list.size(); i++)
    {
        int index = 0;
        for (size_t j = 0; j < i; j++)
        {
            if (list[j] < list[i])
                index++;
        }
        T temp = list[i];
        list.erase(list.begin() + i);
        list.insert(list.begin() + index, temp);
    }
}

/*
 @brief 快速排序,基于递归实现快速排序，首先随机选取一个基准值（此处选择每次排序的最左值）
        ，将待排序列中小于基准值的元素置于基准值左侧，大于基准值的元素置于基准值右侧，递归
        排列新形成的两个子序列。时间复杂度为O(nlogn)。
*/
template <typename T>
void quick_sort(vector<T> &list, int l, int r)
{
    if (l >= r)
    {
        return;
    }
    else
    {
        T base = list[l];
        int dis = l; // 用来记录基准值的下标
        for (size_t i = l + 1; i < r; i++)
        {
            auto base_t = find(list.begin() + l, list.begin() + r, base);
            if (list[i] < *base_t)
            {
                T temp = list[i];
                list.erase(list.begin() + i);
                list.insert(base_t, temp);
                dis++;
            }
        }
        quick_sort(list, l, dis);
        quick_sort(list, dis + 1, r);
    }
}

/*
 @brief 归并排序，将原序列递归的细分为小序列后，再将小序列两两合并成新的有序序列，经过
 递归的分割融合最终使得整体有序，稳定，时间复杂度为O(nlogn)
*/
template <typename T>
void merge_sort(vector<T> &list, int l, int r)
{ // 分割
    if (l >= r)
        return;
    int mid = (l + r) / 2;
    merge_sort(list, l, mid);
    merge_sort(list, mid + 1, r);

    // 融合
    vector<T> left(list.begin() + l, list.begin() + mid + 1);
    vector<T> right(list.begin() + mid + 1, list.begin() + r + 1);
    // 再left和right的尾部追加MAX_INT，防止比较时发生数组越界
    left.insert(left.end(), numeric_limits<int>::max());
    right.insert(right.end(), numeric_limits<int>::max());
    int left_index = 0, right_index = 0;
    for (size_t i = l; i <= r; i++)
    {
        if (left[left_index] < right[right_index])
        {
            list[i] = left[left_index];
            left_index++;
        }
        else
        {
            list[i] = right[right_index];
            right_index++;
        }
    }
}

/*
 @brief 堆的维护函数，保证堆始终为大根堆
*/
template <typename T>
void heapfiy(vector<T> &list, int n, int cur)
{ // 维护堆为大根堆
    if (cur > n / 2 - 1)
        return;
    int temp = cur;
    int left_node = 2 * cur + 1;
    int right_node = 2 * cur + 2;
    if (left_node < n && list[cur] < list[left_node])
    {
        cur = left_node;
    }
    if (right_node < n && list[cur] < list[right_node])
    {
        cur = right_node;
    }
    if (cur != temp)
    {
        swap(list[temp], list[cur]);
        heapfiy(list, n, cur);
    }
}
/*
 @brief 堆排序,首先根据list建立一个大根堆，建堆过程从list的中间位置开始
        (表示第一个非叶子节点)，向前逐个调用维护函数，在堆的维护函数中递
        归的下沉元素。堆顶元素总为list的最值。逐个将最值移动到最后即可完
        成排序建立堆的过程时间复杂度为O(N)，排序过程时间复杂度为O(NlogN).
*/
template <typename T>
void heap_sort(vector<T> &list)
{ // 执行建堆和排序
    int n = list.size();
    // 建堆O(n)
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapfiy(list, n, i);
    }
    // 排序O(nlogn)
    for (int i = n - 1; i > 0; i--)
    {
        swap(list[i], list[0]);
        heapfiy(list, i, 0);
    }
}

/*
 @brief 计数排序,时间复杂度为O(n+k)
*/
template <typename T>
void counting_sort(vector<T> &list)
{
    auto max_num = max_element(list.begin(), list.end());
    auto min_num = min_element(list.begin(), list.end());
    T range = *max_num - *min_num + 1;
    // 统计
    vector<T> count(range, 0);
    for (int i = 0; i < list.size(); i++)
    {
        count[list[i] - *min_num]++;
    }
    // 累加
    for (int i = 1; i < range; i++)
    {
        count[i] += count[i - 1];
    }
    // 排序
    vector<T> temp(list.size(), 0);
    for (int i = range - 1; i >= 0;)
    {
        if (count[i] - count[i - 1] == 0)
        {
            i--;
            continue;
        }
        int index = count[i] - 1;
        temp[index] = i + *min_num;
        count[i]--;
    }
    list = temp;
}

/*
 @brief 桶排序,将数据根据合适的映射函数拆分后放入m个桶中（即容器中），映射后左
        侧桶中的数据总要小于右侧桶中的数据。每个桶再使用其他的算法进行排序（一般）
        使用快排。时间复杂度为O(n+n*(lnn-lnm)),这里我们选择有10个桶
*/
template <typename T>
void bucket_sort(vector<T> &list)
{
    auto max_num = max_element(list.begin(), list.end());
    auto min_num = min_element(list.begin(), list.end());
    T range = *max_num - *min_num + 1;
    vector<vector<T>> bucket(10);
    // 放入桶中
    for (int i = 0; i < list.size(); i++)
    {
        int index = (list[i] - *min_num) * 10 / range;
        bucket[index].push_back(list[i]);
    }
    // 桶内排序
    for (int i = 0; i < 10; i++)
    {
        quick_sort(bucket[i], 0, bucket[i].size());
    }
    // 重新组合
    vector<T> temp;
    for (int i = 0; i < 10; i++)
    {
        temp.insert(temp.end(), bucket[i].begin(), bucket[i].end());
    }
    list = temp;
}

/*
 @brief 为基数排序服务的一类特殊的计数排序，基于(原数%基数)的值对原序列进行排序
        使用基数个桶，每个桶所装的值通过余数确定。
*/
template <typename T>
void counting_sort_for_radix(vector<T> &list, T radix, int n)
{
    vector<vector<T>> bucket(radix);
    for (int i = 0; i < list.size(); i++)
    {
        int index = static_cast<T>(list[i] / pow(radix, n)) % radix;
        bucket[index].push_back(list[i]);
    }
    // 将每个桶的值拼接即可得到一次基数排列的序列
    vector<T> temp;
    for (int i = 0; i < radix; i++)
    {
        temp.insert(temp.end(), bucket[i].begin(), bucket[i].end());
    }
    list = temp;
}
/*
 @brief 基数排序,基于特殊的计数排序实现，对元素的每位分别进行计数排序，由于计数排序
        是稳定的，从低位到高位最终可以实现序列的排序。
*/
template <typename T>
void radix_sort(vector<T> &list)
{
    int radix = 10; // 基数为10进行排序
    auto max_it = max_element(list.begin(), list.end());
    T max_num = *max_it;
    int digits = 0;
    while (max_num != 0)
    {
        max_num /= radix;
        digits++;
    }
    for (size_t n = 0; n < digits; n++)
    {
        counting_sort_for_radix(list, radix, n);
    }
}

int main(void)
{
    // int n, m;
    // cin >> n;
    vector<int> a;
    // for (size_t i = 0; i < n; i++) {
    //     cin >> m;
    //     a.push_back(m);
    // }
    // 使用随机数生成器生成随机序列
    std::random_device rd;  // 用于获取种子
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    // 定义要生成的范围
    int lower_bound = 1;
    int upper_bound = 100;
    std::uniform_int_distribution<> distribution(lower_bound, upper_bound);

    // 生成 10 个随机数
    int n = 10;
    for (int i = 0; i < n; ++i)
    {
        a.push_back(distribution(gen));
    }
    // 基于STL::sort的排序
    vector<int> b(a);
    sort(b.begin(), b.end());
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <基于STL::sort的排序(升序)";
    cout << endl;

    b = a; // 重置b
    sort(b.begin(), b.end(), [](int a, int b)
         { return a >= b; });
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <基于STL::sort的排序(降序)";
    cout << endl;

    // 基于上述各类排序--默认排序结果为升序
    // 一般的冒泡排序
    b = a;
    bubble_sort_n(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <一般的冒泡排序";
    cout << endl;

    // 可以及时终止的冒泡排序
    b = a;
    bubble_sort_s(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <及时终止的冒泡排序";
    cout << endl;

    // 选择排序
    b = a;
    selection_sort_n(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <选择排序";
    cout << endl;

    // 选择排序
    b = a;
    selection_sort_s(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <及时终止的选择排序";
    cout << endl;

    // 插入排序
    b = a;
    insertion_sort(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <插入排序";
    cout << endl;

    // 快速排序
    b = a;
    quick_sort(b, 0, n);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <快速排序";
    cout << endl;

    // 归并排序
    b = a;
    merge_sort(b, 0, n - 1);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <归并排序";
    cout << endl;

    // 堆排序
    b = a;
    heap_sort(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <堆排序";
    cout << endl;

    // 计数排序
    b = a;
    counting_sort(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <计数排序";
    cout << endl;

    // 桶排序
    b = a;
    bucket_sort(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <桶排序";
    cout << endl;

    // 基数排序
    b = a;
    radix_sort(b);
    copy(b.begin(), b.end(), ostream_iterator<int>(cout, " "));
    cout << "   <基数排序";
    cout << endl;
}