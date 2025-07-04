#pragma once

#include <type_traits>
#include <span>
#include <algorithm>
#include <atomic>
#include <shared_mutex>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <variant>
#include <type_traits>

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
    auto empty(this ThreadsafeVecLock const& self) -> bool;

    auto erase(this ThreadsafeVecLock& self, T* begin, T* end) -> void;
    auto insert(this ThreadsafeVecLock& self, T* pos, T value) -> void;

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

    for (usize i = 0; i < n_elements; ++i) {
        delete_begin[i].~T();
    }

    std::memmove(begin, end, sizeof(T) * (self.end() - end));

    self.parent->len -= n_elements;
}

template <class T>
auto ThreadsafeVecLock<T>::insert(this ThreadsafeVecLock& self, T* pos, T value)
    -> void {
    if (pos < self.begin() || pos > self.end()) {
        throw Panic("invalid insert");
    }

    auto const index = (usize) (pos - self.begin());

    if (index == self.size()) {
        self.push(std::move(value));
        return;
    }

    self.reserve(1);

    if (!self.empty()) {
        std::memmove(
            self.parent->ptr + index + 1, self.parent->ptr + index,
            sizeof(T) * (self.size() - index)
        );
    }

    new (self.parent->ptr + index) T{std::move(value)};

    self.parent->len += 1;
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

    auto result = std::move(self[self.size() - 1]);
    self.parent->len -= 1;

    return result;
}

template <class T>
auto ThreadsafeVecLock<T>::empty(this ThreadsafeVecLock const& self) -> bool {
    return self.size() == 0;
}

class StaticString {
public:
    inline StaticString(char const* c_string)
    : value{std::string_view{c_string}} {}

    /*implicit*/ inline StaticString(std::string_view value)
    : value{value} {}

    /*implicit*/ inline StaticString(std::string value)
    : value{std::move(value)} {}

    operator std::string_view(this StaticString const& self);

    inline auto operator<=>(
        this StaticString const& self, StaticString const& other
    ) {
        return std::string_view{self} <=> std::string_view{other};
    }

    inline auto operator==(
        this StaticString const& self, StaticString const& other
    ) -> bool {
        return std::string_view{self} == std::string_view{other};
    }

    inline auto operator!=(
        this StaticString const& self, StaticString const& other
    ) -> bool {
        return std::string_view{self} != std::string_view{other};
    }

private:
    std::variant<std::string, std::string_view> value;
};

}  // namespace tmine

template <>
struct std::hash<tmine::StaticString> {
    inline auto operator()(tmine::StaticString const& str) const noexcept
        -> tmine::usize {
        return std::hash<std::string_view>{}(std::string_view{str});
    }
};

namespace tmine {

template <class T>
struct MaybeUninitialized {
    alignas(T) std::array<std::byte, sizeof(T)> data;
};

template <usize N, class T>
    requires requires { N > 0; }
class SmallVec {
public:
    inline SmallVec()
    : len{0}
    , ptr{nullptr}
    , cap{0} {}

    ~SmallVec()
        requires std::is_trivially_destructible_v<T>
    {
        if (this->len > N) {
            free(this->ptr);
        }
    }

    ~SmallVec()
        requires(!std::is_trivially_destructible_v<T>)
    {
        if (this->len <= N) {
            for (usize i = 0; i < this->len; ++i) {
                reinterpret_cast<T*>(&this->values[i])->~T();
            }
        } else {
            for (usize i = 0; i < this->len; ++i) {
                this->ptr[i].~T();
            }

            free(this->ptr);
        }
    }

private:
    auto spill(this SmallVec& self, usize cap) -> void {
        if (cap <= N) {
            throw Panic("bad SmallVec spill");
        }

        auto const ptr = (T*) malloc(cap * sizeof(T));

        if (self.len > 0) {
            memcpy(ptr, self.values, self.len * sizeof(T));
        }

        self.ptr = ptr;
        self.cap = cap;
    }

    static auto next_capacity(usize cap) -> usize { return 3 * (cap + 1) / 2; }

public:
    auto push(this SmallVec& self, T value) -> void {
        if (self.len < N) {
            new (&self.values[self.len]) T{std::move(value)};
        } else if (self.len == N) {
            self.spill(self.len + 1);
            new (self.ptr + self.len) T{std::move(value)};
        } else {
            if (self.len == self.cap) {
                self.cap = SmallVec::next_capacity(self.cap);
                self.ptr = (T*) realloc(self.ptr, sizeof(T) * self.cap);
            }

            new (self.ptr + self.len) T{std::move(value)};
        }

        self.len += 1;
    }

    auto begin(this SmallVec& self) -> T* {
        if (self.len <= N) {
            return reinterpret_cast<T*>(&self.values[0]);
        } else {
            return self.ptr;
        }
    }

    auto end(this SmallVec& self) -> T* { return self.begin() + self.len; }

    auto size(this SmallVec const& self) -> usize { return self.len; }

    template <class Self>
    auto operator[](this Self&& self, usize index) {
        return std::forward_like<Self>(self.begin()[index]);
    }

    auto erase_from(this SmallVec& self, T* it) -> void {
        if (it < self.begin() || it >= self.end()) {
            return;
        }

        for (auto cur = it; cur != self.end(); ++cur) {
            cur->~T();
        }

        auto const was_on_stack_before = self.len > N;

        self.len -= self.end() - it;

        if (!was_on_stack_before && self.len <= N) {
            auto const ptr = self.ptr;

            memcpy(self.values, ptr, sizeof(T) * self.len);
            free(ptr);
        }
    }

    auto empty(this SmallVec const& self) -> bool { return self.len == 0; }

private:
    usize len;

    union {
        MaybeUninitialized<T> values[N];

        struct {
            T* ptr;
            usize cap;
        };
    };
};

}  // namespace tmine
