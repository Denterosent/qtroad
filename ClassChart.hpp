#ifndef CLASSCHART_HPP
#define CLASSCHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <map>

enum class Visibility { private_, protected_, public_ };

class Argument
{
	public:
		Argument(std::string name, Type* type)
			: name(name)
			, type(type)
		{
		}

		std::string getName() const
		{
			return name;
		}

		const std::unique_ptr<Type>& getType() const
		{
			return type;
		}
	private:
		std::string name;
		std::unique_ptr<Type> type;
};

class Operation
{
	public:
		enum Stereotype { normal, constructor, destructor };

		Operation(std::string name, Type* returnType, std::vector<Argument>& arguments, Visibility visibility, bool abstract, Stereotype stereotype)
			: name(name)
			, returnType(returnType)
			, arguments(std::move(arguments))
			, visibility(visibility)
			, abstract(abstract)
			, stereotype(stereotype)
		{
		}

		std::string getName() const
		{
			return name;
		}

		const std::unique_ptr<Type>& getReturnType() const
		{
			return returnType;
		}

		const std::vector<Argument>& getArguments() const
		{
			return arguments;
		}

		Visibility getVisibility() const
		{
			return visibility;
		}

		bool isAbstract() const
		{
			return abstract;
		}

		Stereotype getStereotype() const
		{
			return stereotype;
		}

	private:
		std::string name;
		std::unique_ptr<Type> returnType;
		std::vector<Argument> arguments;
		Visibility visibility;
		bool abstract;
		Stereotype stereotype;
};

class Attribute
{
	public:
		Attribute(std::string name, Type* type, Visibility visibility)
			: name(name)
			, type(type)
			, visibility(visibility)
		{
		}

		std::string getName() const
		{
			return name;
		}

		const std::unique_ptr<Type>& getType() const
		{
			return type;
		}

		Visibility getVisibility() const
		{
			return visibility;
		}
	private:
		std::string name;
		std::unique_ptr<Type> type;
		Visibility visibility;
};

class Class;

class Edge
{
	public:
		Edge(Class* head, Class* tail)
			: head(head)
			, tail(tail)
		{
		}

		virtual ~Edge() = default;

		Class* getHead()
		{
			return head;
		}

		Class* getTail()
		{
			return tail;
		}
	private:
		Class* head;
		Class* tail;
};

class Class
{
	public:
		Class(std::string name) :
			name(name)
		{
		}

		std::string getName() const
		{
			return name;
		}

		const std::vector<Operation>& getOperations() const
		{
			return operations;
		}

		const std::vector<Attribute>& getAttributes() const
		{
			return attributes;
		}

		const std::vector<Edge*>& getEdges() const
		{
			return edges;
		}

		std::vector<Class*> getParentClasses() const
		{
			std::vector<Class*> parentClasses;
			for (Edge* edge : edges) {
				if (edge->getTail() == this) {
					parentClasses.push_back(edge->getHead());
				}
			}
			return parentClasses;
		}

		void getAncestorClasses(std::set<const Class*>& classes) const
		{
			for (Class* parent : getParentClasses()) {
				if (classes.find(parent) == classes.end()) {
					classes.insert(parent);
					parent->getAncestorClasses(classes);
				}
			}
		}

		void addOperation(Operation&& operation)
		{
			operations.push_back(std::move(operation));
		}

		void addAttribute(Attribute&& attribute)
		{
			attributes.push_back(std::move(attribute));
		}

		void addEdge(Edge* class_)
		{
			edges.push_back(class_);
		}

		bool isAbstract() const
		{
			std::set<const Class*> classes;
			getAncestorClasses(classes);
			classes.insert(this);
			std::map<std::string, bool> operationImplemented;
			for (const Class* parent : classes) {
				for (const Operation& operation : parent->getOperations()) {
					operationImplemented[operation.getName()] |= !operation.isAbstract();
				}
			}
			for (const std::pair<const std::string, bool>& operation : operationImplemented) {
				if (!operation.second) {
					return true;
				}
			}
			return false;
		}

	private:
		std::string name;
		std::vector<Operation> operations;
		std::vector<Attribute> attributes;
		std::vector<Edge*> edges;
};

class Inheritance : public Edge
{
	public:
		Inheritance(Class* head, Class* tail)
			: Edge(head, tail)
		{
		}
};

class Association : public Edge
{
	public:
		Association(Class* head, Class* tail, int multiplicity, std::string roleName)
			: Edge(head, tail)
			, multiplicity(multiplicity)
			, roleName(roleName)
		{
		}

		int getMultiplicity()
		{
			return multiplicity;
		}

		std::string getRoleName()
		{
			return roleName;
		}

	private:
		int multiplicity;
		std::string roleName;
};

class ClassChart
{
	public:
		void addClass(Class* class_)
		{
			classes.emplace_back(class_);
		}

		void addEdge(Edge* edge)
		{
			edges.emplace_back(edge);
			edge->getHead()->addEdge(edge);
			edge->getTail()->addEdge(edge);
		}

		const std::vector<std::unique_ptr<Class>>& getClasses() const
		{
			return classes;
		}

		const std::vector<std::unique_ptr<Edge>>& getEdges() const
		{
			return edges;
		}

	private:
		std::vector<std::unique_ptr<Class>> classes;
		std::vector<std::unique_ptr<Edge>> edges;
};

#endif
