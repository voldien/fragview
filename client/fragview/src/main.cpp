#include "FragView.h"
#include <Exception/IException.h>
#include <csignal>
#include <cstdlib>
#include <dlfcn.h>
#include <execinfo.h>
#include <iostream>
using namespace fragcore;
using namespace fragview;

int main(int argc, const char **argv) {

	/*  Add debug functionality for signal associated with error.    */
	signal(SIGABRT, handler);
	signal(SIGILL, handler);
	signal(SIGTERM, handler);
	signal(SIGSEGV, handler);

	try {
		// TODO add support for logging support.
		// Log::addIOOutput(stdoutIO, Log::VERBOSITY);
		FragView(argc, argv).run();
	} catch (fragcore::IException &ex) {
		std::cerr << "Internal exception - " << ex.getName() << std::endl;
		std::cerr << ex.what() << std::endl;
		std::cerr << ex.getBackTrace() << std::endl;
		return EXIT_FAILURE;

	} catch (std::exception &ex) {
		std::cerr << "External library error" << std::endl;
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
