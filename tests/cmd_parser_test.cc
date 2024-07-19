#include <gtest/gtest.h>

extern "C" {
#include "cmd_parser.h"
#include "error.h"
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
    EXPECT_EQ(cmd.size, 2);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "pwd");
    EXPECT_EQ(args[1], nullptr);
}

TEST_F(CommandParserTest, MultipleArgumentsTest) {
    char input[] = "cp file1.txt file2.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 4);

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
    EXPECT_EQ(cmd.size, 4);

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
    EXPECT_EQ(cmd.size, 3);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "Hello, World!");
    EXPECT_EQ(args[2], nullptr);
}

TEST_F(CommandParserTest, ArgumentsWithOnlySpacesTest) {
    char input[] = "echo \"     \" \"ab\"";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 4);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "     ");
    EXPECT_STREQ(args[2], "ab");
    EXPECT_EQ(args[3], nullptr);
}

TEST_F(CommandParserTest, WriteOutputRedirection) {
    char input[] = "echo \"hello\" > hello.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "hello");
    EXPECT_EQ(args[2], nullptr);
    EXPECT_STREQ(cmd.input_file, nullptr);
    EXPECT_STREQ(cmd.output_file, "hello.txt");
    EXPECT_FALSE(cmd.append);
}

TEST_F(CommandParserTest, AppendOutputRedirection) {
    char input[] = "echo \"hello\" >> hello.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "hello");
    EXPECT_EQ(args[2], nullptr);
    EXPECT_STREQ(cmd.input_file, nullptr);
    EXPECT_STREQ(cmd.output_file, "hello.txt");
    EXPECT_TRUE(cmd.append);
}

TEST_F(CommandParserTest, InputRedirection) {
    char input[] = "grep \"error\" < log.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "grep");
    EXPECT_STREQ(args[1], "error");
    EXPECT_EQ(args[2], nullptr);
    EXPECT_STREQ(cmd.input_file, "log.txt");
    EXPECT_STREQ(cmd.output_file, nullptr);
    EXPECT_FALSE(cmd.append);
}

TEST_F(CommandParserTest, InputAndWriteOutputRedirection) {
    char input[] = "sort < numbers.txt > sorted.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 2);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "sort");
    EXPECT_EQ(args[1], nullptr);
    EXPECT_STREQ(cmd.input_file, "numbers.txt");
    EXPECT_STREQ(cmd.output_file, "sorted.txt");
    EXPECT_FALSE(cmd.append);
}

TEST_F(CommandParserTest, InputAndAppendOutputRedirection) {
    char input[] = "sort < numbers.txt >> sorted.txt";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 2);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "sort");
    EXPECT_EQ(args[1], nullptr);
    EXPECT_STREQ(cmd.input_file, "numbers.txt");
    EXPECT_STREQ(cmd.output_file, "sorted.txt");
    EXPECT_TRUE(cmd.append);
}

TEST_F(CommandParserTest, WriteOutputRedirectionWithoutFileName) {
    char input[] = "echo \"hello\" >";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_NE(result, 0);
    EXPECT_EQ(get_error_code(), REDIRECTION_WITHOUT_FILENAME);
}

TEST_F(CommandParserTest, AppendOutputRedirectionWithoutFileName) {
    char input[] = "echo \"hello\" >>";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_NE(result, 0);
    EXPECT_EQ(get_error_code(), REDIRECTION_WITHOUT_FILENAME);
}

TEST_F(CommandParserTest, InputRedirectionWithoutFileName) {
    char input[] = "grep \"error\" <";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_NE(result, 0);
    EXPECT_EQ(get_error_code(), REDIRECTION_WITHOUT_FILENAME);
}

TEST_F(CommandParserTest, BackgroundProcess) {
    char input[] = "sleep 5 &";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);
    EXPECT_TRUE(cmd.is_background);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "sleep");
    EXPECT_STREQ(args[1], "5");
    EXPECT_EQ(args[2], nullptr);
}

TEST_F(CommandParserTest, BackgroundProcessWithWriteOutputRedirection) {
    char input[] = "echo \"hello\" > hello.txt &";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);
    EXPECT_TRUE(cmd.is_background);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "hello");
    EXPECT_EQ(args[2], nullptr);
    EXPECT_STREQ(cmd.output_file, "hello.txt");
    EXPECT_FALSE(cmd.append);
    EXPECT_TRUE(cmd.is_background);
}

TEST_F(CommandParserTest, BackgroundProcessWithAppendOutputRedirection) {
    char input[] = "echo \"hello\" >> hello.txt &";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);
    EXPECT_TRUE(cmd.is_background);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "echo");
    EXPECT_STREQ(args[1], "hello");
    EXPECT_EQ(args[2], nullptr);
    EXPECT_STREQ(cmd.output_file, "hello.txt");
    EXPECT_TRUE(cmd.append);
    EXPECT_TRUE(cmd.is_background);
}

TEST_F(CommandParserTest, BackgroundProcessWithInputRedirection) {
    char input[] = "grep \"error\" < log.txt &";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 3);
    EXPECT_TRUE(cmd.is_background);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "grep");
    EXPECT_STREQ(args[1], "error");
    EXPECT_EQ(args[2], nullptr);
    EXPECT_STREQ(cmd.input_file, "log.txt");
    EXPECT_FALSE(cmd.append);
    EXPECT_TRUE(cmd.is_background);
}

TEST_F(CommandParserTest, BackgroundProcessWithBothRedirectionsWrite) {
    char input[] = "sort < numbers.txt > sorted.txt &";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 2);
    EXPECT_TRUE(cmd.is_background);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "sort");
    EXPECT_EQ(args[1], nullptr);
    EXPECT_STREQ(cmd.input_file, "numbers.txt");
    EXPECT_STREQ(cmd.output_file, "sorted.txt");
    EXPECT_FALSE(cmd.append);
    EXPECT_TRUE(cmd.is_background);
}

TEST_F(CommandParserTest, BackgroundProcessWithBothRedirectionsAppend) {
    char input[] = "sort < numbers.txt >> sorted.txt &";
    int8_t result = cmd_parse_input(&cmd, input);
    ASSERT_EQ(result, 0);
    EXPECT_EQ(cmd.size, 2);
    EXPECT_TRUE(cmd.is_background);

    const char *const *args = cmd.args;
    EXPECT_STREQ(args[0], "sort");
    EXPECT_EQ(args[1], nullptr);
    EXPECT_STREQ(cmd.input_file, "numbers.txt");
    EXPECT_STREQ(cmd.output_file, "sorted.txt");
    EXPECT_TRUE(cmd.append);
    EXPECT_TRUE(cmd.is_background);
}
