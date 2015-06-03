#ifndef STRUCTURECHART_HPP
#define STRUCTURECHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <map>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <vector>

struct Declaration
{
		std::unique_ptr<Type> type;
		std::string varName;
};

struct Block
{
		virtual ~Block()
		{
		}
};

struct BlockSequence
{
		boost::ptr_vector<Block> blocks;
//		std::vector<std::shared_ptr<Block>> blocks;
};

struct SimpleBlock : public Block
{
		SimpleBlock(std::string command)
			: command(command)
		{
		}
		std::string command;
};

struct IfElseBlock : public Block
{
		IfElseBlock(std::string condition, BlockSequence& yes, BlockSequence& no)
			: condition(condition)

		{
			this->yes.blocks.swap(yes.blocks);
			this->no.blocks.swap(no.blocks);
		}

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
