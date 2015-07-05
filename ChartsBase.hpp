#ifndef CHARTSBASE_HPP
#define CHARTSBASE_HPP

#include <string>

class Type
{
	public:
		virtual std::string umlName() = 0;
		static Type* createFromCppName(std::string type);
};

class ClassType : public Type
{
	private:
		std::string name;
	public:
		ClassType(std::string name);
		virtual std::string umlName() override;
};

class PrimitiveType : public Type
{
	public:
		enum TypeEnum { Integer, Float, Boolean, Text, Character };
		PrimitiveType(TypeEnum typeEnum);
		virtual std::string umlName() override;
	private:
		TypeEnum typeEnum;
};

#endif
