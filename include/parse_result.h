#pragma once
#include <cstring>
#include <string>
#include <variant>

struct parse_error
{
    const size_t column_offset;
    const size_t length;
    const std::string message;
};

std::string repeated(char ch, size_t cnt)
{
    std::string res;
    res.resize(cnt + 1);
    memset(res.data(), ch, cnt);
    res[cnt] = 0;
    return res;
}

template <typename out>
struct parse_success
{
    out value;
    size_t consumedChars;
};

template <typename left, typename right>
using either = std::variant<left, right>;

template <typename out>
using parse_result = either<parse_success<out>, parse_error>;
