#include "Runnable.h"

bool Runnable::autoDelete() const {
	return m_autoDelete;
}

void Runnable::setAutoDelete(bool autoDelete) {
	m_autoDelete = autoDelete;
}
