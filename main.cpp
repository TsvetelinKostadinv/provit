#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

using i32 = std::int32_t;
using i64 = std::int64_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

struct parse_error
{
    const std::string filename;
    const size_t row;
    const size_t column;
    const size_t length;
    const std::string message;
};

template <typename out>
struct parse_success
{
    out value;
    size_t consumedChars;
    std::string_view remainingStr;
};

template <typename left, typename right>
using either = std::variant<left, right>;

template <typename out>
using parse_result = either<parse_success<out>, parse_error>;

template <typename out>
struct parser
{
    virtual parse_result<out> parse(const std::string_view& str) = 0;
};

// inefficient i64 parser just for demonstration
struct integer_parser : parser<i64>
{
    virtual parse_result<i64> parse(const std::string_view& str) override
    {
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
            return parse_success<i64>{strtoll(str.data(), &end, 10), consumed,
                                      str.substr(consumed)};
        }
        else
        {
            return parse_error{"no-filename", 1, 0, 1, "Expected integer!"};
        }
    }
};

int main()
{
    std::cout << "Hello World!\n";

    integer_parser parser;
    const std::string input = "a123asdasd123a";
    const parse_result<i64> res = parser.parse(input);

    switch (res.index())
    {
        case 0:
        {
            // we got an integer
            parse_success<i64> succ = std::get<0>(res);
            printf(
                "parsed integer: %lld, parsed chars count: %lld, remaining "
                "str: %s\n",
                succ.value, succ.consumedChars, succ.remainingStr.data());
        }
        break;
        case 1:
        {
            parse_error err = std::get<1>(res);
            printf("%s", err.message.data());
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