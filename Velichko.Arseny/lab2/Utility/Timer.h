#pragma once

#include <csignal>
#include <ctime>
#include <functional>

class Timer {
public:
	Timer();
	~Timer();

	using Callback = std::function<void ()>;
	void callOnTimeout(const Callback& callback);

	//interval in milliseconds
	void start(int interval);
	void stop();

private:
	static void onTimeout(union sigval val);

	pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	timer_t m_id;
	Callback m_callback;
};
