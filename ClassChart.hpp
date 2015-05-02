#ifndef CLASSCHART_HPP
#define CLASSCHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <vector>
#include <memory>
#include <boost/ptr_container/ptr_vector.hpp>

enum class Visibility { private_, protected_, public_ };

struct Argument
{
		std::string name;
		std::unique_ptr<Type> type;
};

struct Operation
{
		std::string name;
		std::unique_ptr<Type> returnType;
		std::vector<Argument> arguments;
		Visibility visibility;
};

struct Attribute
{
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
		boost::ptr_vector<Class> classes;
		boost::ptr_vector<Edge> edges;
};

#endif
