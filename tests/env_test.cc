#include <gtest/gtest.h>
#include <pwd.h>

extern "C" {
#include "env.h"
}

TEST(EnvTest, SuccessfullInitialization) {
    char *orig_user = getenv("USER") ? strdup(getenv("USER")) : nullptr;
    char *orig_home = getenv("HOME") ? strdup(getenv("HOME")) : nullptr;
    char *orig_pwd = getenv("PWD") ? strdup(getenv("PWD")) : nullptr;
    char *orig_path = getenv("PATH") ? strdup(getenv("PATH")) : nullptr;
    char *orig_shell = getenv("SHELL") ? strdup(getenv("SHELL")) : nullptr;

    env_init();

    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    ASSERT_NE(pw, nullptr);

    EXPECT_STREQ(getenv("USER"), pw->pw_name);
    EXPECT_STREQ(getenv("HOME"), pw->pw_dir);
    EXPECT_STREQ(getenv("PWD"), pw->pw_dir);
    EXPECT_STREQ(getenv("PATH"), "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:.");
    EXPECT_STREQ(getenv("SHELL"), "losh");

    env_restore();

    EXPECT_STREQ(getenv("USER"), orig_user);
    EXPECT_STREQ(getenv("HOME"), orig_home);
    EXPECT_STREQ(getenv("PWD"), orig_pwd);
    EXPECT_STREQ(getenv("PATH"), orig_path);
    EXPECT_STREQ(getenv("SHELL"), orig_shell);

    free(orig_user);
    free(orig_home);
    free(orig_pwd);
    free(orig_path);
    free(orig_shell);
}
