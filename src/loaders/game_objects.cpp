#include <cstring>
#include <ranges>

#include <rapidjson/document.h>
#include <fmt/format.h>

#include "../panic.hpp"
#include "../loaders.hpp"

namespace tmine {

namespace rj = rapidjson;
namespace vs = std::ranges::views;

using TextureMap = std::unordered_map<std::string, GameBlockTextureIdentifier>;

static auto parse_side_flags(char const* path, rj::Value const& src)
    -> Side::Value {
    if (!src.IsArray()) {
        throw Panic("`sides` property should be an array in '{}'", path);
    }

    auto result = Side::EMPTY;

    for (auto const& value : src.GetArray()) {
        if (!value.IsString()) {
            throw Panic(
                "all elements of the `sides` property should be strings in "
                "'{}'",
                path
            );
        }

        auto const value_string = value.GetString();

        if (0 == std::strcmp("+x", value_string)) {
            result |= Side::POS_X;
        } else if (0 == std::strcmp("-x", value_string)) {
            result |= Side::NEG_X;
        } else if (0 == std::strcmp("+y", value_string)) {
            result |= Side::POS_Y;
        } else if (0 == std::strcmp("-y", value_string)) {
            result |= Side::NEG_Y;
        } else if (0 == std::strcmp("+z", value_string)) {
            result |= Side::POS_Z;
        } else if (0 == std::strcmp("-z", value_string)) {
            result |= Side::NEG_Z;
        } else {
            throw Panic(
                "unknown value of `{}` in the element of `sides` property in "
                "'{}'",
                value_string, path
            );
        }
    }

    return result;
}

static auto parse_texture_property(
    char const* path, rj::Value const& src, TextureMap const& texture_data,
    RefMut<std::optional<TextureId>> top_texture_id,
    RefMut<std::optional<TextureId>> bottom_texture_id,
    RefMut<std::optional<TextureId>> left_texture_id,
    RefMut<std::optional<TextureId>> right_texture_id,
    RefMut<std::optional<TextureId>> front_texture_id,
    RefMut<std::optional<TextureId>> back_texture_id
) -> void {
    if (!src.IsString() && !src.IsObject()) {
        throw Panic(
            "property 'texture' should be a string or an object in "
            "'{}'",
            path
        );
    }

    if (src.IsString()) {
        auto const texture_name = src.GetString();

        if (!texture_data.contains(texture_name)) {
            throw Panic(
                "unknown texture name '{}' in '{}'", texture_name, path
            );
        }

        auto const id = texture_data.at(texture_name).id;

        *top_texture_id = id;
        *bottom_texture_id = id;
        *left_texture_id = id;
        *right_texture_id = id;
        *front_texture_id = id;
        *back_texture_id = id;
    } else if (src.IsObject()) {
        for (auto const& [side, texture] : src.GetObject()) {
            auto const side_name = side.GetString();

            if (!texture.IsString()) {
                throw Panic(
                    "invalid property in 'texture': all members "
                    "should be strings in '{}'",
                    path
                );
            }

            auto const texture_name = texture.GetString();

            if (!texture_data.contains(texture_name)) {
                throw Panic(
                    "unknown texture name '{}' in '{}'", texture_name, path
                );
            }

            auto const id = texture_data.at(texture_name).id;

            if (0 == std::strcmp("top", side_name)) {
                *top_texture_id = id;
            } else if (0 == std::strcmp("bottom", side_name)) {
                *bottom_texture_id = id;
            } else if (0 == std::strcmp("left", side_name)) {
                *left_texture_id = id;
            } else if (0 == std::strcmp("right", side_name)) {
                *right_texture_id = id;
            } else if (0 == std::strcmp("front", side_name)) {
                *front_texture_id = id;
            } else if (0 == std::strcmp("back", side_name)) {
                *back_texture_id = id;
            } else if (0 == std::strcmp("sides", side_name)) {
                *left_texture_id = id;
                *right_texture_id = id;
                *front_texture_id = id;
                *back_texture_id = id;
            } else {
                throw Panic("invalid size name '{}' in '{}'", side_name, path);
            }
        }
    }
}

static auto parse_block_description(
    char const* path, TextureMap const& texture_data, rj::Value const& desc
) -> std::pair<GameBlock, u32> {
    if (!desc.IsObject()) {
        throw Panic(
            "invalid array element in game blocks description '{}': should be "
            "an object",
            path
        );
    }

    auto name = std::optional<std::string>{};
    auto voxel_id = std::optional<VoxelId>{};
    auto is_translucent = false;
    auto is_extra_transparent = false;
    auto orientations = Side::ALL;
    auto top_texture_id = std::optional<TextureId>{};
    auto bottom_texture_id = std::optional<TextureId>{};
    auto left_texture_id = std::optional<TextureId>{};
    auto right_texture_id = std::optional<TextureId>{};
    auto front_texture_id = std::optional<TextureId>{};
    auto back_texture_id = std::optional<TextureId>{};

    for (auto const& [property, value] : desc.GetObject()) {
        auto const property_string = property.GetString();

        if (0 == std::strcmp("name", property_string)) {
            if (!value.IsString()) {
                throw Panic("property 'name' should be a string in '{}'", path);
            }

            name = value.GetString();
        } else if (0 == std::strcmp("id", property_string)) {
            if (!value.IsInt()) {
                throw Panic("property 'id' should be an integer in '{}'", path);
            }

            auto const stored_id = value.GetInt();

            if (stored_id < 0 || 0xFF < stored_id) {
                throw Panic("invalid block id '{}' in '{}'", stored_id, path);
            }

            voxel_id = stored_id;
        } else if (0 == std::strcmp("translucent", property_string)) {
            if (!value.IsBool()) {
                throw Panic(
                    "property 'translucent' should be a bool in '{}'", path
                );
            }

            is_translucent = value.GetBool();
        } else if (0 == std::strcmp("extra_transparency", property_string)) {
            if (!value.IsBool()) {
                throw Panic(
                    "property 'extra_transparent' should be a bool in '{}'",
                    path
                );
            }

            is_extra_transparent = value.GetBool();
        } else if (0 == std::strcmp("texture", property_string)) {
            parse_texture_property(
                path, value, texture_data, &top_texture_id, &bottom_texture_id,
                &left_texture_id, &right_texture_id, &front_texture_id,
                &back_texture_id
            );
        } else if (0 == std::strcmp("orientations", property_string)) {
            orientations = parse_side_flags(path, value);
        } else {
            throw Panic("unknown property '{}' in '{}'", property_string, path);
        }
    }

    if (!name.has_value()) {
        throw Panic("property 'name' has not been set in '{}'", path);
    }

    if (!voxel_id.has_value()) {
        throw Panic(
            "property 'id' has not been set for '{}' block in '{}'",
            name.value(), path
        );
    }

    if (!top_texture_id.has_value() || !bottom_texture_id.has_value() ||
        !left_texture_id.has_value() || !right_texture_id.has_value() ||
        !front_texture_id.has_value() || !back_texture_id.has_value())
    {
        throw Panic(
            "not all sides have a texture for '{}' block in '{}'", name.value(),
            path
        );
    }

    return std::make_pair(
        GameBlock{
            .name = std::move(name).value(),
            .texture_ids =
                {top_texture_id.value(), bottom_texture_id.value(),
                 left_texture_id.value(), right_texture_id.value(),
                 front_texture_id.value(), back_texture_id.value()},
            .voxel_id = voxel_id.value(),
            .meta = GameBlock::meta_of(
                is_translucent, is_extra_transparent, orientations
            ),
        },
        orientations
    );
}

auto load_game_blocks(char const* path, TextureMap const& texture_data)
    -> std::vector<std::array<GameBlock, 6>> {
    auto const file_content = read_to_string(path);

    auto document = rj::Document{};
    document.Parse(file_content.c_str());

    if (!document.IsArray()) {
        throw Panic(
            "invalid game blocks description '{}': should be an array", path
        );
    }

    auto result = std::vector<std::array<GameBlock, 6>>{};
    result.reserve(document.Size() + 1);
    result.emplace_back(std::array<GameBlock, 6>{GameBlock{
        .name = "air",
        .texture_ids = {0, 0, 0, 0, 0, 0},
        .voxel_id = 0,
        .meta = GameBlock::meta_of(true, true, 0),
    }});

    for (auto const& entry : document.GetArray()) {
        auto const [desc, sides] =
            parse_block_description(path, texture_data, entry);

        result.resize((usize) desc.voxel_id + 1);

        if (0 != (Side::POS_X & sides)) {
            result[(usize) desc.voxel_id][GameBlock::ORIENTATION_POS_X] = desc;
        }

        if (0 != (Side::NEG_X & sides)) {
            result[(usize) desc.voxel_id][GameBlock::ORIENTATION_NEG_X] = desc;
        }

        if (0 != (Side::POS_Y & sides)) {
            result[(usize) desc.voxel_id][GameBlock::ORIENTATION_POS_Y] = desc;
        }

        if (0 != (Side::NEG_Y & sides)) {
            result[(usize) desc.voxel_id][GameBlock::ORIENTATION_NEG_Y] = desc;
        }

        if (0 != (Side::POS_Z & sides)) {
            result[(usize) desc.voxel_id][GameBlock::ORIENTATION_POS_Z] = desc;
        }

        if (0 != (Side::NEG_Z & sides)) {
            result[(usize) desc.voxel_id][GameBlock::ORIENTATION_NEG_Z] = desc;
        }
    }

    return result;
}

auto load_game_block_textures(char const* path)
    -> std::unordered_map<std::string, GameBlockTextureIdentifier> {
    auto file_content = read_to_string(path);

    auto document = rj::Document{};
    document.Parse(file_content.c_str());

    if (!document.IsArray()) {
        throw Panic("'{}' should be an array", path);
    }

    auto result = std::unordered_map<std::string, GameBlockTextureIdentifier>{};
    result.reserve(document.Size() + 1);
    result.insert({"air", {.name = "air", .id = 0}});

    for (auto [i, entry] : document.GetArray() | vs::enumerate) {
        if (!entry.IsObject()) {
            throw Panic("array elements should be objects in '{}'", path);
        }

        auto name = std::optional<std::string>{};
        auto id = std::optional<TextureId>{};

        for (auto const& [key, value] : entry.GetObject()) {
            auto const key_string = key.GetString();

            if (0 == std::strcmp("name", key_string)) {
                if (!value.IsString()) {
                    throw Panic(
                        "invalid property 'name' in '{}': should be a string",
                        path
                    );
                }

                name = value.GetString();
            } else if (0 == std::strcmp("id", key_string)) {
                if (!value.IsInt()) {
                    throw Panic(
                        "invlid property 'id' in '{}': should be an integer",
                        path
                    );
                }

                auto const read_id = value.GetInt();

                if (read_id < 0 || 0xFF < read_id) {
                    throw Panic(
                        "too large texture id '{}' in '{}'", read_id, path
                    );
                }

                id = (TextureId) read_id;
            } else {
                throw Panic("unknown property '{}' in '{}'", key_string, path);
            }
        }

        if (!name.has_value()) {
            throw Panic("no property 'name' in '{}'", path);
        }

        if (!id.has_value()) {
            throw Panic(
                "no property 'id' for '{}' texture in '{}'", name.value(), path
            );
        }

        if (id.value() != i + 1) {
            throw Panic(
                "texture ids should be the same as their order in the list in "
                "'{}'",
                path
            );
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
