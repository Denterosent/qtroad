#include "Parser.hpp"

Parser::Parser(const char *begin, const char *end)
{
	parseStructures(begin, end);
}

void Parser::parseStructures(const char* begin, const char* end)
{

		parseFunctionBody(begin, end);

}

BlockSequence Parser::parseFunctionBody(const char*& begin, const char* end)
{
	BlockSequence ret;
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
			skipBody(begin, end);
			yesBlockEnd = begin;
			begin++;

			skipWhitespaces(begin);

			if(match(begin, end, "else")) {
				falseBranche = true;
				skipWhitespaces(begin);
				if(following(begin, end, "if")) {
					noBlockBegin = begin;
					skipBody(begin, end);
					noBlockEnd = begin;
					begin++;
				} else {
					expect(begin, end, "{");
					noBlockBegin = begin;
					skipBody(begin, end);
					noBlockEnd = begin;
					begin++;
				}
				skipWhitespaces(begin);
			}
			BlockSequence yes = parseFunctionBody(yesBlockBegin, yesBlockEnd);
			if(falseBranche) {
				BlockSequence no = parseFunctionBody(noBlockBegin, noBlockEnd);
				ret.blocks.push_back(new IfElseBlock{condition, yes, no});
			}
			// missing option that no false Branch is given
		} else if (match(begin, end, "switch")) {

		} else if (match(begin, end, "while")) {
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end);
			const char* whileBlockEnd = begin;
			begin++;

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(new LoopBlock(condition, body, true));

			skipWhitespaces((begin));

		} else if (match(begin, end, "do")) {

			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end);
			const char* whileBlockEnd = begin;
			begin++;

			skipWhitespaces(begin);
			expect(begin, end, "while");
			std::string condition = getCondition(begin, end);
			expect(begin, end, ";");

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(new LoopBlock(condition, body, true));

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


