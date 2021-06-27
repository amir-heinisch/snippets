#include <iostream>
#include <experimental/source_location>
#include <mutex>
#include <string>

using namespace std::experimental;

class deb {

	std::mutex print_lock;


	void dbg(std::string message, const source_location &location = source_location::current()) {
		const std::lock_guard<std::mutex> rl(this->print_lock);
		std::cout << "DEBUG (thread: " << pthread_self() << ") ("
			<< location.file_name() << ":" << location.line() << ") \t["
			<< location.function_name() << "] " << message << '\n';
	}

};

int main() { return 0; }
