#ifndef CHARTSBASE_HPP
#define CHARTSBASE_HPP

#include <string>

class Type
{
	protected:
		std::string name;
	public:
		virtual std::string cppName() = 0;
		virtual std::string umlName() = 0;
		static Type* createFromUmlName(std::string type);
};

class ClassType : public Type
{
	public:
		virtual std::string cppName() override;
		virtual std::string umlName() override;
};

class PrimitiveType : public Type
{
	public:
		enum { Integer, Float, Boolean, Text, Character };
		virtual std::string cppName() override;
		virtual std::string umlName() override;
};

#endif
