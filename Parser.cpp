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
	skipWhitespaces(begin, end);

	while (begin != end) {
		skipWhitespaces(begin, end);

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

			skipWhitespaces(begin, end);

			if(match(begin, end, "else")) {
				falseBranche = true;
				skipWhitespaces(begin, end);
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
				skipWhitespaces(begin,end);
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

		} else if (matchWithFollowing(begin, end, "while", '(')) {
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(std::unique_ptr<Block> (new LoopBlock(condition, body, true)));

			skipWhitespaces(begin, end);

		} else if (matchWithFollowing(begin, end, "do", '{')) {

			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			skipWhitespaces(begin, end);
			expect(begin, end, "while");
			std::string condition = getCondition(begin, end);
			expect(begin, end, ";");

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(std::unique_ptr<Block>(new LoopBlock(condition, body, false)));

		} else if (matchWithFollowing(begin, end, "for", '(')){
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.blocks.push_back(std::unique_ptr<Block> (new LoopBlock(condition, body, true)));

			skipWhitespaces(begin, end);

		}else {

			const char* commandEnd = begin;
			while (*commandEnd != ';' && commandEnd < end) {
				commandEnd++;
			}

			ret.blocks.push_back(std::unique_ptr<Block>(new SimpleBlock{std::string(begin, commandEnd)}));

			if (*commandEnd == ';')
				commandEnd++;

			begin = commandEnd;
			skipWhitespaces(begin, end);
		}
	}
	return ret;
}

void Parser::skipWhitespaces(const char*& c, const char* end)
{
	while ((*c == ' ' || *c == '\t' || *c == '\n') && c != end)
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
	skipWhitespaces(tmp, end);
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
	skipWhitespaces(tmp, end);
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
			throw std::runtime_error(std::string("Missing ") + literal);
		}
		tmp++;
		literal++;
	}
	begin = tmp;
}

std::string Parser::getCondition(const char*& begin, const char* end)
{
	int braces = 1;
	bool text = false;
	skipWhitespaces(begin, end);
	expect(begin, end, "(");

	const char* conditionBegin = begin;
	while (braces != 0) {
		if (*begin == '\\' && begin + 2 < end) {
			begin = begin + 2;
		}
		if (*begin == '\"' || *begin == '\'') {
			text = !text;
		}
		if(*begin == '(' && !text) {
			braces++;
		}else if(*begin == ')' && !text) {
			braces--;
		}
		if(begin == end) {
			throw std::runtime_error("invalid condition");
		}
		if (braces != 0) {
			begin++;
		}
	}
	const char* conditionEnd = begin;
	if(begin != end) {
		begin ++;
	}
	skipWhitespaces(begin, end);
	return std::string(conditionBegin,conditionEnd);
}

void Parser::skipBody(const char*& begin, const char* end, int pDepth)
{
	int depth = pDepth;
	bool text = false;
	while (depth != 0) {
		if(*begin == '\\' && begin < end) {
			begin++;
		}
		if (*begin == '{' && !text) {
			depth++;
		}

		if (*begin == '}' && !text) {
			depth--;
		}
		if(begin == end) {
			throw std::runtime_error("Missing }");
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
