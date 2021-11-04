#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

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
    virtual parse_result<out> parse(
        const parse_context_for_single_file& context) = 0;
};

struct fail_parser : parser<char>
{
    virtual parse_result<char> parse(
        const parse_context_for_single_file& context) override
    {
        return parse_error{0, context.get_remaining_of_line().length(),
                           "Fail parser invoked"};
    }
};

// inefficient i64 parser just for demonstration
struct integer_parser : parser<i64>
{
    virtual parse_result<i64> parse(
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

int main()
{
    integer_parser int_parser;
    fail_parser failer;
    const std::string input = "a123asdasd123a";
    parse_context_for_single_file context = {"no-file",
                                             {"123asdasd123a", "text123"}};
    const parse_result<i64> res = int_parser.parse(context);

    switch (res.index())
    {
        case 0:
        {
            // we got an integer
            parse_success<i64> succ = std::get<0>(res);
            context.consume(succ.consumedChars);
            printf(
                "parsed integer: %lld, parsed chars count: %lld, "
                "remaining  str: %s\n",
                succ.value, succ.consumedChars,
                context.get_remaining_of_line().data());

            printf("Invoking fail parser:\n");

            const parse_error fail = std::get<1>(failer.parse(context));
            const std::string spaces =
                repeated(' ', context.consumed_from_current_line +
                                  fail.column_offset + 1);
            const std::string underlining = repeated('^', fail.length);
            printf("@%s : %lld\nMessage: %s\n\t %s\n\t%s%s\n",
                   context.filename.data(), context.get_current_line_number(),
                   fail.message.data(), context.get_current_line().data(),
                   spaces.data(), underlining.data());
        }
        break;
        case 1:
        {
            parse_error err = std::get<1>(res);
            const std::string spaces =
                repeated(' ', context.consumed_from_current_line +
                                  err.column_offset + 1);
            const std::string underlining = repeated('^', err.length);
            printf("@%s : %lld\nMessage: %s\n\t %s\n\t%s%s\n",
                   context.filename.data(), context.get_current_line_number(),
                   err.message.data(), context.get_current_line().data(),
                   spaces.data(), underlining.data());
        }
        break;
        default:
        {
            printf("Shouldn't get here\n");
            return 1;
        }
    }

    puts("--- LINE 2 ---\n");

    context.advance_line();

    const parse_result<i64> res2 = int_parser.parse(context);

    switch (res2.index())
    {
        case 0:
        {
            // we got an integer
            parse_success<i64> succ = std::get<0>(res2);
            context.consume(succ.consumedChars);
            printf(
                "parsed integer: %lld, parsed chars count: %lld, "
                "remaining  str: %s\n",
                succ.value, succ.consumedChars,
                context.get_remaining_of_line().data());
        }
        break;
        case 1:
        {
            parse_error err = std::get<1>(res2);
            const std::string spaces =
                repeated(' ', context.consumed_from_current_line +
                                  err.column_offset + 1);
            const std::string underlining = repeated('^', err.length);
            printf("@%s : %lld\nMessage: %s\n\t %s\n\t%s%s\n",
                   context.filename.data(), context.get_current_line_number(),
                   err.message.data(), context.get_current_line().data(),
                   spaces.data(), underlining.data());
        }
        break;
        default:
        {
            printf("Shouldn't get here\n");
            return 1;
        }
    }

    return 0;
}