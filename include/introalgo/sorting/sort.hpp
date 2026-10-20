#pragma once

#include <cstddef>
#include <iosfwd>
#include <utility>
#include <vector>

namespace introalgo {

/// A record with several integer "digits" and some satellite data, sorted by radix sort
/// (called `dat` in the original sort.cpp).
struct DigitKey {
    std::vector<int> key;
    int data = 0;

    DigitKey() = default;
    /// `size` digits, all zero.
    explicit DigitKey(int size) : key(static_cast<std::size_t>(size < 0 ? 0 : size), 0) {}
    explicit DigitKey(std::vector<int> a) : key(std::move(a)) {}

    int digits() const { return static_cast<int>(key.size()); }
    /// Is this record's i-th digit greater than d's?
    bool comp(const DigitKey& d, int i) const { return key[i] > d.key[i]; }
    void show(std::ostream& os) const;
};

/// The classic comparison and distribution sorts, working on an internal array of ints.
///
///  algorithm        time (worst)   extra space   stable
///  bubble sort      O(n^2)         O(1)          yes
///  insertion sort   O(n^2)         O(1)          yes
///  selection sort   O(n^2)         O(1)          no
///  quick sort       O(n^2)*        O(log n)      no      (* expected O(n log n))
///  counting sort    O(n + range)   O(n + range)  yes
///  radix sort       O(d (n + k))   O(n + k)      yes
class Sort {
public:
    enum class Partition {
        Lomuto,  ///< partition_qsa_1: last element is the pivot, single left-to-right scan
        Hole,    ///< partition_qsa_2: first element is the pivot, two pointers close in from both
                 ///< ends
    };

    Sort() = default;
    explicit Sort(std::vector<int> a);

    void bubble_sort();
    void insert_sort();
    void select_sort();

    /// Sorts the positions a..b (inclusive).  Throws std::out_of_range if b is past the end.
    void quick_sort(int a, int b, Partition scheme = Partition::Hole);
    /// Sorts everything.
    void quick_sort(Partition scheme = Partition::Hole);
    /// Partitions arr[a..b] around a pivot and returns its final position (-1 if a >= b).
    int partition_qsa_1(int a, int b);
    int partition_qsa_2(int a, int b);

    /// Throws std::length_error if max - min + 1 exceeds 2^24 (the count array would be huge).
    void counting_sort();

    /// LSD radix sort of records with `digit` digits each (digit 0 is the most significant).
    /// Throws std::invalid_argument if digit is negative or a record has fewer digits.
    void radix_sort(std::vector<DigitKey>& t, int digit) const;
    /// One stable counting-sort pass on the digit-th digit.
    void aux_counting_radix(std::vector<DigitKey>& ds, int digit) const;

    const std::vector<int>& data() const { return arr; }
    void checkout(std::ostream& os) const;  // prints the array on one line

private:
    std::vector<int> arr;
    int size() const { return static_cast<int>(arr.size()); }
};

}  // namespace introalgo
