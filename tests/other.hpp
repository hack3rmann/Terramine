#include <memory>
#include "panic.hpp"

namespace tmine_test {

using namespace tmine;

inline auto test_dynamic_cast_if_init() -> void {
    struct Base1 {
        virtual ~Base1() = default;
    };

    struct Base2 {
        virtual ~Base2() = default;
    };

    struct DerivedFrom1 : Base1 {};

    struct DerivedFrom2 : Base2 {};

    auto upcasted_to_base1 =
        std::unique_ptr<Base1>{std::make_unique<DerivedFrom1>()};

    auto upcasted_to_base2 =
        std::unique_ptr<Base2>{std::make_unique<DerivedFrom2>()};

    if ([[maybe_unused]] auto derived_from1 =
            dynamic_cast<DerivedFrom1*>(upcasted_to_base1.get()))
    {
    } else {
        Panic("invalid cast");
    }

    if ([[maybe_unused]] auto derived_from2 =
            dynamic_cast<DerivedFrom2*>(upcasted_to_base2.get()))
    {
    } else {
        Panic("invalid cast");
    }

    if ([[maybe_unused]] auto derived_from2 =
            dynamic_cast<DerivedFrom2*>(upcasted_to_base1.get()))
    {
        Panic("invalid cast");
    }

    if ([[maybe_unused]] auto derived_from1 =
            dynamic_cast<DerivedFrom1*>(upcasted_to_base2.get()))
    {
        Panic("invalid cast");
    }
}

}  // namespace tmine_test
