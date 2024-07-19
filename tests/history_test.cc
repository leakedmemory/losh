#include <gtest/gtest.h>
#include <stdio.h>

extern "C" {
#include "error.h"
#include "history.h"
#include "io_handler.h"
}

TEST(HistoryTest, SaveCmdInHistory) {
    const char *cmd = "cd dev/c/losh\n";
    const char *test_filepath = "./temp_history.txt";
    ASSERT_EQ(save_cmd_in_history(cmd, strlen(cmd), test_filepath), 0);

    FILE *test_file = fopen(test_filepath, "r");
    ASSERT_NE(test_file, nullptr);

    char buffer[1024] = {0};
    ASSERT_EQ(fread(buffer, sizeof(char), strlen(cmd), test_file), strlen(cmd));
    EXPECT_STREQ(buffer, cmd);
    EXPECT_EQ(feof(test_file), 0);

    fclose(test_file);
    remove(test_filepath);
}

TEST(HistoryTest, PrintCmdHistory) {
    const char *test_filepath = "./temp_history.txt";
    const char *cmd1 = "first command\n";
    const char *cmd2 = "second command\n";
    save_cmd_in_history(cmd1, strlen(cmd1), test_filepath);
    save_cmd_in_history(cmd2, strlen(cmd2), test_filepath);

    char output[1024] = {0};
    FILE *mock_out = fmemopen((void *)output, sizeof(output), "w");
    ASSERT_NE(mock_out, nullptr);
    io_alloc_cfg_singleton(stdin, mock_out, stderr);

    ASSERT_EQ(print_cmd_history(test_filepath), 0);
    io_flush(mock_out);
    EXPECT_STREQ(output, " 1  first command\n 2  second command\n");

    io_free_cfg_singleton();
    remove(test_filepath);
}

TEST(HistoryTest, PrintCmdHistoryFileDoesNotExist) {
    const char *non_existent_filepath = "non_existent_history.txt";

    ASSERT_EQ(print_cmd_history(non_existent_filepath), -1);
    ASSERT_EQ(get_error_code(), HISTORY_OPEN_FAILED);
    EXPECT_STREQ(get_current_error_msg(), strerror(ENOENT));
}
