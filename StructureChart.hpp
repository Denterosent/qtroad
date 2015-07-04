#ifndef STRUCTURECHART_HPP
#define STRUCTURECHART_HPP

#include "ChartsBase.hpp"
#include <string>
#include <map>
#include <memory>
#include <vector>

class Declaration
{
	private:
		std::string varName;
		std::string type;

	public:
		Declaration(std::string pVarName, std::string pType): varName(pVarName), type(pType)
		{
		}
		std::string getType(){return type;}
		std::string getVarName(){return varName;}
};

class Block
{
	public:
		virtual ~Block()
		{
		}
};

class BlockSequence
{
	private:
		std::vector<std::unique_ptr<Block>> blocks;
	public:
		BlockSequence() = default;
		BlockSequence(BlockSequence&&) = default;
		std::vector<std::unique_ptr<Block>>& getBlocks(){return blocks;}
		//void pushbackBlock(std::unique_ptr<Block>& block){blocks.push_back(block);}
		//void emplaceBackBlock(std::unique_ptr<Block>& block){blocks.emplace_back(block);}
};

class SimpleBlock : public Block
{
	private:
		std::string command;

	public:
		SimpleBlock(std::string command)
			: command(command)
		{
		}
		std::string getCommand(){return command;}
};

class IfElseBlock : public Block
{
	private:
		std::string condition;
		BlockSequence yes;
		BlockSequence no;

	public:
		IfElseBlock(std::string condition, BlockSequence& yes, BlockSequence& no)
			: condition(condition)
			, yes(std::move(yes))
			, no(std::move(no))
		{blockType = "IfElseBlock";}
		std::string getCondition(){return condition;}
		BlockSequence& getYes(){return yes;}
		BlockSequence& getNo(){return no;}
};

class SwitchBlock : public Block
{
	private:
		std::string expression;
		std::map<std::string, BlockSequence> sequences; // Empty string: else

	public:
		SwitchBlock(std::string expression, std::map<std::string, BlockSequence>& sequences)
			:expression(expression), sequences(std::move(sequences))
		{
		}
		std::string getExpression(){return expression;}
		std::map<std::string, BlockSequence>& getSequences(){return sequences;}
};

class LoopBlock : public Block
{
	private:
		std::string condition;
		BlockSequence body;
		bool headControlled;

	public:
		LoopBlock(std::string condition, BlockSequence& body, bool headControlled)
			: condition(condition)
			, body(std::move(body))
			, headControlled(headControlled)
		{
		}
		std::string getCondition(){return condition;}
		BlockSequence& getBody(){return body;}
		bool getHeadcontrolled(){return headControlled;}
};

class StructureChart
{
	private:
		std::string headline;
		std::vector<Declaration> declarations;
		BlockSequence root;

	public:
		StructureChart() = default;
		StructureChart(std::string headline, std::vector<Declaration>&& declarations, BlockSequence root)
			: headline(headline), declarations(std::move(declarations)), root(std::move(root))
		{
		}
		std::string getHeadline(){return headline;}
		std::vector<Declaration>& getDeclarations(){return declarations;}
		BlockSequence& getRoot(){return root;}
};

#endif
