#ifndef SORTED_ARRAY_RPG_H
#define SORTED_ARRAY_RPG_H
#include <cassert>

template<typename T, std::size_t Size>
class Array {
public:
    T& operator[](std::size_t index) { return m_data[index]; }
    const T& operator[](std::size_t index) const { return m_data[index]; }
    std::size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    void insert_at(std::size_t insert_point, T value)
    {
        // Move all elements to the right by an index
        std::size_t curr = m_size++;
        assert(m_size <= Size);
        while(curr > insert_point) {
            m_data[curr] = m_data[curr-1];
            --curr;
        }
        m_data[insert_point] = value;
    }

    void erase(std::size_t index)
    {
        std::size_t curr = index;
        --m_size;
        assert(m_size >= 0);
        // Move all elements to the left by an index
        while(curr < m_size) {
            m_data[curr] = m_data[curr+1];
            ++curr;
        }
    }

    void append(T value)
    {
        m_data[m_size++] = value;
        assert(m_size <= Size);
    }
protected:
    std::size_t m_size = 0;
    T m_data[Size];
};


template<typename T, std::size_t Size>
class SortedArray final : public Array<T, Size> {
    using Array<T, Size>::m_size;
    using Array<T, Size>::m_data;
public:
    std::size_t index_of(T value) const
    {
        int low = 0;
        int high = m_size;
        while (low < high) {
            int mid =  low + (high - low) / 2;
            if (value <= m_data[mid]) {
                high = mid;
            } else {
                low = mid + 1;
            }
        }
        return low;
    }

    void insert(T value)
    {
        const std::size_t insert_point = index_of(value);
        insert_at(insert_point, value);
    }
};
#endif
