#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <exception>

#include "parse_result.h"
#include "provit.h"

using i32 = std::int32_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

struct parse_context_for_single_file
{
    std::string_view filename;
    std::vector<std::string_view> lines;
    std::size_t current_line = 1;
    std::size_t consumed_from_current_line = 0;

    std::string_view get_current_line() const
    {
        return lines[current_line - 1];
    }
    size_t get_current_line_number() const { return current_line; }
    // One indexed and not zero indexed
    std::string_view get_line(size_t row) const { return lines[row - 1]; }

    void consume(size_t consumed) { consumed_from_current_line += consumed; }

    std::string_view get_remaining_of_line() const
    {
        return get_current_line().substr(consumed_from_current_line);
    }

    void advance_line()
    {
        ++current_line;
        consumed_from_current_line = 0;
    }
};

template <typename out>
struct parser
{
    // Parses a given part of the input context
    // produces either a parse success or a parse error
    // intentionally does not consume the characters from the
    // context so as to give greater freedom to the end user
    virtual parse_result<out> parse(
        const parse_context_for_single_file& context) = 0;

    parse_result<out> parseAndConsume(parse_context_for_single_file& context)
    {
        parse_result<out> res = parse(context);
        switch (res.index())
        {
            case 0:
            {
                context.consume(std::get<0>(res).consumedChars);
            }
            break;
            case 1:
            {
                // we do nothing
            }
            break;
            default:
                throw std::exception("Should be unreachable!!");
        }
        return res;
    }
};

struct fail_parser : parser<void>
{
    parse_result<void> parse(
        const parse_context_for_single_file& context) override
    {
        return parse_error{0, context.get_remaining_of_line().length(),
                           "Fail parser invoked"};
    }
};

// inefficient i64 parser just for demonstration
struct integer_parser : parser<i64>
{
    parse_result<i64> parse(
        const parse_context_for_single_file& context) override
    {
        const std::string_view str = context.get_remaining_of_line();
        const size_t len = str.length();
        size_t consumed = 0;
        for (size_t i = 0; i < len; ++i, ++consumed)
        {
            if (str[i] < '0' || '9' < str[i])
            {
                break;
            }
        }

        if (consumed != 0)
        {
            char* end = nullptr;
            return parse_success<i64>{strtoll(str.data(), &end, 10), consumed};
        }
        else
        {
            return parse_error{0, 1, "Expected integer!"};
        }
    }
};

struct text_parser : parser<void>
{
    const std::string text;
    const size_t textLen;

    text_parser(const std::string& text) : text(text), textLen(text.length()) {}

    parse_result<void> parse(
        const parse_context_for_single_file& context) override
    {
        const std::string_view currLine = context.get_remaining_of_line();
        if (currLine.length() < textLen)
        {
            return parse_error{0, currLine.length(),
                               "Expected \"" + text +
                                   "\" got: " + std::string(currLine.data())};
        }

        for (size_t i = 0; i < textLen; ++i)
        {
            if (text[i] != currLine[i])
            {
                return parse_error{0, textLen,
                                   "Expected \"" + text + "\" got: " +
                                       std::string(currLine.data())};
            }
        }

        return parse_success<void>{textLen};
    }
};

struct eof_parser : parser<void>
{
    parse_result<void> parse(
        const parse_context_for_single_file& context) override
    {
        if (context.get_remaining_of_line().length() == 0)
        {
            return parse_success<void>{0};
        }
        else
        {
            return parse_error{0, context.get_remaining_of_line().length(),
                               "Expected end of input!"};
        }
    }
};

std::string repeated(char ch, size_t cnt)
{
    return std::string(cnt, ch);
}

int main()
{
    printf("Starting provit v%d.%d\n", provit_VERSION_MAJOR,
           provit_VERSION_MINOR);

    integer_parser integer;
    text_parser text("text");
    text_parser plus("+");
    eof_parser eof;

    fail_parser failer;

    parse_context_for_single_file context = {
        "no-file", {"12+13", "123asdasd123a", "text123"}};

    puts("Processing first line of input\n");

    i64 intA = std::get<0>(integer.parseAndConsume(context)).value;
    plus.parseAndConsume(context);
    i64 intB = std::get<0>(integer.parseAndConsume(context)).value;

    printf("%lld + %lld = %lld\n", intA, intB, intA + intB);

    return 0;
}