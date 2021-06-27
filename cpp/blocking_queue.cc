#include <algorithm>
#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class blocking_queue {
	private:
		std::deque<T> dq_queue;

		std::recursive_mutex dq_lock;
		std::condition_variable_any dq_condition;

	public:
		T pop() {
			const std::lock_guard<std::recursive_mutex> rl(this->dq_lock);
			this->dq_condition.wait(this->dq_lock, [=]{ return !this->dq_queue.empty(); });
			T elem(this->dq_queue.front());
			this->dq_queue.pop_front();
			return elem;
		}

		void push(T const& value) {
			const std::lock_guard<std::recursive_mutex> rl(this->dq_lock);
			this->dq_queue.push_back(value);
			this->dq_condition.notify_one();
		}

		void push_unique(T const& value) {
			const std::lock_guard<std::recursive_mutex> rl(this->dq_lock);
			if (!this->contains(value)) {
				this->push(value);
			}
		}

		bool contains(T const& value) {
			const std::lock_guard<std::recursive_mutex> rl(this->dq_lock);
			return std::find(this->dq_queue.begin(), this->dq_queue.end(), value) != this->dq_queue.end();
		}
};

int main() { return 0; }
