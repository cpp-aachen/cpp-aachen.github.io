/**
    based on: https://github.com/olawlor/osl/blob/master/alignocator.h
    from: Dr. Orion Lawlor, lawlor@alaska.edu, 2018-03-20 (Public Domain)

    based on: http://blogs.msdn.com/b/vcblog/archive/2008/08/28/the-alignocator.aspx

 Use it to allocate a std::vector for AVX floats like this:
    std::vector<float, alignocator<float,32> >   myVec;
*/
#pragma once

#include <cstdlib>

#include <stdexcept> // Required for std::length_error

template<typename T, int alignment>
class align_allocator
{
  public:
    // The following will be the same for virtually all allocators.
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    align_allocator() = default;
    align_allocator(const align_allocator&) = default;
    ~align_allocator() = default;

    align_allocator& operator=(const align_allocator&) = delete;

    T* address(T& r) const { return &r; }
    const T* address(const T& s) const { return &s; }
    size_t max_size() const { return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T); }

    template<typename U>
    struct rebind
    {
        typedef align_allocator<U, alignment> other;
    };

    void construct(T* const p, const T& t) const
    {
        void* const pv = static_cast<void*>(p);
        new (pv) T(t);
    }

    void destroy(T* const p) const { p->~T(); }

    // bool operator!=(const align_allocator& other) const { return !(*this == other); }
    bool operator!=(const align_allocator& other) const { return false; }
    bool operator==(const align_allocator& other) const { return true; }

    template<typename U>
    align_allocator(const align_allocator<U, alignment>&)
    {}

    template<typename U>
    T* allocate(const size_t n, const U* /* const hint */) const
    {
        return allocate(n);
    }
    T* allocate(const size_t n) const
    {
        if (n == 0) { return nullptr; }
        if (n > max_size()) { throw std::length_error("align_allocator<T>::allocate() - Integer overflow."); }
#ifndef _MSC_VER  
        void* const pv = std::aligned_alloc(alignment, n * sizeof(T));
#else
        void* const pv = _aligned_malloc(n * sizeof(T), alignment);
#endif
        if (pv == nullptr) { throw std::bad_alloc(); }

        return static_cast<T*>(pv);
    }

    void deallocate(T* const p, const size_t n) const {
#ifndef _MSC_VER  
        std::free(p);
#else
        _aligned_free(p);
#endif
    }
};
