// This file is put into the Public Domain.

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <cstddef>
#include <atomic>
#include <chrono>

class semaphore {
private:
	std::mutex x_m;
	std::condition_variable x_cv;
	std::size_t x_n;

public:
	explicit semaphore(std::size_t n_init = 0)
		: x_m(), x_cv(), x_n(n_init)
	{
	}

public:
	void p(){
		std::unique_lock<std::mutex> lock(x_m);
		x_cv.wait(lock, [&]{ return x_n > 0; });
		--x_n;
	}
	void v(){
		std::unique_lock<std::mutex> lock(x_m);
		if(x_n == static_cast<std::size_t>(-1)){
			throw std::overflow_error("semaphore::v()");
		}
		++x_n;
		x_cv.notify_all();
	}
};

int main(){
	const std::size_t n_threads = std::thread::hardware_concurrency() - 1; // One for main thread
	constexpr std::size_t n_loops = 1000000;

	std::atomic<std::size_t> n_ping(0), n_pong(0);

	std::vector<std::thread> threads(n_threads);
	semaphore sem_g;
	std::vector<semaphore> sems(n_threads);
	for(std::size_t i = 0; i < n_threads; ++i){
		threads[i] = std::thread([&, i]{
			for(std::size_t j = 0; j < n_loops; ++j){
				sems[i].p();
				n_pong.fetch_add(1, std::memory_order_relaxed);
				sem_g.v();
			}
		});
	}
	const auto t1 = std::chrono::high_resolution_clock::now();
	for(std::size_t j = 0; j < n_loops; ++j){
		for(auto&& sem : sems) {
			n_ping.fetch_add(1, std::memory_order_relaxed);
			sem.v();
		}
		for(std::size_t i = 0; i < n_threads; ++i) {
			sem_g.p();
		}
	}
	const auto t2 = std::chrono::high_resolution_clock::now();
	for(auto&& thread : threads){
		thread.join();
	}

	std::cout <<"n_ping = " <<n_ping.load(std::memory_order_relaxed) <<std::endl
	          <<"n_pong = " <<n_pong.load(std::memory_order_relaxed) <<std::endl
	          <<"delta_milliseconds = " <<std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() <<std::endl;
}
