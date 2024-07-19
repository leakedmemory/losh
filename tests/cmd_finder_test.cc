#include <gtest/gtest.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

extern "C" {
#include "cmd_finder.h"
#include "env.h"
#include "error.h"
#include "io_handler.h"
}

class CmdFinderTest : public ::testing::Test {
   protected:
    void SetUp() override {
        env_init();
        io_alloc_cfg_singleton(stdout, stdin, stderr);
        set_error_code(SUCCESS);
    }

    void TearDown() override {
        env_restore();
        io_free_cfg_singleton();
    }
};

TEST_F(CmdFinderTest, FindExistentCommand) {
    char *result = cmd_find("ls");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "/usr/bin/ls");
    EXPECT_EQ(get_error_code(), SUCCESS);
    free(result);
}

TEST_F(CmdFinderTest, FindNonExistentCommand) {
    char *result = cmd_find("nonexistent_command");
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(get_error_code(), CMD_NOT_FOUND);
}

TEST_F(CmdFinderTest, FindExistentCommandAll) {
    char **results = cmd_find_all("ls");
    ASSERT_NE(results, nullptr);
    EXPECT_STREQ(results[0], "/usr/bin/ls");
    EXPECT_STREQ(results[1], "/bin/ls");
    EXPECT_EQ(results[2], nullptr);
    EXPECT_EQ(get_error_code(), SUCCESS);

    for (size_t i = 0; results[i] != nullptr; i++) {
        free(results[i]);
    }
    free(results);
}

TEST_F(CmdFinderTest, FindNonExistentCommandAll) {
    char **results = cmd_find_all("nonexistent_command");
    ASSERT_EQ(results, nullptr);
    EXPECT_EQ(get_error_code(), CMD_NOT_FOUND);
}
