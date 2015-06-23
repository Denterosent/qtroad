#ifndef STRUCTURECHART_HPP
#define STRUCTURECHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <map>
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
		BlockSequence() = default;
		BlockSequence(BlockSequence&&) = default;
		std::vector<std::unique_ptr<Block>> blocks;
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
			, yes(std::move(yes))
			, no(std::move(no))
		{
		}

		std::string condition;
		BlockSequence yes;
		BlockSequence no;
};

struct SwitchBlock : public Block
{
		SwitchBlock(std::string expression, std::map<std::string, BlockSequence> sequences)
			:expression(expression), sequences(std::move(sequences))
		{
		}
		std::string expression;
		std::map<std::string, BlockSequence> sequences; // Empty string: else
};

struct LoopBlock : public Block
{
		LoopBlock()
		{
		}

		LoopBlock(std::string condition, BlockSequence& body, bool headControlled)
			: condition(condition)
			, body(std::move(body))
			, headControlled(headControlled)
		{
		}
		std::string condition;
		BlockSequence body;
		bool headControlled;
};

struct StructureChart
{
		StructureChart()
		{
		}

		StructureChart(std::string headline, std::vector<Declaration>&& declarations, BlockSequence root)
			: headline(headline), declarations(std::move(declarations)), root(std::move(root))
		{
		}
		std::string headline;
		std::vector<Declaration> declarations;
		BlockSequence root;
};

#endif
