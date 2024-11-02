#include <glad/gl.h>
#include <spng.h>

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <stdexcept>
#include <fmt/format.h>

#include "../loaders.hpp"

namespace tmine {

auto load_png(char const* path) -> Image {
    FILE* image_file;
    int result = 0;
    spng_ctx* ctx = NULL;

    image_file = std::fopen(path, "rb");

    if (image_file == nullptr) {
        throw std::runtime_error(fmt::format(
            "failed to open file '{}': {}", path, std::strerror(errno)
        ));
    }

    ctx = spng_ctx_new(0);

    if (ctx == nullptr) {
        throw std::runtime_error("failed to create spng context");
    }

    /* Ignore and don't calculate chunk CRC's */
    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);

    /* Set memory usage limits for storing standard and unknown chunks,
       this is important when reading untrusted files! */
    size_t limit = 1024 * 1024 * 64;
    spng_set_chunk_limits(ctx, limit, limit);

    /* Set source PNG */
    spng_set_png_file(ctx, image_file); /* or _buffer(), _stream() */

    auto ihdr = (struct spng_ihdr) {};
    if (spng_get_ihdr(ctx, &ihdr)) {
        throw std::runtime_error("failed to get ihdr");
    }

    auto plte = (struct spng_plte) {};

    result = spng_get_plte(ctx, &plte);
    if (result && result != SPNG_ECHUNKAVAIL) {
        throw std::runtime_error("failed to get plte");
    }

    size_t image_size, image_width;

    /* Output format, does not depend on source PNG format except for
       SPNG_FMT_PNG, which is the PNG's format in host-endian or
       big-endian for SPNG_FMT_RAW.
       Note that for these two formats <8-bit images are left byte-packed */
    int fmt = SPNG_FMT_PNG;

    /* With SPNG_FMT_PNG indexed color images are output as palette indices,
       pick another format to expand them. */
    if (ihdr.color_type == SPNG_COLOR_TYPE_INDEXED) {
        fmt = SPNG_FMT_RGB8;
    }

    if (spng_decoded_image_size(ctx, fmt, &image_size)) {
        throw std::runtime_error("failed to decode image size");
    }

    auto image_bytes = std::vector<u8>(image_size, 0);

    /* Decode the image in one go */
    /* ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
    if(ret)
    {
        printf("spng_decode_image() error: %s\n", spng_strerror(ret));
        goto error;
    }*/

    /* Alternatively you can decode the image progressively,
       this requires an initialization step. */
    // char* out = new char[image_size];
    if (spng_decode_image(ctx, nullptr, 0, fmt, SPNG_DECODE_PROGRESSIVE)) {
        throw std::runtime_error("failed to decode image");
    }

    /* ihdr.height will always be non-zero if spng_get_ihdr() succeeds */
    image_width = image_size / ihdr.height;

    auto row_info = (struct spng_row_info) {};

    do {
        result = spng_get_row_info(ctx, &row_info);
        if (result) {
            break;
        }

        result = spng_decode_row(
            ctx, image_bytes.data() + row_info.row_num * image_width,
            image_width
        );
    } while (!result);

    if (result != SPNG_EOI) {
        throw std::runtime_error(
            fmt::format("spng failed: {}", spng_strerror(result))
        );
    }

    if (result == SPNG_ECHUNKAVAIL) {
        throw std::runtime_error("no chunk available");
    }

    auto const width = image_width / 4;
    auto const height = ihdr.height;

    spng_ctx_free(ctx);
    std::fclose(image_file);

    return Image{
        .data = std::move(image_bytes), .width = width, .height = height
    };
}

}  // namespace tmine
