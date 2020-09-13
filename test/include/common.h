#ifndef _FRAGVIEW_ASSERT_COMMON_H_
#define _FRAGVIEW_ASSERT_COMMON_H_ 1
#include<gtest/gtest.h>
#include<cstddef>

class CommonBaseTest : public testing::Test {
public:
protected:
	void SetUp() override;

	void TearDown() override;
};

#endif