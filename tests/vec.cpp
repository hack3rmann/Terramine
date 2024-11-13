#include <thread>
#include <vector>
#include <fmt/ranges.h>

#include "collections.hpp"
#include "vec.hpp"
#include "assert.hpp"

namespace tmine_test {

using namespace tmine;

auto test_vec_append() -> void {
    auto vec = tmine::ThreadsafeVec<usize>{};

    auto append_vec = [&vec](usize start, usize end) {
        auto range = std::vector<usize>{};
        range.reserve(end - start);
        
        for (usize i = start; i < end; ++i) {
            range.push_back(i);
        }

        vec.append(range);
    };

    {
        auto thread1 = std::jthread(append_vec, 0, 10);
        auto thread2 = std::jthread(append_vec, 10, 20);
        auto thread3 = std::jthread(append_vec, 20, 30);
        auto thread4 = std::jthread(append_vec, 30, 40);
        auto thread5 = std::jthread(append_vec, 40, 50);
        auto thread6 = std::jthread(append_vec, 50, 60);
        auto thread7 = std::jthread(append_vec, 60, 70);
        auto thread8 = std::jthread(append_vec, 70, 80);
        auto thread9 = std::jthread(append_vec, 80, 90);
        auto thread10 = std::jthread(append_vec, 90, 100);
        auto thread11 = std::jthread(append_vec, 100, 110);
        auto thread12 = std::jthread(append_vec, 110, 120);
        auto thread13 = std::jthread(append_vec, 120, 130);
        auto thread14 = std::jthread(append_vec, 130, 140);
        auto thread15 = std::jthread(append_vec, 140, 150);
        auto thread16 = std::jthread(append_vec, 150, 160);
        auto thread17 = std::jthread(append_vec, 160, 170);
        auto thread18 = std::jthread(append_vec, 170, 180);
        auto thread19 = std::jthread(append_vec, 180, 190);
        auto thread20 = std::jthread(append_vec, 190, 200);
    }

    auto read = vec.read();

    tmine_assert_eq(read.as_span().size(), 200);
}

auto test_vec_append_single_thread() -> void {
    auto vec = ThreadsafeVec<usize>{};

    auto arr = std::array<usize, 10>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
    vec.append(arr);
}

}
