#include "ChartsBase.hpp"

Type* Type::createFromUmlName(std::string type)
{
	ClassType* ct = new ClassType();
	ct->name = type;
	return ct;
}

std::string ClassType::cppName()
{
	return name;
}

std::string ClassType::umlName()
{
	return name;
}
