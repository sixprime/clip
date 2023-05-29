#pragma once

#include <visit_struct/visit_struct_metadata.hpp>

#include <algorithm>
#include <charconv>
#include <iterator>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

namespace clip
{

struct argument
{
    std::optional<std::string> help;
};

namespace details
{

struct parser final
{
    constexpr parser(std::vector<std::string> arguments)
        : arguments(arguments)
    {
    }

    const std::vector<std::string> arguments;
    std::size_t positional_index = 0;

    // TODO: a lot of work in here.
    constexpr std::optional<std::string> get_next_positional_argument()
    {
        if (positional_index >= arguments.size())
        {
            return std::nullopt;
        }

        while (positional_index < arguments.size())
        {
            // TODO:
            // - Doesn't work for positional arguments that are negative numbers.
            // - Doesn't know arguments belong to someone else, ex: options.
            if (arguments[positional_index].starts_with("-"))
            {
                positional_index++;
            }
            else
            {
                break;
            }
        }

        return std::make_optional(arguments[positional_index++]);
    }

    // Positional arguments
    template <typename T>
    void operator()(const char* /*name*/, T& value, const argument& /*arg*/)
    {
        std::optional<std::string> argument = get_next_positional_argument();
        if (argument.has_value())
        {
            const std::string_view view { argument.value() };

            if constexpr (std::is_integral_v<T>)
            {
                // Decimal is the default base.
                int base = 10;
                std::size_t view_offset = 0;

                // Hexadecimal base.
                if (view.starts_with("0x") || view.starts_with("0X"))
                {
                    base = 16;
                    view_offset = 2;
                }
                // Binary base.
                else if (view.starts_with("0b") || view.starts_with("0B"))
                {
                    base = 2;
                    view_offset = 2;
                }
                // Octal base.
                else if (view.starts_with("0"))
                {
                    base = 8;
                    view_offset = 1;
                }

                const std::from_chars_result result = std::from_chars(view.data() + view_offset, view.data() + view.size(), value, base);
                // TODO: error handling
                if (result.ec != std::errc {} || result.ptr != (view.data() + view.size()))
                {
                    std::cerr << view << '\n' << std::string(std::distance(view.data(), result.ptr), ' ') << "^- here\n";
                    const std::error_code ec = std::make_error_code(result.ec);
                    std::cerr << "<error> : problem with converting '" << view << "' : " << ec.message() << " (extra = " << result.ptr << ")\n";
                }
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                // TODO
                //float ff {};
                auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), value);
                if (ec == std::errc {})
                {
                    // Good.
                    //value = ff;
                }
                else
                {
                    // TODO: error handling
                    std::cerr << "<error> : problem with converting '" << view << "'\n";
                }
            }
            else
            {
                value = view;
            }
        }
        else
        {
            std::cerr << "<error> : no positional argument found!\n";
        }
    }

    // Optional arguments
    template <typename T>
    void operator()(const char* name, std::optional<T>& value, const argument& /*arg*/)
    {
        for (std::size_t i {}; i < arguments.size(); ++i)
        {
            // TODO: optimization. Use 2 containers: one for used arguments and one for not-used-yet arguments. Or not?
            if (arguments[i] == (std::string { "--" } + name))
            {
                //std::cerr << "# found " << arguments[i] << std::endl;

                // Set the value equal to the next argument.
                // TODO: error handling.
                // TODO: check it's a value type and not another kind of argument.
                if (i + 1 < arguments.size())
                {
                    const std::string_view view { arguments[i + 1] };
                    //std::cerr << "# str value = " << view << std::endl;

                    //if constexpr (std::is_arithmetic_v<T>)
                    if constexpr (std::is_integral_v<T>)
                    {
                        // Decimal is the default base.
                        int base = 10;
                        std::size_t view_offset = 0;

                        // Hexadecimal base.
                        if (view.starts_with("0x") || view.starts_with("0X"))
                        {
                            base = 16;
                            view_offset = 2;
                        }
                        // Binary base.
                        else if (view.starts_with("0b") || view.starts_with("0B"))
                        {
                            base = 2;
                            view_offset = 2;
                        }
                        // Octal base.
                        else if (view.starts_with("0"))
                        {
                            base = 8;
                            view_offset = 1;
                        }

                        T data {};
                        const std::from_chars_result result = std::from_chars(view.data() + view_offset, view.data() + view.size(), data, base);
                        value = data;
                        // TODO: error handling
                        if (result.ec != std::errc {} || result.ptr != (view.data() + view.size()))
                        {
                            std::cerr << view << '\n' << std::string(std::distance(view.data(), result.ptr), ' ') << "^- here\n";
                            const std::error_code ec = std::make_error_code(result.ec);
                            std::cerr << "<error> : problem with converting '" << view << "' : " << ec.message() << " (extra = " << result.ptr << ")\n";
                        }
                    }
                    else if constexpr (std::is_floating_point_v<T>)
                    {
                        float ff {};
                        auto [ptr, ec] = std::from_chars(view.data(), view.data() + view.size(), ff);
                        if (ec == std::errc {})
                        {
                            // Good.
                            value = ff;
                        }
                        else
                        {
                            // TODO: error handling
                            std::cerr << "<error> : problem with converting '" << view << "'\n";
                        }
                    }
                    else
                    {
                        value = view;
                    }

                    i += 2; // Next.
                }
                else
                {
                    std::cerr << "Can't set the value of '" << name << " : missing argument value!" << std::endl;
                }
            }
        }
    }

    // Flag arguments
    template <>
    void operator()(const char* /*name*/, std::optional<bool>& /*value*/, const argument& /*arg*/)
    {
    }
};

template <typename T>
T parse(std::vector<std::string> arguments)
{
    T type {};
    parser p { arguments };
    visit_struct::for_each(type, p);

    return type;
}

} // namespace details

template <typename T>
T parse(int argc, char* argv[])
{
    return details::parse<T>({ argv + 1, argc + argv });
}

} // namespace clip

#define CLIP_COMMAND VISITABLE_STRUCT_METADATA
#define CLIP_ARGUMENT(x, ...) x, clip::argument(__VA_ARGS__)

#if defined(WITH_TESTS)

#include <test.h>

struct commandline
{
    int x;
    std::string str;
    std::optional<int> id;
};

CLIP_COMMAND(commandline,
    CLIP_ARGUMENT(x, { .help = "help message for X" }),
    CLIP_ARGUMENT(str, { .help = "help message for S" }),
    CLIP_ARGUMENT(id, { .help = "help message for ID" })
);

TEST(get_next_positional_argument, "test name 1",
{
    std::vector<std::string> arguments = { "0xFF", "random text", "--id", "10" };
    clip::details::parser parser { arguments };

    std::optional<std::string> result_1 = parser.get_next_positional_argument();
    TEST_EQ(result_1.value_or("<none>"), "0xFF");

    std::optional<std::string> result_2 = parser.get_next_positional_argument();
    TEST_EQ(result_2.value_or("<none>"), "random text");

    // No more positional arguments.
    std::optional<std::string> result_3 = parser.get_next_positional_argument();
    TEST_EQ(result_3.value_or("<none>"), "<none>");
    // TODO: `//TEST_EQ(result_3, std::nullopt);` would be nicer
});

TEST(parser, "test name 2",
{
    std::vector<std::string> arguments = { "0xFF", "random text", "--id", "10" };
    clip::details::parser parser { arguments };
    commandline command {};
    visit_struct::for_each(command, parser);

    TEST_EQ(command.x, 255);
    TEST_EQ(command.str, "random text");
    TEST_EQ(command.id.value_or(-1), 10);
});

#endif // WITH_TESTS
