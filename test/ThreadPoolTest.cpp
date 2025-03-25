#include <gtest/gtest.h>
#include "utils/ThreadPool.h"


TEST(ThreadPool, SingletonInstance) {
	ThreadPool &pool1 = ThreadPool::getInstance();
	ThreadPool &pool2 = ThreadPool::getInstance();
	EXPECT_EQ(&pool1, &pool2);  // 确保是单例
}

TEST(ThreadPool, PushTask) {
	ThreadPool &pool = ThreadPool::getInstance();
	std::atomic<int> counter = 0;

	pool.push([&]() {
	  std::this_thread::sleep_for(std::chrono::milliseconds(100));
	  counter.fetch_add(1, std::memory_order_relaxed);
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	EXPECT_EQ(counter.load(std::memory_order_relaxed), 1);
}

TEST(ThreadPool, CommitTask) {
	ThreadPool &pool = ThreadPool::getInstance();
	std::atomic<int> counter = 0;

	auto future = pool.commit([&]() {
	  std::this_thread::sleep_for(std::chrono::milliseconds(100));
	  counter.fetch_add(1, std::memory_order_relaxed);
	});

	future.get();  // 等待任务执行完成
	EXPECT_EQ(counter.load(std::memory_order_relaxed), 1);
}

TEST(ThreadPool, PushTaskAfterStop) {
	ThreadPool &pool = ThreadPool::getInstance();
	pool.stop();  // 停止线程池

	std::atomic<int> counter = 0;
	pool.push([&]() { counter.fetch_add(1, std::memory_order_relaxed); });


	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	EXPECT_EQ(counter.load(std::memory_order_relaxed), 0);  // 任务不会执行
}

