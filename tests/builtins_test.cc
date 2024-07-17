#include <gtest/gtest.h>

extern "C" {
#include "builtins.h"
#include "env.h"
#include "io_handler.h"
}

class BuiltinsTest : public ::testing::Test {
   protected:
    char actual_output[1024];
    FILE *mock_stdout = nullptr;

    char actual_err[1024];
    FILE *mock_stderr = nullptr;

    void SetUp() override {
        env_init();
        builtins_init();
        mock_stdout = fmemopen((void *)actual_output, sizeof(actual_output), "w");
        ASSERT_NE(mock_stdout, nullptr);
        mock_stderr = fmemopen((void *)actual_err, sizeof(actual_err), "w");
        ASSERT_NE(mock_stderr, nullptr);
        io_alloc_cfg_singleton(stdin, mock_stdout, mock_stderr);
    }

    void TearDown() override {
        builtins_free();
        io_free_cfg_singleton();
        fclose(mock_stdout);
        mock_stdout = nullptr;
        fclose(mock_stderr);
        mock_stderr = nullptr;
        env_restore();
    }
};

TEST_F(BuiltinsTest, PwdCommandPrintsCurrentWorkingDirectory) {
    char *args[] = {(char *)"pwd", NULL};
    BuiltinCmd *pwd_cmd = get_builtin(args[0]);
    ASSERT_NE(pwd_cmd, nullptr);

    EXPECT_EQ(pwd_cmd->func(args), 0);
    io_flush(io_instance()->out);

    char expected_output[1024] = {0};
    snprintf(expected_output, sizeof(expected_output), "%s\n", getenv("PWD"));
    EXPECT_STREQ(actual_output, expected_output);
}

TEST_F(BuiltinsTest, CdCommandChangesDirectory) {
    char *args[] = {(char *)"cd", (char *)"Documents", NULL};
    BuiltinCmd *cd_cmd = get_builtin(args[0]);
    ASSERT_NE(cd_cmd, nullptr);

    EXPECT_EQ(cd_cmd->func(args), 0);
    io_flush(io_instance()->out);

    char expected_output[1024] = {0};
    snprintf(expected_output, sizeof(expected_output), "%s/Documents", getenv("HOME"));
    EXPECT_STREQ(getenv("PWD"), expected_output);
}

TEST_F(BuiltinsTest, CdCommandChangesToRootDirectory) {
    char *args[] = {(char *)"cd", (char *)"/usr/bin", NULL};
    BuiltinCmd *cd_cmd = get_builtin(args[0]);
    ASSERT_NE(cd_cmd, nullptr);

    EXPECT_EQ(cd_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(getenv("PWD"), "/usr/bin");
}

TEST_F(BuiltinsTest, EmptyCdCommandChangesToHomeDirectory) {
    char *args[] = {(char *)"cd", NULL};
    BuiltinCmd *cd_cmd = get_builtin(args[0]);
    ASSERT_NE(cd_cmd, nullptr);

    EXPECT_EQ(cd_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(getenv("PWD"), getenv("HOME"));
}

TEST_F(BuiltinsTest, CdCommandChangesToParentDirectoryOnDoubleDots) {
    char *args[] = {(char *)"cd", (char *)"..", NULL};
    BuiltinCmd *cd_cmd = get_builtin(args[0]);
    ASSERT_NE(cd_cmd, nullptr);

    EXPECT_EQ(cd_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(getenv("PWD"), "/home");
}

TEST_F(BuiltinsTest, CdCommandHandlesInvalidPath) {
    char *args[] = {(char *)"cd", (char *)"nonexistent", NULL};
    BuiltinCmd *cd_cmd = get_builtin(args[0]);
    ASSERT_NE(cd_cmd, nullptr);

    EXPECT_EQ(cd_cmd->func(args), -1);
    io_flush(io_instance()->err);

    EXPECT_STREQ(actual_err, "ERROR: Change directory failed: No such file or directory\n");
}

TEST_F(BuiltinsTest, EmptyEchoCommandOutputsNewLine) {
    char *args[] = {(char *)"echo", NULL};
    BuiltinCmd *echo_cmd = get_builtin(args[0]);
    ASSERT_NE(echo_cmd, nullptr);

    EXPECT_EQ(echo_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "\n");
}

TEST_F(BuiltinsTest, EchoCommandOutputsSingleString) {
    char *args[] = {(char *)"echo", (char *)"hello world", NULL};
    BuiltinCmd *echo_cmd = get_builtin(args[0]);
    ASSERT_NE(echo_cmd, nullptr);

    EXPECT_EQ(echo_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "hello world\n");
}

TEST_F(BuiltinsTest, EchoCommandOutputsMultipleStrings) {
    char *args[] = {(char *)"echo", (char *)"hello", (char *)"world", NULL};
    BuiltinCmd *echo_cmd = get_builtin(args[0]);
    ASSERT_NE(echo_cmd, nullptr);

    EXPECT_EQ(echo_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "hello world\n");
}

TEST_F(BuiltinsTest, EchoCommandOutputsEnvironmentVariable) {
    char *args[] = {(char *)"echo", (char *)"$SHELL", NULL};
    BuiltinCmd *echo_cmd = get_builtin(args[0]);
    ASSERT_NE(echo_cmd, nullptr);

    EXPECT_EQ(echo_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "losh\n");
}

TEST_F(BuiltinsTest, EchoCommandOutputsNonEnvironmentVariable) {
    char *args[] = {(char *)"echo", (char *)"$SOMETHING", NULL};
    BuiltinCmd *echo_cmd = get_builtin(args[0]);
    ASSERT_NE(echo_cmd, nullptr);

    EXPECT_EQ(echo_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "$SOMETHING\n");
}

TEST_F(BuiltinsTest, EchoCommandOutputsMultipleStringsWithEnvironmentVariables) {
    char *args[] = {(char *)"echo",   (char *)"hello",    (char *)"world", (char *)"     ",
                    (char *)"$SHELL", (char *)"$INVALID", (char *)"test",  NULL};
    BuiltinCmd *echo_cmd = get_builtin(args[0]);
    ASSERT_NE(echo_cmd, nullptr);

    EXPECT_EQ(echo_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "hello world       losh $INVALID test\n");
}

TEST_F(BuiltinsTest, WhichCommandFindsBuiltinCommand) {
    char *args[] = {(char *)"which", (char *)"echo", NULL};
    BuiltinCmd *which_cmd = get_builtin(args[0]);
    ASSERT_NE(which_cmd, nullptr);

    EXPECT_EQ(which_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "echo: shell built-in command\n");
}

TEST_F(BuiltinsTest, WhichCommandFindsExternalCommand) {
    char *args[] = {(char *)"which", (char *)"ls", NULL};
    BuiltinCmd *which_cmd = get_builtin(args[0]);
    ASSERT_NE(which_cmd, nullptr);

    EXPECT_EQ(which_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "/usr/bin/ls\n");
}

TEST_F(BuiltinsTest, WhichCommandHandlesNonExistentCommand) {
    char *args[] = {(char *)"which", (char *)"nonexistent", NULL};
    BuiltinCmd *which_cmd = get_builtin(args[0]);
    ASSERT_NE(which_cmd, nullptr);

    EXPECT_EQ(which_cmd->func(args), -1);
    io_flush(io_instance()->err);

    EXPECT_STREQ(actual_err, "losh: Command not found: nonexistent\n");
}

TEST_F(BuiltinsTest, WhereCommandListsPaths) {
    char *args[] = {(char *)"where", (char *)"echo", NULL};
    BuiltinCmd *where_cmd = get_builtin(args[0]);
    ASSERT_NE(where_cmd, nullptr);

    EXPECT_EQ(where_cmd->func(args), 0);
    io_flush(io_instance()->out);

    EXPECT_STREQ(actual_output, "echo: shell built-in command\n/usr/bin/echo\n/bin/echo\n");
}

TEST_F(BuiltinsTest, WhereCommandHandlesNonExistentCommand) {
    char *args[] = {(char *)"where", (char *)"nonexistent", NULL};
    BuiltinCmd *where_cmd = get_builtin(args[0]);
    ASSERT_NE(where_cmd, nullptr);

    EXPECT_EQ(where_cmd->func(args), -1);
    io_flush(io_instance()->err);

    EXPECT_STREQ(actual_err, "losh: Command not found: nonexistent\n");
}

TEST_F(BuiltinsTest, GetBuiltinReturnsNullOnInvalidMnemonic) {
    char *args[] = {(char *)"nonexistent", NULL};
    BuiltinCmd *nonexistent_cmd = get_builtin(args[0]);
    EXPECT_EQ(nonexistent_cmd, nullptr);
}
