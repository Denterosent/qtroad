#include "Parser.hpp"

Parser::Parser(const char *begin, const char *end)
{
	parseStructures(begin, end);
}

void Parser::parseStructures(const char* begin, const char* end)
{
	int i = 0;
	while(begin != end) {
		parseFunctionBody(begin, end);
		i++;
	}
}

BlockSequence Parser::parseFunctionBody(const char* begin, const char* end)
{
	BlockSequence ret;
	while (begin != end) {

		skipWhitespaces(begin);

		if (matchWithFollowing(begin, end, "if", '(')) {
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* blockBegin = begin;
			skipBody(begin, end);
			const char* blockEnd = begin;
			begin++;

			BlockSequence yes = parseFunctionBody(blockBegin, blockEnd);
			//Chris, the init structure of the IfElseBlock has been modified.
			//The two lines, which are commented out don't work with this new structure.
			//Sorry.
//			ret.blocks.push_back(new IfElseBlock{condition, yes, BlockSequence()});
			skipWhitespaces(begin);

			if(matchWithFollowing(begin, end, "else" , '{')) {
				condition = getCondition(begin, end);
				expect(begin, end, "{");
				const char* blockBegin = begin;
				skipBody(begin, end);
				const char* blockEnd = begin;
				begin++;

				BlockSequence no = parseFunctionBody(blockBegin, blockEnd);
//				ret.blocks.push_back(new IfElseBlock{condition, no, BlockSequence()}); //<<<sure that the order is right? how about (condition, BlockSequence(), no)???
				skipWhitespaces(begin);
			}
		} else if (match(begin, end, "switch")) {

		} else if (match(begin, end, "while")) {

		} else if (match(begin, end, "do")) {

		} else {
			const char* commandEnd = begin;
			while (*commandEnd != ';' && commandEnd < end) {
				commandEnd++;
			}

			ret.blocks.push_back(std::unique_ptr<Block>(new SimpleBlock{std::string(begin, commandEnd)}));

			if (*commandEnd == ';')
				commandEnd++;

			begin = commandEnd;
		}
	}
	return ret;
}

void Parser::skipWhitespaces(const char*& c)
{
	while (*c == ' ' || *c == '\t' || *c == '\n')
		c++;
}

bool Parser::match(const char*& begin, const char* end, const char* literal)
{
	const char* tmp = begin;

	while (*literal != '\0') {
		if (*tmp != *literal || tmp == end) {
			return false;
		}
		tmp++;
		literal++;
	}
	skipWhitespaces(tmp);
	begin = tmp;
	return true;
}

bool Parser::matchWithFollowing(const char*& begin, const char* end, const char* literal, const char following)
{
	const char* tmp = begin;

	while (*literal != '\0') {
		if (*tmp != *literal || tmp == end) {
			return false;
		}
		tmp++;
		literal++;
	}
	skipWhitespaces(tmp);
	if(*tmp == following) {
		begin = tmp;
		return true;
	} else {
		return false;
	}
}

void Parser::expect(const char*& begin, const char* end, const char* literal)
{
	const char* tmp = begin;
	while (*literal != '\0') {
		if (*tmp != *literal || tmp == end) {
			throw std::runtime_error("Invalid file.");
		}
		tmp++;
		literal++;
	}
	begin = tmp;
}

std::string Parser::getCondition(const char*& begin, const char* end)
{
	skipWhitespaces(begin);
	expect(begin, end, "(");
	const char* conditionBegin = begin;
	while (*begin != ')') {
		if(begin == end) {
			throw std::runtime_error("An if has no valid Condition");
		}
		begin++;
	}
	const char* conditionEnd = begin;
	begin++;
	skipWhitespaces(begin);
	return std::string(conditionBegin,conditionEnd);
}

void Parser::skipBody(const char*& begin, const char* end)
{
	int depth = 1;
	while (depth != 0) {
		if (*begin == '{')
			depth++;

		if (*begin == '}')
			depth--;
		if(begin == end) {
			throw std::runtime_error("There is a missing curly brace");
		}
		begin++;
	}
}


