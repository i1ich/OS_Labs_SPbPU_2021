#include <stdexcept>

template<class T>
T ConfigParser::getValue(int key) const {
	T value;
	std::istringstream input(m_values.at(key));
	input >> value;

	if (input.fail()) {
		std::string typeName(typeid(T).name());
		throw std::invalid_argument("Unable convert value to type" + typeName);
	}
	return value;
}
