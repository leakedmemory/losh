#include <gtest/gtest.h>

#include <cstdio>

extern "C" {
#include "error.h"
#include "io_handler.h"
}

class IOHandlerTest : public ::testing::Test {
   protected:
    void TearDown() override { io_free_cfg_singleton(); }
};

TEST_F(IOHandlerTest, AllocCfgSingleton) {
    io_alloc_cfg_singleton(stdin, stdout, stderr);
    const IOConfig *io_cfg = io_get_cfg_instance();
    ASSERT_NE(io_cfg, nullptr);
    EXPECT_EQ(io_get_in_stream(), stdin);
    EXPECT_EQ(io_get_out_stream(), stdout);
    EXPECT_EQ(io_get_err_stream(), stderr);
    EXPECT_EQ(get_error_code(), SUCCESS);

    io_alloc_cfg_singleton(stdin, stdout, stderr);
    EXPECT_EQ(io_cfg, io_get_cfg_instance());
    EXPECT_EQ(get_error_code(), IO_CFG_ALREADY_INITIALIZED);
}

TEST_F(IOHandlerTest, ReadValidInput) {
    char buf[6] = {0};
    FILE *mock_input = fmemopen((void *)"test\n", 5, "r");
    ASSERT_NE(mock_input, nullptr);

    io_alloc_cfg_singleton(mock_input, stdout, stderr);
    ASSERT_EQ(io_get_in_stream(), mock_input);
    EXPECT_EQ(io_read(buf, sizeof(buf)), 5);
    EXPECT_EQ(get_error_code(), SUCCESS);
    EXPECT_STREQ(buf, "test\n");

    fclose(mock_input);
}

TEST_F(IOHandlerTest, ReadInvalidBigInput) {
    char buf[6] = {0};
    FILE *mock_large_input = fmemopen((void *)"testtesttest\n", 13, "r");
    ASSERT_NE(mock_large_input, nullptr);

    io_alloc_cfg_singleton(mock_large_input, stdout, stderr);
    ASSERT_EQ(io_get_in_stream(), mock_large_input);
    EXPECT_EQ(io_read(buf, sizeof(buf)), -1);
    EXPECT_EQ(get_error_code(), IO_INPUT_TOO_BIG);

    char c = '\0';
    fseek(mock_large_input, -1, SEEK_CUR);
    EXPECT_TRUE((c = getc(mock_large_input) == '\n' || c == EOF));

    fclose(mock_large_input);
}

TEST_F(IOHandlerTest, WriteToOutStream) {
    char buf[6] = {0};
    FILE *mock_out = fmemopen((void *)buf, sizeof(buf), "w");
    ASSERT_NE(mock_out, nullptr);

    io_alloc_cfg_singleton(stdin, mock_out, stderr);
    EXPECT_EQ(io_write("test\n"), 5);
    io_flush_out_stream();
    EXPECT_EQ(get_error_code(), SUCCESS);
    EXPECT_STREQ(buf, "test\n");

    fclose(mock_out);
}

TEST_F(IOHandlerTest, WriteToErrStream) {
    char buf[6] = {0};
    FILE *mock_err = fmemopen((void *)buf, sizeof(buf), "w");
    ASSERT_NE(mock_err, nullptr);

    io_alloc_cfg_singleton(stdout, stdin, mock_err);
    EXPECT_EQ(io_write_err("test\n"), 5);
    io_flush_err_stream();
    EXPECT_EQ(get_error_code(), SUCCESS);
    EXPECT_STREQ(buf, "test\n");

    fclose(mock_err);
}
