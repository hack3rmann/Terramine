#include <cstring>
#include <ranges>
#include <stdexcept>

#include <rapidjson/document.h>
#include <fmt/format.h>

#include "../loaders.hpp"

namespace tmine {

namespace rj = rapidjson;
namespace vs = std::ranges::views;

auto load_game_blocks(
    char const* path,
    std::unordered_map<std::string, GameBlockTextureIdentifier> const&
        texture_data
) -> std::vector<GameBlock> {
    auto const file_content = read_to_string(path);

    auto document = rj::Document{};
    document.Parse(file_content.c_str());

    if (!document.IsArray()) {
        throw std::runtime_error(fmt::format(
            "invalid game blocks description '{}': should be an array", path
        ));
    }

    auto result = std::vector<GameBlock>{};
    result.reserve(document.Size() + 1);
    result.emplace_back(GameBlock{
        .name = "air",
        .texture_ids = {0, 0, 0, 0, 0, 0},
        .voxel_id = 0,
        .meta = GameBlock::meta_of(true, 0),
    });

    for (auto [i, entry] : document.GetArray() | vs::enumerate) {
        if (!entry.IsObject()) {
            throw std::runtime_error(fmt::format(
                "invalid array element in game blocks description '{}': should "
                "be an object",
                path
            ));
        }

        auto name = std::optional<std::string>{};
        auto voxel_id = std::optional<VoxelId>{};
        auto variation = u8{0};
        auto is_translucent = false;
        auto top_texture_id = std::optional<TextureId>{};
        auto bottom_texture_id = std::optional<TextureId>{};
        auto left_texture_id = std::optional<TextureId>{};
        auto right_texture_id = std::optional<TextureId>{};
        auto front_texture_id = std::optional<TextureId>{};
        auto back_texture_id = std::optional<TextureId>{};

        for (auto const& [property, value] : entry.GetObject()) {
            auto const property_string = property.GetString();

            if (0 == std::strcmp("name", property_string)) {
                if (!value.IsString()) {
                    throw std::runtime_error(fmt::format(
                        "property 'name' should be a string in '{}'", path
                    ));
                }

                name = value.GetString();
            } else if (0 == std::strcmp("id", property_string)) {
                if (!value.IsInt()) {
                    throw std::runtime_error(fmt::format(
                        "property 'id' should be an integer in '{}'", path
                    ));
                }

                auto const stored_id = value.GetInt();

                if (stored_id < 0 || 0xFF < stored_id) {
                    throw std::runtime_error(fmt::format(
                        "invalid block id '{}' in '{}'", stored_id, path
                    ));
                }

                voxel_id = stored_id;
            } else if (0 == std::strcmp("variation", property_string)) {
                if (!value.IsInt()) {
                    throw std::runtime_error(fmt::format(
                        "property 'variation' should be an integer in '{}'",
                        path
                    ));
                }

                auto const stored_variation = value.GetInt();

                if (stored_variation < 0 || 127 < stored_variation) {
                    throw std::runtime_error(fmt::format(
                        "invalid variation '{}' in '{}'", stored_variation, path
                    ));
                }

                variation = stored_variation;
            } else if (0 == std::strcmp("translucent", property_string)) {
                if (!value.IsBool()) {
                    throw std::runtime_error(fmt::format(
                        "property 'translucent' should be a bool in '{}'", path
                    ));
                }

                is_translucent = value.GetBool();
            } else if (0 == std::strcmp("texture", property_string)) {
                if (value.IsString()) {
                    auto const texture_name = value.GetString();

                    if (!texture_data.contains(texture_name)) {
                        throw std::runtime_error(fmt::format(
                            "unknown texture name '{}' in '{}'", texture_name,
                            path
                        ));
                    }

                    auto const id = texture_data.at(texture_name).id;

                    top_texture_id = id;
                    bottom_texture_id = id;
                    left_texture_id = id;
                    right_texture_id = id;
                    front_texture_id = id;
                    back_texture_id = id;
                } else if (value.IsObject()) {
                    for (auto const& [side, texture] : value.GetObject()) {
                        auto const side_name = side.GetString();

                        if (!texture.IsString()) {
                            throw std::runtime_error(fmt::format(
                                "invalid property in 'texture': all members "
                                "should be strings in '{}'",
                                path
                            ));
                        }

                        auto const texture_name = texture.GetString();

                        if (!texture_data.contains(texture_name)) {
                            throw std::runtime_error(fmt::format(
                                "unknown texture name '{}' in '{}'",
                                texture_name, path
                            ));
                        }

                        auto const id = texture_data.at(texture_name).id;

                        if (0 == std::strcmp("top", side_name)) {
                            top_texture_id = id;
                        } else if (0 == std::strcmp("bottom", side_name)) {
                            bottom_texture_id = id;
                        } else if (0 == std::strcmp("left", side_name)) {
                            left_texture_id = id;
                        } else if (0 == std::strcmp("right", side_name)) {
                            right_texture_id = id;
                        } else if (0 == std::strcmp("front", side_name)) {
                            front_texture_id = id;
                        } else if (0 == std::strcmp("back", side_name)) {
                            back_texture_id = id;
                        } else if (0 == std::strcmp("sides", side_name)) {
                            left_texture_id = id;
                            right_texture_id = id;
                            front_texture_id = id;
                            back_texture_id = id;
                        } else {
                            throw std::runtime_error(fmt::format(
                                "invalid size name '{}' in '{}'", side_name,
                                path
                            ));
                        }
                    }
                } else {
                    throw std::runtime_error(fmt::format(
                        "property 'texture' should be a string or an object in "
                        "'{}'",
                        path
                    ));
                }
            } else {
                throw std::runtime_error(fmt::format(
                    "unknown property '{}' in '{}'", property_string, path
                ));
            }
        }

        if (!name.has_value()) {
            throw std::runtime_error(
                fmt::format("property 'name' has not been set in '{}'", path)
            );
        }

        if (!voxel_id.has_value()) {
            throw std::runtime_error(fmt::format(
                "property 'id' has not been set for '{}' block in '{}'",
                name.value(), path
            ));
        }

        if (voxel_id.value() != i + 1) {
            throw std::runtime_error(fmt::format(
                "voxel ids should be the same as their order in the list in "
                "'{}'",
                path
            ));
        }

        if (!top_texture_id.has_value() || !bottom_texture_id.has_value() ||
            !left_texture_id.has_value() || !right_texture_id.has_value() ||
            !front_texture_id.has_value() || !back_texture_id.has_value())
        {
            throw std::runtime_error(fmt::format(
                "not all sides have a texture for '{}' block in '{}'",
                name.value(), path
            ));
        }

        result.emplace_back(GameBlock{
            .name = std::move(name).value(),
            .texture_ids =
                {top_texture_id.value(), bottom_texture_id.value(),
                 left_texture_id.value(), right_texture_id.value(),
                 front_texture_id.value(), back_texture_id.value()},
            .voxel_id = voxel_id.value(),
            .meta = GameBlock::meta_of(is_translucent, variation),
        });
    }

    return result;
}

auto load_game_block_textures(char const* path)
    -> std::unordered_map<std::string, GameBlockTextureIdentifier> {
    auto file_content = read_to_string(path);

    auto document = rj::Document{};
    document.Parse(file_content.c_str());

    if (!document.IsArray()) {
        throw std::runtime_error(fmt::format("'{}' should be an array", path));
    }

    auto result = std::unordered_map<std::string, GameBlockTextureIdentifier>{};
    result.reserve(document.Size() + 1);
    result.insert({"air", {.name = "air", .id = 0}});

    for (auto [i, entry] : document.GetArray() | vs::enumerate) {
        if (!entry.IsObject()) {
            throw std::runtime_error(
                fmt::format("array elements should be objects in '{}'", path)
            );
        }

        auto name = std::optional<std::string>{};
        auto id = std::optional<TextureId>{};

        for (auto const& [key, value] : entry.GetObject()) {
            auto const key_string = key.GetString();

            if (0 == std::strcmp("name", key_string)) {
                if (!value.IsString()) {
                    throw std::runtime_error(fmt::format(
                        "invalid property 'name' in '{}': should be a string",
                        path
                    ));
                }

                name = value.GetString();
            } else if (0 == std::strcmp("id", key_string)) {
                if (!value.IsInt()) {
                    throw std::runtime_error(fmt::format(
                        "invlid property 'id' in '{}': should be an integer",
                        path
                    ));
                }

                auto const read_id = value.GetInt();

                if (read_id < 0 || 0xFF < read_id) {
                    throw std::runtime_error(fmt::format(
                        "too large texture id '{}' in '{}'", read_id, path
                    ));
                }

                id = (TextureId) read_id;
            } else {
                throw std::runtime_error(fmt::format(
                    "unknown property '{}' in '{}'", key_string, path
                ));
            }
        }

        if (!name.has_value()) {
            throw std::runtime_error(
                fmt::format("no property 'name' in '{}'", path)
            );
        }

        if (!id.has_value()) {
            throw std::runtime_error(fmt::format(
                "no property 'id' for '{}' texture in '{}'", name.value(), path
            ));
        }

        if (id.value() != i + 1) {
            throw std::runtime_error(fmt::format(
                "texture ids should be the same as their order in the list in '{}'",
                path
            ));
        }

        auto key = std::string{name.value()};

        result.insert(
            {std::move(key),
             GameBlockTextureIdentifier{
                 .name = std::move(name).value(), .id = id.value()
             }}
        );
    }

    return result;
}

auto load_game_blocks_data(
    char const* game_blocks_path, char const* game_block_textures_path
) -> GameBlocksData {
    auto textures = load_game_block_textures(game_block_textures_path);
    auto blocks = load_game_blocks(game_blocks_path, textures);

    auto textures_inline =
        std::vector<GameBlockTextureIdentifier>(textures.size());

    for (auto&& [name, data] : std::move(textures)) {
        textures_inline[data.id] = std::move(data);
    }

    return GameBlocksData{
        .blocks = std::move(blocks), .textures = std::move(textures_inline)
    };
}

}  // namespace tmine
