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

template <typename out>
struct parse_success
{
    out value;
    size_t consumedChars;
};

// Specialization for when the parser
// doesn't produce any part of the AST
template <>
struct parse_success<void>
{
    size_t consumedChars;
};

template <typename left, typename right>
using either = std::variant<left, right>;

template <typename out>
using parse_result = either<parse_success<out>, parse_error>;
