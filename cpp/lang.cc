#include <iostream>
#include <string>

/* Comment multiline */
// Comment.

class test {

	public:
		enum cached_lock_state { none, freed, locked, acquiring, releasing };

		test() { return; }
		void tester() { return; }
		void untester(int, std::string);

	private:
		int private_num;

};

void
test::untester(int a, std::string b) {
	std::cout << "Num: " << a << " and String: " << b << "." << std::endl;
}


int main() { return 0; }
