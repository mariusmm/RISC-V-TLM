#ifndef ADDR_RANGE_H
#define ADDR_RANGE_H

#include <cstdint>
/**
 * The address_range class in C++ is a simple class that represents a range of memory addresses. 
 * It has two public member variables s and e, which represent the start and end addresses of the range, respectively.
*/
class address_range{
    public:

        /**
         * Constructs an address_range object with the given start and end addresses.
         *
         * @param s The start address of the range.
         * @param e The end address of the range.
         *
         */
        address_range(std::uint64_t const s, std::uint64_t const e) : s(s<=e?s:e), e(s<=e?e:s) {}

        /**
         * Overloads the less than operator to compare two address_range objects.
         *
         * @param y The address_range object to compare with.
         *
         * @return true if the end address of this address_range is less than the start address of the other address_range, false otherwise.
         */
        bool operator < (const address_range& y ) const
        {
            return this->e < y.s;
        }

        /**
         * Checks if the given value is within the range of this address_range object.
         *
         * @param value The value to check.
         *
         * @return True if the value is within the range, false otherwise.
         */
        bool search(std::uint64_t value) const {
            return (value >= s && value <= e);
        }

    public:
        uint64_t s, e;        
    };
#endif