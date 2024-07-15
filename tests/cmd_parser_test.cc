#include <gtest/gtest.h>

extern "C" {
#include "cmd_parser.h"
}

class CommandParserTest : public ::testing::Test {
   protected:
    Command cmd;

    void SetUp() override { cmd_init(&cmd); }

    void TearDown() override { cmd_free(&cmd); }
};

TEST_F(CommandParserTest, NoArgumentsTest) {
    char input[] = "pwd";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "pwd");
    EXPECT_EQ(args[1], nullptr);
}

TEST_F(CommandParserTest, MultipleArgumentsTest) {
    char input[] = "cp file1.txt file2.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "cp");
    EXPECT_STREQ(args[1], "file1.txt");
    EXPECT_STREQ(args[2], "file2.txt");
    EXPECT_EQ(args[3], nullptr);
}

TEST_F(CommandParserTest, MultipleSpacesBetweenArgumentsTest) {
    char input[] = "mv       file1.txt     file2.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "mv");
    EXPECT_STREQ(args[1], "file1.txt");
    EXPECT_STREQ(args[2], "file2.txt");
    EXPECT_EQ(args[3], nullptr);
}

TEST_F(CommandParserTest, ArgumentsWithSpacesTest) {
    char input[] = "echo \"Hello, World!\"";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "Hello, World!");
    EXPECT_EQ(args[2], nullptr);
}

TEST_F(CommandParserTest, ArgumentsWithOnlySpacesTest) {
    char input[] = "echo \"     \" \"ab\"";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "     ");
    EXPECT_STREQ(args[2], "ab");
    EXPECT_EQ(args[3], nullptr);
}
