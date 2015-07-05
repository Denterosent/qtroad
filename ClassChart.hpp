#ifndef CLASSCHART_HPP
#define CLASSCHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <vector>
#include <memory>

enum class Visibility { private_, protected_, public_ };

struct Argument
{
		Argument(std::string name, Type* type)
			: name(name)
			, type(type)
		{
		}

		std::string name;
		std::unique_ptr<Type> type;
};

struct Operation
{
		Operation(std::string name, Type* returnType, std::vector<Argument>& arguments, Visibility visibility)
			: name(name)
			, returnType(returnType)
			, visibility(visibility)
			, arguments(std::move(arguments))
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
		Class(std::string name) :
			name(name)
		{
		}

		std::string name;
		std::vector<Operation> operations;
		std::vector<Attribute> attributes;
};

struct Edge
{
		Edge(Class* head, Class* tail)
			: head(head)
			, tail(tail)
		{
		}

		Class* head;
		Class* tail;
};

struct Inheritance : public Edge
{
		using Edge::Edge;
};

struct Association : public Edge
{
		Association(Class* head, Class* tail, unsigned multiplicity, std::string roleName)
			: Edge(head, tail)
			, multiplicity(multiplicity)
			, roleName(roleName)
		{
		}

		unsigned multiplicity;
		std::string roleName;
};

struct ClassChart
{
		std::vector<std::unique_ptr<Class>> classes;
		std::vector<std::unique_ptr<Edge>> edges;
};

#endif
