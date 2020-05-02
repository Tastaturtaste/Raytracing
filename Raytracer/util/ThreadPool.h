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
		AnyJob(std::packaged_task<RetType()> f) noexcept
			: f_(std::move(f)) {}
		void execute() override {
			f_();
		}
	};

	std::vector<std::thread> threads{};
	std::queue<std::unique_ptr<Job>> jobs{};
	std::size_t jobSizeCache{};
	std::mutex poolMutex{};
	std::condition_variable notifyThreads;
	bool terminate{ false };
	bool stopped{ false };

public:
	ThreadPool(std::size_t poolSize)
	{
		threads.reserve(poolSize);
		for (std::size_t i = 0; i < poolSize; ++i) {
			threads.emplace_back([&]() {
				std::unique_ptr<Job> job;
				while (!terminate) {
					{
						std::unique_lock<std::mutex> lock(poolMutex);
						notifyThreads.wait(lock, [&] {return !jobs.empty() || terminate; });
						if (jobs.size() < 1)
							continue;
						job = std::move(jobs.front());
						jobs.pop();
						jobSizeCache = jobs.size();;
					}
					if (job)
						job->execute();
				}
				});
			/*threads.back().detach();*/
		}
	}

	ThreadPool() = delete;

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
		auto upt = std::make_unique<AnyJob<retType> >(std::move(task));
		{
			std::lock_guard<std::mutex> lock(poolMutex);
			jobs.emplace(std::move(upt));
			jobSizeCache = jobs.size();
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
			spdlog::error("Exception in Threadpool: {}",e.what());
			assert(false);
		}
	}
};