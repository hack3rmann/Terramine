#pragma once

#include <cstdint>

namespace tmine {

using u8 = std::uint8_t;
using i8 = std::int8_t;
using u16 = std::uint16_t;
using i16 = std::int16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i64 = std::int64_t;
using usize = std::size_t;
using isize = std::ptrdiff_t;
using f32 = float;
using f64 = double;

/// Mutable reference to `T`. Non-null mutable reference replacement in function
/// arguments.
/// The main idea is to notify the caller of some function that that particular
/// parameter expected to be mutated.
///
/// # Guarantees
///
/// - pointer in non-null
/// - pointee should be valid already constructed object
///
/// # Motivation
///
/// In C++ functions that take mutable reference are indistinguishable from
/// functions that take only const reference.
///
/// ```cpp
/// int a = 1, b = 2;
///
/// // We can't be sure that the function will change the variable or not
/// foo(a, b);
/// 
/// // function can be defined like that
/// auto foo(int& a, int const& b) -> void {
///     a = b;
/// }
///
/// // So I have introduced a better approach:
/// // All functions that would mutate a variable should take that variable through `RefMut`:
/// auto foo(RefMut<int> a, int const& b) -> void {
///     *a = b;
/// }
///
/// // Now we 100% sure that the function would change variable `a`:
/// foo(&a, b);
/// ```
///
/// # Example
///
/// ```cpp
/// // Function that would mutate value
/// auto foo(RefMut<i32> value) -> void {
///     *value = 42;
/// }
///
/// auto bar() -> void {
///     int value = 90;
///
///     // using `&` to signal about potential change of `value` variable
///     foo(&value);
/// }
/// ```
template <class T>
using RefMut = T*;

}  // namespace tmine
