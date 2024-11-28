#include "../collections.hpp"

namespace tmine {

StaticString::operator std::string_view(this StaticString const& self) {
    return std::visit(
        [](auto const& str) -> std::string_view {
            return std::string_view{str};
        },
        self.value
    );
}

}  // namespace tmine
