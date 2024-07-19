#include <gtest/gtest.h>

extern "C" {
#include "utils.h"
}

class UtilsTest : public ::testing::Test {
   protected:
    char *original_home;
    void SetUp() override {
        original_home = getenv("HOME");
        ASSERT_NE(setenv("HOME", "/home/user", 1), -1);
    }

    void TearDown() override { ASSERT_NE(setenv("HOME", original_home, 1), -1); }
};

TEST_F(UtilsTest, PathConcatNormalPaths) {
    const char *parent_path = "/home/user";
    const char *relative_path = "dir/file.txt";
    const char *expected = "/home/user/dir/file.txt";
    char *result = path_concat(parent_path, relative_path);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, expected);
    free(result);
}

TEST_F(UtilsTest, PathConcatEmptyStrings) {
    char *result = path_concat("", "");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "/");
    free(result);
}

TEST_F(UtilsTest, ExpandSoloTilde) {
    const char *path = "~";
    const char *expected = "/home/user";
    char *result = expand_tilde(path);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, expected);
    free(result);
}

TEST_F(UtilsTest, ExpandTildeWithSlash) {
    const char *path = "~/";
    const char *expected = "/home/user/";
    char *result = expand_tilde(path);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, expected);
    free(result);
}

TEST_F(UtilsTest, ExpandTildeNormalPath) {
    const char *path = "~/.local/share";
    const char *expected = "/home/user/.local/share";
    char *result = expand_tilde(path);
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, expected);
    free(result);
}
