#pragma once

#include <type_traits>
#include <span>
#include <algorithm>
#include <atomic>
#include <shared_mutex>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include "types.hpp"
#include "panic.hpp"

namespace tmine {

template <class T>
class ThreadsafeVec;

template <class T>
class ThreadsafeVecLock {
public:
    using Parent = ThreadsafeVec<T>;

    ~ThreadsafeVecLock() = default;

    ThreadsafeVecLock(ThreadsafeVecLock&) = delete;
    ThreadsafeVecLock(ThreadsafeVecLock&&) noexcept = default;

    auto operator=(this ThreadsafeVecLock& self, ThreadsafeVecLock const&)
        -> ThreadsafeVecLock& = delete;
    auto operator=(this ThreadsafeVecLock& self, ThreadsafeVecLock&&)
        -> ThreadsafeVecLock& = default;

    explicit ThreadsafeVecLock(Parent* parent);

    auto operator[](this ThreadsafeVecLock& self, usize index) -> T&;
    auto get_unchecked(this ThreadsafeVecLock& self, usize index) -> T&;

    auto as_span(this ThreadsafeVecLock& self) -> std::span<T>;

    auto begin(this ThreadsafeVecLock& self) -> T*;
    auto end(this ThreadsafeVecLock& self) -> T*;
    auto size(this ThreadsafeVecLock const& self) -> usize;

    auto erase(this ThreadsafeVecLock& self, T* begin, T* end) -> void;

    auto reserve_exact(this ThreadsafeVecLock& self, usize amount) -> void;
    auto reserve(this ThreadsafeVecLock& self, usize amount) -> void;

    auto push(this ThreadsafeVecLock& self, T value) -> void;
    auto pop(this ThreadsafeVecLock& self) -> T;

    auto capacity(this ThreadsafeVecLock const& self) -> usize;

private:
    Parent* parent;
    std::unique_lock<std::shared_mutex> guard;
};

template <class T>
class ThreadsafeVec {
    friend class ThreadsafeVecLock<T>;

public:
    ThreadsafeVec()
    : ptr{nullptr}
    , len{0}
    , cap{0}
    , mutex{} {}

    ~ThreadsafeVec()
        requires(!std::is_trivially_destructible_v<T>)
    {
        for (usize i = 0; i < this->len; ++i) {
            this->ptr[i].~T();
        }

        free(this->ptr);
    }

    ~ThreadsafeVec()
        requires std::is_trivially_destructible_v<T>
    {
        free(this->ptr);
    }

    ThreadsafeVec(ThreadsafeVec&) = delete;

    ThreadsafeVec(ThreadsafeVec&& other) noexcept
    : ptr{other.ptr}
    , len{(usize) other.len}
    , cap{other.cap}
    , mutex{} {
        other.ptr = nullptr;
        other.len = 0;
        other.cap = 0;
    }

    auto operator=(this ThreadsafeVec& self, ThreadsafeVec const&)
        -> ThreadsafeVec& = delete;

    auto operator=(this ThreadsafeVec& self, ThreadsafeVec&& other) noexcept
        -> ThreadsafeVec& {
        self.ptr = other.ptr;
        self.len = (usize) other.len;
        self.cap = other.cap;

        other.ptr = nullptr;
        other.len = 0;
        other.cap = 0;
    }

    auto data(this ThreadsafeVec const& self) -> T* { return self.ptr; }

    auto size(this ThreadsafeVec const& self) -> usize { return self.len; }

    auto begin(this ThreadsafeVec const& self) -> T* { return self.ptr; }

    auto end(this ThreadsafeVec const& self) -> T* {
        return self.ptr + self.len;
    }

    auto empty(this ThreadsafeVec const& self) -> bool { return 0 == self.len; }

    auto clear(this ThreadsafeVec& self) -> void
        requires(!std::is_trivially_destructible_v<T>)
    {
        for (usize i = 0; i < self.len; ++i) {
            self.ptr[i].~T();
        }

        self.len = 0;
    }

    auto clear(this ThreadsafeVec& self) -> void
        requires std::is_trivially_destructible_v<T>
    {
        self.len = 0;
    }

    auto operator[](this ThreadsafeVec& self, usize index) -> T& {
        return self.ptr[index];
    }

    auto operator[](this ThreadsafeVec const& self, usize index) -> T const& {
        return self.ptr[index];
    }

    static auto consteval initial_capacity() -> usize { return 1; }

    static auto constexpr next_capacity(usize cap) -> usize {
        return 3 * (cap + 1) / 2;
    }

private:
    auto reserve_exact_occupy(
        this ThreadsafeVec& self, usize requested_len, usize additional_cap
    ) -> usize {
        {
            auto _shared_guard = std::shared_lock{self.mutex};
            auto const prev_len =
                self.len.fetch_add(requested_len, std::memory_order_seq_cst);

            if (prev_len + additional_cap <= self.cap) {
                return prev_len;
            } else {
                self.len.fetch_sub(requested_len, std::memory_order_seq_cst);
            }
        }

        auto _unique_guard = std::unique_lock{self.mutex};

        if (self.len + additional_cap <= self.cap) {
            self.len += requested_len;
            return self.len - requested_len;
        }

        if (0 == self.cap) {
            self.cap = additional_cap;
            self.ptr = (T*) std::malloc(sizeof(*self.ptr) * self.cap);
        } else if (self.cap - self.len < additional_cap) {
            self.cap += additional_cap;
            self.ptr =
                (T*) std::realloc(self.ptr, sizeof(*self.ptr) * self.cap);
        }

        self.len += requested_len;
        return self.len - requested_len;
    }

    auto reserve_occupy(this ThreadsafeVec& self, usize requested_len)
        -> usize {
        self.mutex.lock_shared();
        auto cap = self.cap;
        self.mutex.unlock_shared();

        return self.reserve_exact_occupy(
            requested_len,
            std::max(requested_len, ThreadsafeVec::next_capacity(cap) - cap)
        );
    }

public:
    auto append(this ThreadsafeVec& self, std::span<T> values) -> void
        requires(!std::is_trivially_copy_constructible_v<T> && std::is_move_constructible_v<T>)
    {
        auto const prev_len = self.reserve_occupy(values.size());

        auto _shared_guard = std::shared_lock{self.mutex};

        for (usize i = 0; i < values.size(); ++i) {
            new (self.ptr + prev_len + i) T{std::move(values[i])};
        }
    }

    auto append(this ThreadsafeVec& self, std::span<T> values) -> void
        requires std::is_trivially_copy_constructible_v<T>
    {
        auto const prev_len = self.reserve_occupy(values.size());

        auto _shared_guard = std::shared_lock{self.mutex};

        std::memcpy(
            self.ptr + prev_len, values.data(), sizeof(T) * values.size()
        );
    }

    auto push(this ThreadsafeVec& self, T value) -> void {
        auto const prev_len = self.reserve_occupy(1);

        auto _shared_guard = std::shared_lock{self.mutex};

        new (self.ptr + prev_len) T{std::move(value)};
    }

    auto lock(this ThreadsafeVec& self) -> ThreadsafeVecLock<T> {
        return ThreadsafeVecLock<T>{&self};
    }

private:
    T* ptr;
    std::atomic<usize> len;
    usize cap;
    std::shared_mutex mutex;
};

template <class T>
ThreadsafeVecLock<T>::ThreadsafeVecLock(Parent* parent)
: parent{parent}
, guard{parent->mutex} {}

template <class T>
auto ThreadsafeVecLock<T>::get_unchecked(
    this ThreadsafeVecLock& self, usize index
) -> T& {
    return self.parent->ptr[index];
}

template <class T>
auto ThreadsafeVecLock<T>::operator[](this ThreadsafeVecLock& self, usize index)
    -> T& {
    if (index >= self.parent->len) {
        throw Panic(
            "index '{}' out of bounds 0..{}", index, (usize) self.parent->len
        );
    }

    return self.get_unchecked(index);
}

template <class T>
auto ThreadsafeVecLock<T>::as_span(this ThreadsafeVecLock& self)
    -> std::span<T> {
    return std::span<T>{self.parent->ptr, self.parent->ptr + self.parent->len};
}

template <class T>
auto ThreadsafeVecLock<T>::begin(this ThreadsafeVecLock& self) -> T* {
    return self.parent->ptr;
}

template <class T>
auto ThreadsafeVecLock<T>::end(this ThreadsafeVecLock& self) -> T* {
    return self.parent->ptr + self.parent->len;
}

template <class T>
auto ThreadsafeVecLock<T>::capacity(this ThreadsafeVecLock const& self)
    -> usize {
    return self.parent->cap;
}

template <class T>
auto ThreadsafeVecLock<T>::size(this ThreadsafeVecLock const& self) -> usize {
    return self.parent->len;
}

template <class T>
auto ThreadsafeVecLock<T>::erase(this ThreadsafeVecLock& self, T* begin, T* end)
    -> void {
    if (self.size() == 0 || begin == end) {
        return;
    }

    if (begin < self.parent->ptr ||
        begin >= self.parent->ptr + self.parent->len ||
        end < self.parent->ptr || end > self.parent->ptr + self.parent->len ||
        begin > end)
    {
        throw Panic("invalid erase operation");
    }

    auto const n_elements = (usize) (end - begin);
    auto delete_begin = begin;

    if (self.end() != end) {
        self.reserve(n_elements);

        memcpy(self.end(), begin, sizeof(T) * n_elements);
        memcpy(begin, end, sizeof(T) * (self.end() - end));
        delete_begin = self.end();
    }

    for (usize i = 0; i < n_elements; ++i) {
        delete_begin[i].~T();
    }

    self.parent->len -= n_elements;
}

template <class T>
auto ThreadsafeVecLock<T>::reserve_exact(
    this ThreadsafeVecLock& self, usize amount
) -> void {
    if (self.size() + amount <= self.capacity()) {
        return;
    }

    if (0 == self.capacity()) {
        self.parent->cap = amount;
        self.parent->ptr =
            (T*) std::malloc(sizeof(*self.parent->ptr) * self.parent->cap);
    } else if (self.capacity() - self.size() < amount) {
        self.parent->cap += amount;
        self.parent->ptr = (T*) std::realloc(
            self.parent->ptr, sizeof(*self.parent->ptr) * self.parent->cap
        );
    }
}

template <class T>
auto ThreadsafeVecLock<T>::reserve(this ThreadsafeVecLock& self, usize amount)
    -> void {
    return self.reserve_exact(std::max(
        amount,
        ThreadsafeVec<T>::next_capacity(self.capacity()) - self.capacity()
    ));
}

template <class T>
auto ThreadsafeVecLock<T>::push(this ThreadsafeVecLock& self, T value) -> void {
    self.reserve(1);
    
    new (self.parent->ptr + self.size()) T{std::move(value)};
    self.parent->len += 1;
}

template <class T>
auto ThreadsafeVecLock<T>::pop(this ThreadsafeVecLock& self) -> T {
    if (self.size() == 0) {
        throw Panic("cannot pop out of empty `ThreadsafeVec`");
    }

    self.parent->len -= 1;
    auto result = std::move(self[self.size()]);

    return result;
}

}  // namespace tmine
