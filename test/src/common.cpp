#include <gtest/gtest.h>
#include"common.h"
#include<Core/IO/FileSystem.h>
#include<Core/IConfig.h>
#include"Prerequisites.h"

using namespace fragcore;


void CommonBaseTest::SetUp() {
	Test::SetUp();
	Ref<IScheduler> ref;
	EXPECT_NO_THROW(FileSystem::createFileSystem(ref));
}

void CommonBaseTest::TearDown() {
	Test::TearDown();
}


