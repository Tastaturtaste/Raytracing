#pragma once
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <mutex>
#include <queue>
#include <memory>
#include <concepts>

namespace ThreadPool_ {
	template<class F>
	concept returnsJob = requires(F f) { f()->std::function<void()>; };
}


class ThreadPool {

	class Job {
	public:
		virtual ~Job() {};
		virtual void execute() = 0;
	};

	template<class RetType>
	class AnyJob : public Job {
		std::packaged_task<RetType()> f_;
	public:
		AnyJob(std::packaged_task<RetType> f)
			: f_(std::move(f)) {}
		void execute() {
			f_();
		}
	};

	std::vector<std::thread> threads{ std::thread::hardware_concurrency() };
	std::queue<std::unique_ptr<Job>> jobs;
	std::size_t jobSizeCache{};
	std::mutex poolMutex;
	std::condition_variable notifyThreads;
	bool terminate{ false };
	bool stopped{ false };

	void init()
	{
		for (std::size_t i = 0; i < threads.size(); ++i) {
			threads.emplace_back([&]() {
				std::unique_ptr<Job> job;
				while (true) {
					{
						std::unique_lock<std::mutex> lock(poolMutex);
						notifyThreads.wait(lock, [&] {return !jobs.empty() || terminate; });
						if (jobs.size() < 1)
							continue;
						job = std::move(jobs.front());
						jobs.pop();
						--jobSizeCache;
					}
					if(job)
						job->execute();
				}
			});
		}
	}


public:
	ThreadPool(std::size_t poolSize)
		: threads(poolSize){
		init();
	}

	ThreadPool(){
		init();
	}

	~ThreadPool(){
		if (!stopped)
			shutdown();
	}

	std::size_t numThreads() const noexcept { return threads.size(); }
	std::size_t numJobs() const noexcept {
		return jobSizeCache; 
	}

	template<class F, class... Args>
	auto addJob(F&& f, Args&&... args){
		using retType = decltype(f(args...));
		std::packaged_task<retType()> task(std::move(std::bind(f, args...)));
		std::future<retType> future = task.get_future();
		{
			std::lock_guard<std::mutex> lock(poolMutex);
			jobs.emplace(std::move(task));
			++jobSizeCache;
		}
		notifyThreads.notify_one();
		return future;
	}

	void shutdown() {
		try {
			{
				std::lock_guard<std::mutex> lock(poolMutex);
				terminate = true;
			}
			notifyThreads.notify_all();
			for (auto& thread : threads) {
				thread.join();
			}
		}
		catch (std::exception& e) {
			assert(false);
		}
	}
};