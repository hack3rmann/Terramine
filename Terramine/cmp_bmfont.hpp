/* cmp_bmfont - v0.2 - public domain BM Font loader
   no warranty implied; use at your own risk
This is a single file library, in the spirit of (https://github.com/nothings/stb), including a
public domain license (see below).
You must include the implementation at least once. To include the implementation in one of your cpp
files, use the following:
    #define CMP_BMFONT_INCLUDE
    #include "cmp_bmfont.hpp"
API:
To load a BMFont file (in text form), use something like the following:
    cmp::BMFont *font = cmp::bmfont_parse_file("path_to_your_file");
    if (!font) {
        fprintf(stderr, "Failed to load font file. Reason: %s", cmp::bmfont_get_error_string());
        // Other error handling goes here, including a return, exit, etc.
    }
    // Use font file
    cmp::bmfont_free(font);
CHANGELOG
    v0.2 11/20/2016 - Remove use of C++ limits header
    v0.1 11/19/2016 - Initial revision
LICENSE
This software is dual-licensed to the public domain and under the following
license: you are granted a perpetual, irrevocable license to copy, modify,
publish, and distribute this file as you see fit.
*/

#ifndef CMP_BMFONT_INCLUDE
#define CMP_BMFONT_INCLUDE

#include <stdint.h>

namespace cmp {

struct BMFont {
    struct Char {
        uint32_t id;
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        uint16_t x_offset;
        uint16_t y_offset;
        uint16_t x_advance;
        uint8_t  page;
        uint8_t  channel;
    };

    struct Kerning {
        uint32_t first;
        uint32_t second;
        int16_t  amount;
        uint8_t _padding[2];
    };

    char *    font_name;
    char **   page_names;
    Char *    chars;
    Kerning * kernings;

    int16_t  font_size;
    uint16_t line_height;
    uint16_t base;
    uint16_t scale_w;
    uint16_t scale_h;
    uint16_t num_pages;
    uint16_t num_chars;
    uint16_t num_kernings;

    uint8_t  alpha_channel;
    uint8_t  red_channel;
    uint8_t  green_channel;
    uint8_t  blue_channel;
    uint8_t  _padding[4];
};

// Loads a BMFont from the specified file or returns nullptr if there is an error. Use
// bmfont_get_error_string() to get the error.
BMFont *bmfont_parse_file(const char *filename);
void  bmfont_free(BMFont *font);
const char *bmfont_get_error_string();

} // namespace cmp

#endif // ifndef CMP_BMFONT_INCLUDE


#if defined(CMP_BMFONT_IMPLEMENTATION) || defined(CMP_BMFONT_SELF_TEST)
#undef CMP_BMFONT_IMPLEMENTATION

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace cmp {

#define CMP_BMFONT__CONCAT2(x, y) x##y
#define CMP_BMFONT__CONCAT(x, y) CMP_BMFONT__CONCAT2(x, y)

template <typename F>
struct BMFont__Defer
{
    F f;

    BMFont__Defer(F f)
        : f(f) {}

    ~BMFont__Defer() { f(); }
};

struct BMFont__DeferZero {};

template <typename F>
BMFont__Defer<F> operator+(BMFont__DeferZero, F &&f) {
    return BMFont__Defer<F>(f);
}

#define CMP_BMFONT__DEFER auto CMP_BMFONT__CONCAT(bmfont__defer, __LINE__) = BMFont__DeferZero() + [&]()


const size_t BMFONT_MAX_TOKEN_LENGTH = 1024;

static char bmfont__error[BMFONT_MAX_TOKEN_LENGTH * 2 + 1024];

static constexpr unsigned BMFONT__PARSER_OK  = 0x0001;
static constexpr unsigned BMFONT__PARSER_EOF = 0x0002;

struct BMFont__Parser {
    FILE *   file;
    char     next_token[BMFONT_MAX_TOKEN_LENGTH];
    int      start_line, start_col;
    int      curr_line, curr_col;
    unsigned flags;
    int      next_char;
};

void *bmfont__calloc(int count, size_t size)
{
    void *mem = calloc(count, size);
    if (!mem)
    {
        strcpy(bmfont__error, "Out of memory.");
    }
    return mem;
}

bool bmfont__parser_ok(BMFont__Parser *parser) {
    return parser->flags & BMFONT__PARSER_OK;
}

bool bmfont__parser_ready(BMFont__Parser *parser) {
    return bmfont__parser_ok(parser) && !(parser->flags & BMFONT__PARSER_EOF);
}

void bmfont__set_parser_error(BMFont__Parser *parser, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(bmfont__error, sizeof(bmfont__error), fmt, args);
    parser->flags &= ~BMFONT__PARSER_OK;
    va_end(args);
}

// TODO: handle '=' between quotes
bool bmfont__load_next_token(BMFont__Parser *parser) {
    assert(bmfont__parser_ok(parser));
    if (bmfont__parser_ready(parser)) {
        // Consume any whitespace
        while (parser->next_char == ' ' || parser->next_char == '\r') {
            if (parser->next_char == EOF) break;
            if (parser->next_char == ' ') parser->curr_col++;
            if (parser->next_char == '\n') {
                parser->curr_col = 1;
                parser->curr_line++;
            }
            parser->next_char = fgetc(parser->file);
        }

        parser->start_col = parser->curr_col;
        parser->start_line = parser->curr_line;

        if (parser->next_char == EOF) {
            parser->flags |= BMFONT__PARSER_EOF;
            return true;
        }

        if (parser->next_char == '\n') {
            parser->curr_col = 1;
            parser->curr_line++;
            strcpy(parser->next_token, "\n");
            parser->next_char = fgetc(parser->file);
            return true;
        }

        if (parser->next_char == '=') {
            parser->curr_col++;
            strcpy(parser->next_token, "=");
            parser->next_char = fgetc(parser->file);
            return true;
        }

        int len = 0;
        do {
            parser->curr_col++;
            parser->next_token[len++] = (char)parser->next_char;
        } while ((parser->next_char = fgetc(parser->file)) != EOF && parser->next_char != '=' &&
                 parser->next_char != ' ' && parser->next_char != '\r' &&
                 parser->next_char != '\n' && len < BMFONT_MAX_TOKEN_LENGTH);

        if (len == BMFONT_MAX_TOKEN_LENGTH) {
            bmfont__set_parser_error(parser,
                                     "Token length is too large to parse (line %d, col %d).",
                                     parser->start_line,
                                     parser->start_col);
            return false;
        }

        parser->next_token[len] = '\0';
    }

    return bmfont__parser_ok(parser);
}

bool bmfont__expect_more_tokens(BMFont__Parser *parser) {
    if (parser->flags & BMFONT__PARSER_EOF) {
        bmfont__set_parser_error(parser,
                                 "Unexpectedly reached EOF (line %d, col %d). Expected token.",
                                 parser->start_line,
                                 parser->start_col);
        return false;
    }
    return true;
}

bool bmfont__match_token_and_advance(BMFont__Parser *parser, const char *token) {
    if (!bmfont__expect_more_tokens(parser) || !bmfont__parser_ok(parser) ||
        strcmp(parser->next_token, token)) {
        return false;
    }
    return bmfont__load_next_token(parser);
}

bool bmfont__expect_token_and_advance(BMFont__Parser *parser, const char *token) {
    if (!bmfont__match_token_and_advance(parser, token)) {
        if (!(parser->flags & BMFONT__PARSER_EOF)) {
            bmfont__set_parser_error(parser,
                                     "Unexpected token (line %d, col %d): %s. Expected token: %s",
                                     parser->start_line,
                                     parser->start_col,
                                     parser->next_token,
                                     token);
        }
        return false;
    }

    return true;
}

bool bmfont__match_key_and_advance_to_value(BMFont__Parser *parser, const char *token) {
    return !strcmp(parser->next_token, token) &&
        bmfont__load_next_token(parser) &&
        bmfont__match_token_and_advance(parser, "=");
}

bool bmfont__get_token_as_long(BMFont__Parser *parser, int64_t *dest) {
    if (!bmfont__expect_more_tokens(parser)) return false;
    char *end_ptr;
    int64_t long_value = strtol(parser->next_token, &end_ptr, 0);
    if (*end_ptr != '\0') {
        bmfont__set_parser_error(parser,
                                 "Expected an integer value (line %d, col %d). Got: %s",
                                 parser->start_line,
                                 parser->start_col,
                                 parser->next_token);
        return false;
    }

    *dest = long_value;
    return true;
}

bool bmfont__do_get_token_as_int_and_advance(BMFont__Parser *parser, int64_t min, int64_t max, int64_t *dest) {
    if (!bmfont__expect_more_tokens(parser)) return false;
    char *end_ptr;
    int64_t long_value = strtol(parser->next_token, &end_ptr, 0);
    if (*end_ptr != '\0') {
        bmfont__set_parser_error(parser,
                                 "Expected an integer value (line %d, col %d). Got: %s",
                                 parser->start_line,
                                 parser->start_col,
                                 parser->next_token);
        return false;
    }

    if (long_value < min || long_value > max) {
        bmfont__set_parser_error(parser,
                                 "Integer value out of range (line %d, col %d). Got: %ld",
                                 parser->start_line,
                                 parser->start_col,
                                 long_value);
        return false;
    }

    *dest = long_value;
    return bmfont__load_next_token(parser);
}

bool bmfont__get_token_as_int_and_advance(BMFont__Parser *parser, int16_t *dest) {
    int64_t long_value;
    if (bmfont__do_get_token_as_int_and_advance(parser, -32768, 32767, &long_value)) {
        *dest = (int16_t)long_value;
        return true;
    }
    return false;
}

bool bmfont__get_token_as_int_and_advance(BMFont__Parser *parser, uint16_t *dest) {
    int64_t long_value;
    if (bmfont__do_get_token_as_int_and_advance(parser, 0, 65535, &long_value)) {
        *dest = (int16_t)long_value;
        return true;
    }
    return false;
}

bool bmfont__get_token_as_int_and_advance(BMFont__Parser *parser, uint32_t *dest) {
    int64_t long_value;
    if (bmfont__do_get_token_as_int_and_advance(parser, 0, 4294967295, &long_value)) {
        *dest = (int16_t)long_value;
        return true;
    }
    return false;
}

bool bmfont__copy_token_and_advance(BMFont__Parser *parser, char **dest) {
    if (!bmfont__expect_more_tokens(parser)) return false;
    *dest = strdup(parser->next_token);
    return bmfont__load_next_token(parser);
}

bool bmfont__copy_quoted_token_and_advance(BMFont__Parser *parser, char **dest) {
    if (!bmfont__expect_more_tokens(parser)) return false;

    size_t len = strlen(parser->next_token);
    if (len < 2 || parser->next_token[0] != '"' || parser->next_token[len - 1] != '"') {
        bmfont__set_parser_error(parser,
                                 "Expected quoted string (line %d, col %d). Got: %s",
                                 parser->start_line,
                                 parser->start_col,
                                 parser->next_token);
        return false;
    }

    *dest = (char *)bmfont__calloc((int)len - 2, sizeof(char *));
    if (!dest) {
        parser->flags &= ~BMFONT__PARSER_OK;
        return false;
    }

    strncpy(*dest, parser->next_token + 1, len - 2);
    return bmfont__load_next_token(parser);
}

bool bmfont__parse_info(BMFont__Parser *parser, BMFont *font) {
    if (!bmfont__expect_token_and_advance(parser, "info")) return false;

    while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
        if (bmfont__match_key_and_advance_to_value(parser, "face")) {
            bmfont__copy_token_and_advance(parser, &font->font_name);
        } else if (bmfont__match_key_and_advance_to_value(parser, "size")) {
            bmfont__get_token_as_int_and_advance(parser, &font->font_size);
        } else {
            bmfont__match_token_and_advance(parser, "=");
            bmfont__load_next_token(parser);
        }
    }

    return bmfont__parser_ok(parser);
}

bool bmfont__parse_common(BMFont__Parser *parser, BMFont *font) {
    if (!bmfont__expect_token_and_advance(parser, "common")) return false;

    while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
        if (bmfont__match_key_and_advance_to_value(parser, "lineHeight")) {
            bmfont__get_token_as_int_and_advance(parser, &font->line_height);
        } else if (bmfont__match_key_and_advance_to_value(parser, "base")) {
            bmfont__get_token_as_int_and_advance(parser, &font->base);
        } else if (bmfont__match_key_and_advance_to_value(parser, "scaleW")) {
            bmfont__get_token_as_int_and_advance(parser, &font->scale_w);
        } else if (bmfont__match_key_and_advance_to_value(parser, "scaleH")) {
            bmfont__get_token_as_int_and_advance(parser, &font->scale_h);
        } else if (bmfont__match_key_and_advance_to_value(parser, "pages")) {
            bmfont__get_token_as_int_and_advance(parser, &font->num_pages);
            font->page_names = (char **)bmfont__calloc(font->num_pages, sizeof(char *));
            if (!font->page_names) parser->flags &= ~BMFONT__PARSER_OK;
        } else {
            bmfont__match_token_and_advance(parser, "=");
            bmfont__load_next_token(parser);
        }
    }

    return bmfont__parser_ok(parser);
}

bool bmfont__parse_pages(BMFont__Parser *parser, BMFont *font) {
    int i;
    for (i = 0; i < font->num_pages && bmfont__match_token_and_advance(parser, "page"); ++i) {
        uint32_t id = 0;
        char *filename = nullptr;

        while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
            if (bmfont__match_key_and_advance_to_value(parser, "id")) {
                bmfont__get_token_as_int_and_advance(parser, &id);
            } else if (bmfont__match_key_and_advance_to_value(parser, "file")) {
                bmfont__copy_quoted_token_and_advance(parser, &filename);
            } else {
                bmfont__match_token_and_advance(parser, "=");
                bmfont__load_next_token(parser);
            }
        }

        if (filename == nullptr) {
            bmfont__set_parser_error(parser,
                                     "Page tag missing filename (line %d)",
                                     parser->start_line);

            if (filename) free(filename);
            return false;
        }

        font->page_names[id] = filename;
    }

    if (i != font->num_pages) {
        bmfont__set_parser_error(parser,
                                 "Fewer pages than specified in file. Expected: %d, actual: "
                                 "%d",
                                 font->num_pages,
                                 i);
    }

    return bmfont__parser_ok(parser);
}

bool bmfont__parse_chars(BMFont__Parser *parser, BMFont *font) {
    if (!bmfont__expect_token_and_advance(parser, "chars")) return false;

    while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
        if (bmfont__match_key_and_advance_to_value(parser, "count")) {
            if (bmfont__get_token_as_int_and_advance(parser, &font->num_chars)) {
                font->chars = (BMFont::Char *)bmfont__calloc(font->num_chars, sizeof(BMFont::Char));
                if (!font->chars) parser->flags &= ~BMFONT__PARSER_OK;
            }
        } else {
            bmfont__match_token_and_advance(parser, "=");
            bmfont__load_next_token(parser);
        }
    }

    int i;
    for (i = 0; i < font->num_chars && bmfont__match_token_and_advance(parser, "char"); ++i) {
        BMFont::Char *ch = &font->chars[i];
        while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
            if (bmfont__match_key_and_advance_to_value(parser, "id")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->id);
            } else if (bmfont__match_key_and_advance_to_value(parser, "x")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->x);
            } else if (bmfont__match_key_and_advance_to_value(parser, "y")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->y);
            } else if (bmfont__match_key_and_advance_to_value(parser, "width")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->width);
            } else if (bmfont__match_key_and_advance_to_value(parser, "height")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->height);
            } else if (bmfont__match_key_and_advance_to_value(parser, "xoffset")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->x_offset);
            } else if (bmfont__match_key_and_advance_to_value(parser, "yoffset")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->y_offset);
            } else if (bmfont__match_key_and_advance_to_value(parser, "xadvance")) {
                bmfont__get_token_as_int_and_advance(parser, &ch->x_advance);
            } else {
                bmfont__match_token_and_advance(parser, "=");
                bmfont__load_next_token(parser);
            }
        }
    }

    if (i != font->num_chars) {
        bmfont__set_parser_error(parser,
                                 "Fewer chars than specified in file. Expected: %d, actual: %d",
                                 font->num_chars,
                                 i);
    }

    return bmfont__parser_ok(parser);
}

bool bmfont__parse_kernings(BMFont__Parser *parser, BMFont *font) {
    // We treat kerning as optional
    if (!bmfont__parser_ready(parser)) {
        font->kernings = (BMFont::Kerning *)bmfont__calloc(0, sizeof(BMFont::Kerning));
        return false;
    }

    bmfont__expect_token_and_advance(parser, "kernings");

    while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
        if (bmfont__match_key_and_advance_to_value(parser, "count")) {
            if (bmfont__get_token_as_int_and_advance(parser, &font->num_kernings)) {
                font->kernings = (BMFont::Kerning *)bmfont__calloc(font->num_kernings, sizeof(BMFont::Kerning));
                if (!font->kernings) parser->flags &= ~BMFONT__PARSER_OK;
            }
        } else {
            bmfont__match_token_and_advance(parser, "=");
            bmfont__load_next_token(parser);
        }
    }

    int i;
    for (i = 0; i < font->num_kernings && bmfont__match_token_and_advance(parser, "kerning"); ++i) {
        BMFont::Kerning *kerning = &font->kernings[i];
        while (bmfont__parser_ready(parser) && !bmfont__match_token_and_advance(parser, "\n")) {
            if (bmfont__match_key_and_advance_to_value(parser, "first")) {
                bmfont__get_token_as_int_and_advance(parser, &kerning->first);
            } else if (bmfont__match_key_and_advance_to_value(parser, "second")) {
                bmfont__get_token_as_int_and_advance(parser, &kerning->second);
            } else if (bmfont__match_key_and_advance_to_value(parser, "amount")) {
                bmfont__get_token_as_int_and_advance(parser, &kerning->amount);
            } else {
                bmfont__match_token_and_advance(parser, "=");
                bmfont__load_next_token(parser);
            }
        }

        ++kerning;
    }

    if (i != font->num_kernings) {
        bmfont__set_parser_error(parser,
                                 "Fewer kernings than specified in file. Expected: %d, actual: %d",
                                 font->num_kernings,
                                 i);
    }

    return bmfont__parser_ok(parser);
}

BMFont *bmfont_parse_file(const char *filename)
{
    strcpy(bmfont__error, "Success");

    FILE *file = fopen(filename, "rb");
    if (!file) {
        snprintf(bmfont__error,
                 sizeof(bmfont__error),
                 "Couldn't open file: %s. Error: %s",
                 filename,
                 strerror(errno));
        return nullptr;
    }
    CMP_BMFONT__DEFER { fclose(file); };

    BMFont__Parser parser = {};
    parser.file = file;
    parser.curr_line = 1;
    parser.curr_col = 1;
    parser.next_char = fgetc(file);
    parser.flags = BMFONT__PARSER_OK;
    bmfont__load_next_token(&parser);

    BMFont *font = (BMFont *)bmfont__calloc(1, sizeof(BMFont));
    if (!font) return nullptr;

    bmfont__parse_info(&parser, font) && bmfont__parse_common(&parser, font) &&
        bmfont__parse_pages(&parser, font) && bmfont__parse_chars(&parser, font) &&
        bmfont__parse_kernings(&parser, font);

    if (!bmfont__parser_ok(&parser)) {
        bmfont_free(font);
        return nullptr;
    }

    if (!(parser.flags & BMFONT__PARSER_EOF)) {
        bmfont__set_parser_error(&parser,
                                 "Expected EOF (line %d, col %d). Got: %s",
                                 parser.start_line,
                                 parser.start_col,
                                 parser.next_token);
        bmfont_free(font);
        return nullptr;
    }

    return font;
}

void bmfont_free(BMFont *font) {
    if (font->font_name) free(font->font_name);
    if (font->page_names) {
        for (int i = 0; i < font->num_pages; ++i) {
            free(font->page_names[i]);
        }
        free(font->page_names);
    }
    if (font->chars) free(font->chars);
    free(font);
}

const char *bmfont_get_error_string()
{
    return bmfont__error;
}

} // namespace cmp

#endif // ifdef CMP_BMFONT_IMPLEMENTATION


#ifdef CMP_BMFONT_SELF_TEST

#include <stdio.h>

using namespace cmp;

#define ASSERT_NULLPTR(actual)                                                                     \
    if (actual != nullptr) {                                                                       \
        printf("%s(%d): failed: expected nullptr, got: %p\n", __FILE__, __LINE__, actual);         \
        fail = true;                                                                               \
    }


#define ASSERT_STR_EQ(expected, actual)                                                            \
    if (actual == nullptr || strcmp(expected, actual)) {                                           \
        printf("%s(%d): failed: expected: '%s', got: '%s'\n",                                      \
               __FILE__,                                                                           \
               __LINE__,                                                                           \
               expected,                                                                           \
               actual);                                                                            \
        fail = true;                                                    \
    }

#define ASSERT_INT_EQ(expected, actual)                                                            \
    if (expected != actual) {                                                                      \
        printf("%s(%d): failed: expected: %d, got: %d\n", __FILE__, __LINE__, expected, actual);   \
        fail = true;                                                                               \
    }

static bool fail = false;

int main() {
    BMFont *font = bmfont_parse_file("test_data/valid.fnt");
    if (!font) {
        printf("%s: failed: %s\n", __FILE__, bmfont_get_error_string());
        exit(1);
    }

    ASSERT_STR_EQ("valid", font->font_name);
    ASSERT_INT_EQ(8, font->font_size);
    ASSERT_INT_EQ(8, font->line_height);
    ASSERT_INT_EQ(7, font->base);
    ASSERT_INT_EQ(128, font->scale_w);
    ASSERT_INT_EQ(512, font->scale_h);
    ASSERT_INT_EQ(1, font->num_pages);
    ASSERT_STR_EQ("valid.png", font->page_names[0]);
    ASSERT_INT_EQ(3, font->num_chars);
    ASSERT_INT_EQ(33, font->chars[0].id);
    ASSERT_INT_EQ(2, font->chars[0].x);
    ASSERT_INT_EQ(3, font->chars[0].y);
    ASSERT_INT_EQ(6, font->chars[0].width);
    ASSERT_INT_EQ(7, font->chars[0].height);
    ASSERT_INT_EQ(0, font->chars[0].x_offset);
    ASSERT_INT_EQ(1, font->chars[0].y_offset);
    ASSERT_INT_EQ(8, font->chars[0].x_advance);
    ASSERT_INT_EQ(0, font->chars[0].page);
    ASSERT_INT_EQ(2, font->num_kernings);
    ASSERT_INT_EQ(33, font->kernings[0].first);
    ASSERT_INT_EQ(34, font->kernings[0].second);
    ASSERT_INT_EQ(-4, font->kernings[0].amount);

    bmfont_free(font);


    font = bmfont_parse_file("test_data/valid_no_kernings.fnt");
    if (!font) {
        printf("%s: failed: %s\n", __FILE__, bmfont_get_error_string());
        exit(1);
    }

    ASSERT_STR_EQ("valid", font->font_name);
    ASSERT_INT_EQ(1, font->num_pages);
    ASSERT_STR_EQ("valid.png", font->page_names[0]);
    ASSERT_INT_EQ(3, font->num_chars);
    ASSERT_INT_EQ(0, font->num_kernings);

    bmfont_free(font);


    font = bmfont_parse_file("test_data/does_not_exist");
    ASSERT_NULLPTR(font);

    font = bmfont_parse_file("test_data/too_many_chars.fnt");
    ASSERT_NULLPTR(font);

    font = bmfont_parse_file("test_data/too_few_chars.fnt");
    ASSERT_NULLPTR(font);

    font = bmfont_parse_file("test_data/too_many_kernings.fnt");
    ASSERT_NULLPTR(font);

    font = bmfont_parse_file("test_data/too_few_kernings.fnt");
    ASSERT_NULLPTR(font);

    font = bmfont_parse_file("test_data/too_many_pages.fnt");
    ASSERT_NULLPTR(font);

    font = bmfont_parse_file("test_data/too_few_pages.fnt");
    ASSERT_NULLPTR(font);

    if (!fail) {
        printf("%s: success\n", __FILE__);
        return 0;
    }

    return 1;
}

#endif // ifdef CMP_BMFONT_SELF_TEST