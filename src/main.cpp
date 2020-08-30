#include"FragView.h"
#include <cstdlib>
#include <iostream>
#include <csignal>
#include <execinfo.h>
#include <dlfcn.h>
#include <Exception/IException.h>
using namespace fragcore;
using namespace fragview;

void handler(int sig) {
	switch (sig) {
		case SIGABRT:
		case SIGILL:
		case SIGTERM:
		case SIGSEGV: {
			void *array[128];
			const int nArrays = sizeof(array) / sizeof(array[0]);
			size_t size;

			// get void*'s for all entries on the stack
			size = backtrace(array, nArrays);

			// print out all the frames to stderr
			fprintf(stderr, "Error: signal %d:\n", sig);
			backtrace_symbols_fd(array, size, STDERR_FILENO);
			exit(EXIT_FAILURE);
		}
		default:
			break;
	}
}

int main(int argc, const char **argv) {

	/*  Add debug functionality for signal associated with error.    */
	signal(SIGABRT, handler);
	signal(SIGILL, handler);
	signal(SIGTERM, handler);
	signal(SIGSEGV, handler);

	try {
		//TODO add support for logging support.
		//Log::addIOOutput(stdoutIO, Log::VERBOSITY);
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
