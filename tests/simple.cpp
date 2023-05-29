#include <test.h>

#include <clip/parser.h>

#include <iomanip>
#include <iostream>
#include <optional>
#include <string>

/**
 * TODO: (not necessarily in order)
 * - positional arguments
 * - optional arguments
 * - flag arguments
 * - enum class arguments
 *      > `./main --color red`
 * - vector arguments
 *      > `compiler file1 file2 file3`
 *          + with delimiter for positional arguments
 *          > `./main sed --trace X=1 Y=2 Z=3 -- 's/foo/bar/g' foo.txt`
 * - tuple arguments
 *      > `./main multiply 16 5`
 * - compound arguments (?)
 *      > `./main -ac 3.14 2.718`
 *      > `./main -ba`
 *      > `./main -c 1.5 3.0 -ab`
 * - number parsing
 *      > `./main 1 0x5B 071 0b0101 -35 +98`
 *      > `./main -3.15 +2.717 2E-4 0.1e2 .5 -.3 +5.999`
 * - subcommands
 *      > `./main config user.email "john.doe@foo.com"`
 *      > `./main init my_repo`
 * - print full help
 */

struct command_line_arguments
{
    int x;
    float f;
    std::string str;
    std::optional<int> id;
};

CLIP_COMMAND(command_line_arguments,
    CLIP_ARGUMENT(x, { .help = "help message for X" }),
    CLIP_ARGUMENT(f, { .help = "help message for F" }),
    CLIP_ARGUMENT(str, { .help = "help message for STR" }),
    CLIP_ARGUMENT(id, { .help = "help message for OPT" })
);

TEST(get_next_positional_argument, "Test if we do not mistake an optional argument value for a positional argument.",
{
    std::vector<std::string> arguments = { "0xFF", "--id", "10" };
    clip::details::parser parser { arguments };

    std::optional<std::string> result_1 = parser.get_next_positional_argument();
    TEST_EQ(result_1.value_or("<none>"), "0xFF");

    std::optional<std::string> result_2 = parser.get_next_positional_argument();
    TEST_EQ(result_2.value_or("<none>"), "<none>");
});

TEST(parser, "Test if we can parse 3 positional arguments and 1 optional argument at the end.",
{
    std::vector<std::string> arguments = { "0xFF", "4.2", "random text", "--id", "10" };
    auto clargs = clip::details::parse<command_line_arguments>(arguments);

    TEST_EQ(clargs.x, 255);
    TEST_EQ(clargs.f, 4.2f);
    TEST_EQ(clargs.str, "random text");
    TEST_EQ(clargs.id.value_or(-1), 10);
});

int main(int argc, char* argv[])
{
    const auto arguments = clip::parse<command_line_arguments>(argc, argv);

    std::cout << "command_line_arguments = { "
        << "x = " << arguments.x << ", "
        << "f = " << std::fixed << std::setprecision(8) << arguments.f << ", "
        << "str = \"" << arguments.str << "\", "
        << "id = " << arguments.id.value_or(0) << (arguments.id.has_value() ? " (is set)" : " (is NOT set)")
        << " }" << std::endl;

    return 0;
}
