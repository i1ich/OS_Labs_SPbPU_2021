#pragma once

#include <atomic>

class Runnable {
public:
	virtual void run() = 0;
	virtual ~Runnable() = default;

	bool autoDelete() const;
	void setAutoDelete(bool autoDelete);

private:
	std::atomic_bool m_autoDelete = true;
};
