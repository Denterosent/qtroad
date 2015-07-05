#include "ChartsBase.hpp"

Type* Type::createFromCppName(std::string type)
{
	if (type == "void" || type == "") {
		return nullptr;
	} else if (type == "int") {
		return new PrimitiveType(PrimitiveType::Integer);
	} else if (type == "float") {
		return new PrimitiveType(PrimitiveType::Float);
	} else if (type == "bool") {
		return new PrimitiveType(PrimitiveType::Boolean);
	} else if (type == "std::string" || type == "QString") {
		return new PrimitiveType(PrimitiveType::Text);
	} else if (type == "char") {
		return new PrimitiveType(PrimitiveType::Character);
	} else {
		return new ClassType(type);
	}
}

ClassType::ClassType(std::string name)
	: name(name)
{
}

std::string ClassType::umlName()
{
	return name;
}

PrimitiveType::PrimitiveType(TypeEnum typeEnum)
	: typeEnum(typeEnum)
{
}

std::string PrimitiveType::umlName()
{
	switch (typeEnum) {
		case Integer:
			return "GZ";
		case Float:
			return "FKZ";
		case Boolean:
			return "Boolean";
		case Text:
			return "Text";
		case Character:
			return "Zeichen";
	}
}
