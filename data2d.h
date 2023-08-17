#pragma once
#include <vector>

template <typename T>
struct Data2d{
    std::vector<T> m_data; // Ordering: column-major
    size_t m_columns;
    size_t m_rows;
    operator bool() const { return m_data.size() > 0; }
};