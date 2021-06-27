#include <string>
#include <map>
#include <deque>
#include <mutex>
#include <condition_variable>

class lclient {
	public:
		lclient() {}
		lclient(std::string clt_id) {}
		void add_client_info(std::string clt_id, int seq_nr) {
			client_id = clt_id;
			sequence_number = seq_nr;
		}
		int get_client_nonce() { return client_nonce; }
		std::string get_client_id() { return client_id; }
		int get_seq_nr() { return sequence_number; }

	protected:
		int client_nonce;
		std::string client_id;
		int sequence_number;
};

class managed_lock {

	public:
		enum managed_lock_state { freed, locked };

		managed_lock() { return; }

		void acquire_internal() { this->internal_lock.lock(); }
		void release_internal() { this->internal_lock.unlock();}

		void set_state(managed_lock_state state) {
			if (state == locked) {
				this->nacquire++;
				// TODO: set holder.
			}
		}
		managed_lock_state get_state() { return this->state; }

		int get_nacquire() { return this->nacquire; }

		void add_prospect(lclient clt) { this->prospects.push_back(clt); }
		bool remove_prospect(std::string clt_id) {
			for (unsigned int i = 0; i < this->prospects.size(); i++) {
				if (this->prospects[i].get_client_id() == clt_id) {
					this->prospects.erase(prospects.begin() + i);
					return true;
				}
			}
			return false;
		}
		std::deque<lclient> get_prospects() { return this->prospects; }
		bool has_prospects() { return !this->prospects.empty(); }
		lclient next_prospect() {
			lclient prosp = this->prospects.front();
			this->prospects.pop_front();
			return prosp;
		}
		size_t get_prospect_size() { return this->prospects.size(); }

		void add_holding_client_info(std::string clt_id, int seq_nr) {
			this->holding_client.add_client_info(clt_id, seq_nr);
		}
		lclient get_holding_client() { return this->holding_client; }

	protected:
		std::recursive_mutex internal_lock;
		managed_lock_state state;
		int nacquire;
		lclient holding_client;
		std::deque<lclient> prospects;
};

class managed_locks_monitor {

	public:
		void
			lock(int lid, std::string clt_id, int seq_nr) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				return lm_map[lid].set_state(managed_lock::locked);
			}

		void
			unlock(int lid, std::string clt_id) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				return lm_map[lid].set_state(managed_lock::freed);
			}

		bool
			is_locked(int lid) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				return lm_map[lid].get_state() == managed_lock::locked;
			}

		bool
			is_free(int lid) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				return lm_map[lid].get_state() == managed_lock::freed;
			}

		void
			add_prospect(int lid, lclient prosp) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				return lm_map[lid].add_prospect(prosp);
			}

		bool
			has_prospects(int lid) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				return lm_map[lid].has_prospects();
			}

		void
			remove_prospect(int lid, std::string prosp_id) {
				const std::lock_guard<std::recursive_mutex> rl(lm_lock);
				lm_map[lid].remove_prospect(prosp_id);
			}

		lclient get_holder(int lid) {
			const std::lock_guard<std::recursive_mutex> rl(lm_lock);
			return lm_map[lid].get_holding_client();
		}

		std::deque<lclient> get_prospects(int lid) {
			return lm_map[lid].get_prospects();
		}

		int get_nacquire(int lid) {
			return lm_map[lid].get_nacquire();
		}

	private:
		std::map<int, managed_lock> lm_map;

		std::recursive_mutex lm_lock;
		std::condition_variable lm_condition;
};

int main() { return 0; }
