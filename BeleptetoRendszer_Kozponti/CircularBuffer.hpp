/**
 ***************************************************************************************************
 * @file CircularBuffer.hpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief A circular buffer class template.
 ***************************************************************************************************
 */

#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

/**
 * @brief A circular buffer class.
 * @tparam T The type of the elements in the buffer.
 * @tparam BUFFER_SIZE The size of the buffer.
 */
template <typename T, int BUFFER_SIZE>
class CircularBuffer
{
    T buffer[BUFFER_SIZE];
    int head = 0;
    int tail = 0;

public:
    /**
     * @brief Put an element into the buffer.
     * @param value The element to put into the buffer.
     * @return True if the element was put into the buffer successfully, false otherwise.
     */
    bool enqueue(T value)
    {
        int next = (head + 1) % BUFFER_SIZE;
        if (next != tail)
        {
            buffer[head] = value;
            head = next;
            return true;
        }
        return false;
    }

    /**
     * @brief Get an element from the buffer.
     * @param value Pointer to the location where the element will be put.
     * @return True if the element was got from the buffer successfully, false otherwise.
     */
    bool dequeue(T *value)
    {
        if (tail == head)
        {
            return false;
        }
        *value = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        return true;
    }

    /**
     * @brief Get the element at the tail of the buffer.
     * @return Pointer to the element at the tail of the buffer.
     */
    const T *peek()
    {
        return buffer[tail];
    }

    /**
     * @brief Get the number of elements in the buffer.
     * @return The number of elements in the buffer.
     */
    int size()
    {
        return (head + BUFFER_SIZE - tail) % BUFFER_SIZE;
    }
};

#endif /* CIRCULARBUFFER_HPP */
