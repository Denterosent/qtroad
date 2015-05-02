#include "Parser.hpp"

Parser::Parser(const char *begin, const char *end)
{
	parseStructures(begin, end);
}

void Parser::parseStructures(const char* begin, const char* end)
{
	parseFunctionBody(begin, end);
}

BlockSequence Parser::parseFunctionBody(const char* begin, const char* end)
{
	BlockSequence ret;
	while (begin != end) {
		skipWhitespaces(begin);
		if (match(begin, end, "if")) {
			skipWhitespaces(begin);
			expect(begin, end, "(");
			const char* conditionBegin = begin;
			while (begin != ")") {
				begin++;
			}
			const char* conditionEnd = begin;
			begin++;
			skipWhitespaces(begin);
			expect(begin, end, "{");
			const char* blockBegin = begin;
			int depth = 1;
			while (depth != 0) {
				if (*begin == '{')
					depth++;

				if (*begin == '}')
					depth--;
			}
			const char* blockEnd = begin;
			begin++;
			BlockSequence yes = parseFunctionBody(blockBegin, blockEnd);
			ret.blocks.push_back(new IfElseBlock{std::string(conditionBegin, conditionEnd), yes, BlockSequence()});
		} else if (match(begin, end, "switch")) {

		} else if (match(begin, end, "while")) {

		} else if (match(begin, end, "do")) {

		} else {
			const char* commandEnd = begin;
			while (*commandEnd != ';' && commandEnd < end) {
				commandEnd++;
			}

			ret.blocks.push_back(new SimpleBlock{std::string(begin, commandEnd)});

			if (*commandEnd == ';')
				commandEnd++;

			begin = commandEnd;
		}
	}
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
	begin = tmp;
	return true;
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
