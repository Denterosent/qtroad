#ifndef CLASSCHART_HPP
#define CLASSCHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <vector>
#include <memory>

enum class Visibility { private_, protected_, public_ };

struct Argument
{
		std::string name;
		std::unique_ptr<Type> type;
};

struct Operation
{
		Operation(std::string name, Type* returnType, Visibility visibility)
			: name(name)
			, returnType(returnType)
			, visibility(visibility)
		{
		}

		std::string name;
		std::unique_ptr<Type> returnType;
		std::vector<Argument> arguments;
		Visibility visibility;
};

struct Attribute
{
		Attribute(std::string name, Type* type, Visibility visibility)
			: name(name)
			, type(type)
			, visibility(visibility)
		{
		}

		std::string name;
		std::unique_ptr<Type> type;
		Visibility visibility;
};

struct Class
{
		std::string name;
		std::vector<Operation> operations;
		std::vector<Attribute> attributes;
};

struct Edge
{
		Class* head;
		Class* tail;
};

struct Inheritance : public Edge
{

};

struct Association : public Edge
{

};

struct ClassChart
{
		std::vector<std::unique_ptr<Class>> classes;
		std::vector<std::unique_ptr<Edge>> edges;
};

#endif
