#include <glad/gl.h>
#include <spng.h>

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <iostream>

#include "../loaders.hpp"

namespace tmine {

    namespace fs = std::filesystem;

    auto load_png(fs::path image_path) -> std::optional<Image> {
        FILE* image_file;
        int result = 0;
        spng_ctx* ctx = NULL;

        image_file = std::fopen(image_path.c_str(), "rb");

        if (image_file == nullptr) {
            fprintf(
                stderr, "failed to open files '%s': %s\n", image_path.c_str(),
                std::strerror(errno)
            );
            return std::nullopt;
        }

        ctx = spng_ctx_new(0);

        if (ctx == nullptr) {
            return std::nullopt;
        }

        /* Ignore and don't calculate chunk CRC's */
        spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);

        /* Set memory usage limits for storing standard and unknown chunks,
           this is important when reading untrusted files! */
        size_t limit = 1024 * 1024 * 64;
        spng_set_chunk_limits(ctx, limit, limit);

        /* Set source PNG */
        spng_set_png_file(ctx, image_file); /* or _buffer(), _stream() */

        auto ihdr = (struct spng_ihdr){};
        if (spng_get_ihdr(ctx, &ihdr)) {
            return std::nullopt;
        }

        auto plte = (struct spng_plte){};

        result = spng_get_plte(ctx, &plte);
        if (result && result != SPNG_ECHUNKAVAIL) {
            return std::nullopt;
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
            return std::nullopt;
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
            return std::nullopt;
        }

        /* ihdr.height will always be non-zero if spng_get_ihdr() succeeds */
        image_width = image_size / ihdr.height;

        auto row_info = (struct spng_row_info){};

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
            std::cerr << "PNG error: " << spng_strerror(result) << std::endl;
            return std::nullopt;
        }

        if (result == SPNG_ECHUNKAVAIL) {
            return std::nullopt;
        }

        auto const width = image_width / 4;
        auto const height = ihdr.height;

        spng_ctx_free(ctx);
        std::fclose(image_file);

        return Image{image_bytes, width, height};
    }

}  // namespace tmine
