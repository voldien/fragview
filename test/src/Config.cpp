#include<gtest/gtest.h>
#include<Config.h>
#include<Core/IConfig.h>
#include<Core/IO/BufferIO.h>
#include<Core/IO/IO.h>
#include<Core/IO/FileSystem.h>
#include<Core/Ref.h>
#include"IConfigTest.h"

#define CONFIG_DEFAULT_NAME "Config"

using namespace fragcore;
using namespace fragview;

void IConfigTest::TearDown() {
	Test::TearDown();
}

void IConfigTest::SetUp() {
	Test::SetUp();
	this->config = new IConfig();
	this->config->setName(CONFIG_DEFAULT_NAME);
	this->config->set<int>("a", 0);
	this->config->set<int>("b", 1);
	this->config->set<int>("c", 2);
}

//TODO move the config to fragview client.

TEST (Config, ShortOptions) {
	const char *arg[] = {
			"fragview",
			"-N",
			"-t", "brick1.png",
			"-t", "brick2.png",
			"-t", "brick3.png",
			"-t", "brick4.png",
			"-d",
			"-w",
			"-S",
			"-a",
			"-c",
			"-R", "2.0",
			"-p", "cube.obj",
			"-U",
			"-F", "w",
			"-G", "w",
			"-C", "w",
			"-H", "w",
			"-E", "w",

	};
	const int nArg = sizeof(arg) / sizeof(arg[0]);

	Config *config = Config::createConfig(nArg, arg, NULL);

	ASSERT_EQ(config->get<bool>("notify-file"), false);
	const std::vector<std::string> &textures = config->get<const std::vector<std::string>>("textures");
	ASSERT_STREQ(config->get<const char *>("texture0"), "brick1.png");
	ASSERT_STREQ(config->get<const char *>("texture1"), "brick2.png");
	ASSERT_STREQ(config->get<const char *>("texture2"), "brick3.png");
	ASSERT_STREQ(config->get<const char *>("texture3"), "brick4.png");
	ASSERT_STREQ(config->get<const char *>("texture4"), "brick5.png");
	ASSERT_EQ(config->get<bool>("save-configuration"), true);
}

TEST (Config, ConfigLongOptions) {
	const char *arg[] = {
			"fragview",
			"--renderer-opengl",
			"--disable-notify-file",
			"--fragment=fractual.frag",
			"--poly=cube.obj",
			"-f", "wave.frag",
			"--inline-script=",
			"--no-decoration",
			"--save-config",
			"--screen-width=100",
			"--screen-height=200",
			"--screen-x=100",
			"--screen-y=200",
			"--anti-aliasing=2",
			"--srgb",
			"--alpha",
			"--v-sync",
			"--compression",
			"--save-config=tmp.xml",
			"--texture=tmp.png"

	};
	const int nArg = sizeof(arg) / sizeof(arg[0]);

	Config *config = Config::createConfig(nArg, arg, NULL);
	ASSERT_STREQ(config->get<const char *>("renderer-dynamicInterface"),
	             RenderingFactory::getInterfaceLibraryPath(RenderingFactory::eOpenGL));
	ASSERT_STREQ(config->get<const char *>("poly0"), "cube.obj");
	ASSERT_STREQ(config->get<const char *>("shader0"), "wave.frag");
	ASSERT_STREQ(config->get<const char *>("texture0"), "tmp.png");

	ASSERT_EQ(config->get<int>("screen_width"), 100);
	ASSERT_EQ(config->get<int>("screen-height"), 200);

	ASSERT_EQ(config->get<int>("screen-x"), 100);
	ASSERT_EQ(config->get<int>("screen-y"), 100);

	ASSERT_EQ(config->get<bool>("save-configuration"), true);
}

TEST_F(IConfigTest, Formats){
		const char *arg[] = {
			"fragview",
			"-N",
			"-t", "brick1.png",
			"-t", "brick2.png",
			"-t", "brick3.png",
			"-t", "brick4.png",
			"-d",
			"-w",
			"-S",
			"-a",
			"-c",
			"-R", "2.0",
			"-p", "cube.obj",
			"-U",
			"-F", "w",
			"-G", "w",
			"-C", "w",
			"-H", "w",
			"-E", "w",

	};
	const int nArg = sizeof(arg) / sizeof(arg[0]);

	IConfig *config = Config::createConfig(nArg, arg, NULL);
	BufferIO bufferIo(65536);
	Ref<IO> buffer_ref = Ref<IO>(&bufferIo);
	config->save(buffer_ref, IConfig::ConfigFormat::JSON);
	buffer_ref->seek(IO::Seek::SET, 0);
	config->parseConfigFile(buffer_ref, IConfig::ConfigFormat::JSON);

	config->save(buffer_ref, IConfig::ConfigFormat::YAML);
	buffer_ref->seek(IO::Seek::SET, 0);
	config->parseConfigFile(buffer_ref, IConfig::ConfigFormat::YAML);

	config->save(buffer_ref, IConfig::ConfigFormat::XML);
	buffer_ref->seek(IO::Seek::SET, 0);
	config->parseConfigFile(buffer_ref, IConfig::ConfigFormat::XML);

}
