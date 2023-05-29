#pragma once

#include <functional>
#include <string>

class ctest
{
public:
    virtual ~ctest() = default;

    const std::string name;

protected:
    ctest(std::string test_name, std::function<void(const ctest&)> test_function)
        : name(test_name)
        , function(test_function)
    {
    }

    void execute()
    {
        function(*this);
    }

private:
    std::function<void(const ctest&)> function;
};

class ctest_impl : public ctest
{
public:
    ctest_impl(std::string test_name, std::function<void(const ctest&)> test_function)
        : ctest(test_name, test_function)
    {
        execute();
    }
};

#define TEST(F, N, ...) \
    namespace test \
    { \
        ctest_impl ctest_##F(N, [](const ctest& CTEST_) { __VA_ARGS__ }); \
    }

#define TEST_EQ(value, expected) \
    do \
    { \
        if (!(value == expected)) \
        { \
            std::cerr << "FAILED \"" << CTEST_.name << "\"" \
                << " [" << __FILE__ << ":" << __LINE__ << "]" \
                << " TEST_EQ(" << (#value) << ", " << (#expected) << ")" \
                << " : value=" << (value) << ", expected=" << (expected) << std::endl;\
        } \
    } while (0);
