#ifndef STRUCTURECHART_HPP
#define STRUCTURECHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <map>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>

struct Declaration
{
		std::unique_ptr<Type> type;
		std::string varName;
};

struct Block
{

};

struct BlockSequence
{
		boost::ptr_vector<Block> blocks;
};

struct SimpleBlock : public Block
{
		std::string command;
};

struct IfElseBlock : public Block
{
		std::string condition;
		BlockSequence yes;
		BlockSequence no;
};

struct SwitchBlock : public Block
{
		std::string expression;
		std::map<std::string, BlockSequence> sequences; // Empty string: else
};

struct LoopBlock : public Block
{
		std::string condition;
		BlockSequence body;
		bool headControlled;
};

struct StructureChart
{
		std::string headline;
		std::vector<Declaration> declarations;
		BlockSequence root;
};

#endif
