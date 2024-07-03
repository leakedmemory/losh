#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>

extern "C" {
#include "io_handler.h"
}

TEST(IOHandlerTest, GetErrorMsg) {
    EXPECT_STREQ(io_get_error_msg(IO_SUCCESS), "Success");
    EXPECT_STREQ(io_get_error_msg(IO_CFG_NOT_INITIALIZED), "IOConfig not initialized");
    EXPECT_STREQ(io_get_error_msg(IO_CFG_ALREADY_INITIALIZED), "IOConfig already initialized");
    EXPECT_STREQ(io_get_error_msg(IO_INPUT_TOO_BIG), "Input size was too big");
    EXPECT_STREQ(io_get_error_msg(IO_SYSTEM_ERROR), strerror(errno));
}

TEST(IOHandlerTest, AllocCfgSingleton) {
    io_alloc_cfg_singleton(stdout, stdin, stderr);
    const IOConfig *io_cfg = io_get_cfg_instance();
    EXPECT_NE(io_cfg, nullptr);

    io_alloc_cfg_singleton(stdout, stdin, stderr);
    EXPECT_EQ(io_cfg, io_get_cfg_instance());
    EXPECT_EQ(io_get_error_code(), IO_CFG_ALREADY_INITIALIZED);

    io_free_cfg_singleton();
}

TEST(IOHandlerTest, FreeCfgSingleton) {
    io_free_cfg_singleton();
    EXPECT_EQ(io_get_cfg_instance(), nullptr);
    EXPECT_EQ(io_get_error_code(), IO_CFG_NOT_INITIALIZED);

    io_alloc_cfg_singleton(stdout, stdin, stderr);
    io_free_cfg_singleton();
    EXPECT_EQ(io_get_error_code(), IO_SUCCESS);
    EXPECT_EQ(io_get_cfg_instance(), nullptr);
}

TEST(IOHandlerTest, GetCfgInstance) {
    EXPECT_EQ(io_get_cfg_instance(), nullptr);
    EXPECT_EQ(io_get_error_code(), IO_CFG_NOT_INITIALIZED);

    io_alloc_cfg_singleton(stdout, stdin, stderr);
    EXPECT_NE(io_get_cfg_instance(), nullptr);

    io_free_cfg_singleton();
}

TEST(IOHandlerTest, ReadInputStream) {
    char buf[6] = {0};
    EXPECT_EQ(io_read(buf, sizeof(buf)), -1);
    EXPECT_EQ(io_get_error_code(), IO_CFG_NOT_INITIALIZED);

    FILE *mock_input = fmemopen((void *)"test\n", 5, "r");
    ASSERT_NE(mock_input, nullptr);
    io_alloc_cfg_singleton(stdout, mock_input, stderr);
    EXPECT_EQ(io_read(buf, sizeof(buf)), 5);
    EXPECT_STREQ(buf, "test\n");
    fclose(mock_input);

    io_free_cfg_singleton();

    FILE *mock_large_input = fmemopen((void *)"testtesttest\n", 13, "r");
    ASSERT_NE(mock_large_input, nullptr);
    io_alloc_cfg_singleton(stdout, mock_large_input, stderr);
    EXPECT_EQ(io_read(buf, sizeof(buf)), -1);
    EXPECT_EQ(io_get_error_code(), IO_INPUT_TOO_BIG);
    fclose(mock_large_input);

    io_free_cfg_singleton();
}

TEST(IOHandlerTest, WriteToOutStream) {
    EXPECT_EQ(io_write("test\n"), -1);
    EXPECT_EQ(io_get_error_code(), IO_CFG_NOT_INITIALIZED);

    char buf[6] = {0};
    FILE *mock_out = fmemopen((void *)buf, sizeof(buf), "w");
    ASSERT_NE(mock_out, nullptr);
    io_alloc_cfg_singleton(mock_out, stdin, stderr);
    EXPECT_EQ(io_write("test\n"), 5);
    io_flush_out_stream();
    EXPECT_STREQ(buf, "test\n");
    fclose(mock_out);

    io_free_cfg_singleton();
}

TEST(IOHandlerTest, WriteToErrStream) {
    EXPECT_EQ(io_write_err("test\n"), -1);
    EXPECT_EQ(io_get_error_code(), IO_CFG_NOT_INITIALIZED);

    char buf[6] = {0};
    FILE *mock_err = fmemopen((void *)buf, sizeof(buf), "w");
    ASSERT_NE(mock_err, nullptr);
    io_alloc_cfg_singleton(stdout, stdin, mock_err);
    EXPECT_EQ(io_write_err("test\n"), 5);
    io_flush_err_stream();
    EXPECT_STREQ(buf, "test\n");
    fclose(mock_err);

    io_free_cfg_singleton();
}

TEST(IOHandlerTest, CleanInputStream) {
    io_clean_input_stream();
    EXPECT_EQ(io_get_error_code(), IO_CFG_NOT_INITIALIZED);

    char buf[4] = {0};
    FILE *mock_input = fmemopen((void *)"test\n", 5, "r");
    ASSERT_NE(mock_input, nullptr);
    io_alloc_cfg_singleton(stdout, mock_input, stderr);
    io_read(buf, sizeof(buf));

    char c = '\0';
    fseek(mock_input, -1, SEEK_CUR);
    EXPECT_FALSE((c = getc(mock_input) == '\n' || c == EOF));

    io_clean_input_stream();

    fseek(mock_input, -1, SEEK_CUR);
    EXPECT_TRUE((c = getc(mock_input) == '\n' || c == EOF));

    fclose(mock_input);

    io_free_cfg_singleton();
}
