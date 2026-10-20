#include "introalgo/sorting/sort.hpp"

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace introalgo {

void DigitKey::show(std::ostream& os) const {
    for (int d : key) os << d << " ";
    os << std::endl;
}

Sort::Sort(std::vector<int> a) : arr(std::move(a)) {}

void Sort::bubble_sort() {
    int flag = 1, tmp;
    for (int i = 1; i < size() && flag == 1; i++) {
        flag = 0;
        for (int j = 0; j < size() - i; j++) {
            if (arr[j] > arr[j + 1]) {
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
                flag = 1;
            }
        }
    }
}

void Sort::insert_sort() {
    int tmp, j;
    for (int i = 1; i < size(); i++) {
        tmp = arr[i];
        j = i;
        while (j > 0 && tmp < arr[j - 1]) {
            arr[j] = arr[j - 1];
            j--;
        }
        arr[j] = tmp;
    }
}

void Sort::select_sort() {
    int min, tmp;
    for (int i = 0; i < size(); i++) {
        min = i;
        for (int j = i; j < size(); j++) {
            if (arr[j] < arr[min]) min = j;
        }
        tmp = arr[min];
        arr[min] = arr[i];
        arr[i] = tmp;
    }
}

int Sort::partition_qsa_1(int a, int b) {
    if (a >= b) return -1;
    int i = a, j = a, tmp = arr[b], swa;
    while (j < b) {
        if (arr[j] < tmp) {
            swa = arr[i];
            arr[i] = arr[j];
            arr[j] = swa;
            i++;
        }
        j++;
    }
    swa = arr[b];
    arr[b] = arr[i];
    arr[i] = swa;
    return i;
}

int Sort::partition_qsa_2(int a, int b) {
    if (a >= b) return -1;
    int tmp = arr[a], i = a, j = b;
    while (i < j) {
        while (j > i && arr[j] >= tmp) j--;
        arr[i] = arr[j];
        while (i < j && arr[i] <= tmp) i++;
        arr[j] = arr[i];
    }
    arr[i] = tmp;
    return i;
}

void Sort::quick_sort(int a, int b, Partition scheme) {
    if (b >= size()) throw std::out_of_range("Sort::quick_sort: range past the end");
    if (a < 0) throw std::out_of_range("Sort::quick_sort: negative start");
    // Recurse into the smaller side and loop on the larger one: the stack stays O(log n)
    // even for already sorted input (where the recursion of the original went n levels deep).
    while (a < b) {
        const int i = scheme == Partition::Lomuto ? partition_qsa_1(a, b) : partition_qsa_2(a, b);
        if (i - a < b - i) {
            quick_sort(a, i - 1, scheme);
            a = i + 1;
        } else {
            quick_sort(i + 1, b, scheme);
            b = i - 1;
        }
    }
}

void Sort::quick_sort(Partition scheme) {
    quick_sort(0, size() - 1, scheme);
}

void Sort::counting_sort() {
    if (arr.empty()) return;
    int min = arr[0], max = arr[0];
    for (int i = 1; i < size(); i++) {
        if (arr[i] > max) max = arr[i];
        if (arr[i] < min) min = arr[i];
    }
    const long long range = static_cast<long long>(max) - min + 1;
    if (range > (1LL << 24)) throw std::length_error("Sort::counting_sort: value range too large");
    const int n = static_cast<int>(range);
    std::vector<int> brr(arr.size());
    std::vector<int> crr(static_cast<std::size_t>(n), 0);
    for (int i = 0; i < size(); i++) {
        crr[arr[i] - min]++;
    }
    for (int i = 1; i < n; i++) {
        crr[i] += crr[i - 1];
    }
    for (int i = size() - 1; i >= 0; i--) {  // backwards, so equal keys keep their order
        brr[--crr[arr[i] - min]] = arr[i];
    }
    arr = std::move(brr);
}

void Sort::aux_counting_radix(std::vector<DigitKey>& ds,
                              int digi) const {  // sorting based on the digit-th digit value
    const int n = static_cast<int>(ds.size());
    if (n == 0) return;
    int min = ds[0].key.at(static_cast<std::size_t>(digi)), max = min;
    for (int i = 0; i < n; i++) {
        const int v = ds[i].key.at(static_cast<std::size_t>(digi));
        if (v > max) max = v;
        if (v < min) min = v;
    }
    const long long range = static_cast<long long>(max) - min + 1;
    if (range > (1LL << 24)) throw std::length_error("Sort::radix_sort: digit range too large");
    const int m = static_cast<int>(range);
    std::vector<DigitKey> brr(ds.size());
    std::vector<int> crr(static_cast<std::size_t>(m), 0);
    for (int i = 0; i < n; i++) crr[ds[i].key[digi] - min]++;
    for (int i = 1; i < m; i++) crr[i] += crr[i - 1];
    for (int i = n - 1; i >= 0; i--) {
        brr[--crr[ds[i].key[digi] - min]] = std::move(ds[i]);
    }
    ds = std::move(brr);
}

void Sort::radix_sort(std::vector<DigitKey>& t, int digit) const {
    if (digit < 0) throw std::invalid_argument("Sort::radix_sort: negative digit count");
    if (!t.size()) return;
    for (const auto& d : t)
        if (d.digits() < digit)
            throw std::invalid_argument("Sort::radix_sort: record with too few digits");
    for (int i = digit - 1; i >= 0; i--) {
        aux_counting_radix(t, i);
    }
}

void Sort::checkout(std::ostream& os) const {
    for (int i = 0; i < size(); i++) os << arr[i] << " ";
}

}  // namespace introalgo
