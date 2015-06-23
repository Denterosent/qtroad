#include "Parser.hpp"

Parser::Parser(const char *begin, const char *end)
{
	parseStructures(begin, end);
}

void Parser::parseStructures(const char* begin, const char* end)
{
	result.structureCharts.push_back(std::unique_ptr<StructureChart>(new StructureChart("test", {},  parseFunctionBody(begin, end))));

}

BlockSequence Parser::parseFunctionBody(const char*& begin, const char* end)
{
	BlockSequence ret;
	skipWhitespaces(begin);

	while (begin != end) {
		skipWhitespaces(begin);

		if (matchWithFollowing(begin, end, "if", '(')) {
			const char* yesBlockEnd;
			const char* yesBlockBegin;
			const char* noBlockBegin;
			const char* noBlockEnd;

			bool falseBranche = false;
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			yesBlockBegin = begin;
			skipBody(begin, end, 1);
			yesBlockEnd = begin - 1;

			skipWhitespaces(begin);

			if(match(begin, end, "else")) {
				falseBranche = true;
				skipWhitespaces(begin);
				if(following(begin, end, "if")) {
					noBlockBegin = begin;
					skipIf(begin, end);
					noBlockEnd = begin - 1;
				} else {
					expect(begin, end, "{");
					noBlockBegin = begin;
					skipBody(begin, end, 1);
					noBlockEnd = begin - 1;
				}
				skipWhitespaces(begin);
			}
			BlockSequence yes = parseFunctionBody(yesBlockBegin, yesBlockEnd);
			if(falseBranche) {
				BlockSequence no = parseFunctionBody(noBlockBegin, noBlockEnd);
				ret.blocks.push_back(std::unique_ptr<Block>(new IfElseBlock(condition, yes, no)));
			} else {
				BlockSequence no;
				ret.blocks.push_back(std::unique_ptr<Block>(new IfElseBlock(condition, yes, no)));
			}
		} else if (match(begin, end, "switch")) {

		} else if (match(begin, end, "while")) {
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(std::unique_ptr<Block> (new LoopBlock(condition, body, true)));

			skipWhitespaces((begin));

		} else if (match(begin, end, "do")) {

			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			skipWhitespaces(begin);
			expect(begin, end, "while");
			std::string condition = getCondition(begin, end);
			expect(begin, end, ";");

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(std::unique_ptr<Block>(new LoopBlock(condition, body, false)));

		} else {

			const char* commandEnd = begin;
			while (*commandEnd != ';' && commandEnd < end) {
				commandEnd++;
			}

			ret.blocks.push_back(std::unique_ptr<Block>(new SimpleBlock{std::string(begin, commandEnd)}));

			if (*commandEnd == ';')
				commandEnd++;

			begin = commandEnd;
			skipWhitespaces(begin);
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
bool Parser::following(const char*& begin, const char* end, const char* literal)
{
	const char* tmp = begin;

	while (*literal != '\0') {
		if (*tmp != *literal || tmp == end) {
			return false;
		}
		tmp++;
		literal++;
	}
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
			throw std::runtime_error("A if has no valid Condition");
		}
		begin++;
	}
	const char* conditionEnd = begin;
	begin++;
	skipWhitespaces(begin);
	return std::string(conditionBegin,conditionEnd);
}

void Parser::skipBody(const char*& begin, const char* end, int pDepth)
{
	int depth = pDepth;
	while (depth != 0) {
		if (*begin == '{') {
			depth++;
		}

		if (*begin == '}') {
			depth--;
		}
		if(begin == end) {
			throw std::runtime_error("There is a missing curly brace");
		}
		begin++;
	}
}

void Parser::skipIf(const char*& begin, const char* end)
{
	while (*(begin - 1) != '{' && begin != end) {
		begin++;
	}
	skipBody(begin,end, 1);
}

const Result& Parser::getResult()
{
	return result;
}
