#include "Parser.hpp"
#include <algorithm>

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
	bool comment = false;
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
					noBlockEnd = begin;
					skipWhitespaces(begin, end);

					if (match(begin, end, "else")) {
						if(following(begin, end, "if")) {
							skipIf(begin, end);
							noBlockEnd = begin;
							skipWhitespaces(begin, end);

						} else {
							expect(begin, end, "{");
							skipBody(begin, end, 1);
							noBlockEnd = begin;
						}
					}
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
				ret.getBlocks().push_back(std::unique_ptr<Block>(new IfElseBlock(condition, yes, no)));
			} else {
				BlockSequence no;
				ret.getBlocks().push_back(std::unique_ptr<Block>(new IfElseBlock(condition, yes, no)));
			}
		} else if (match(begin, end, "switch")) {

		} else if (matchWithFollowing(begin, end, "while", '(')) {
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.getBlocks().push_back(std::unique_ptr<Block> (new LoopBlock(condition, body, true)));

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

			ret.getBlocks().push_back(std::unique_ptr<Block>(new LoopBlock(condition, body, false)));

		} else if (matchWithFollowing(begin, end, "for", '(')){
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			const char* whileBlockBegin = begin;
			skipBody(begin, end, 1);
			const char* whileBlockEnd = begin - 1;

			BlockSequence body = parseFunctionBody(whileBlockBegin, whileBlockEnd);

			ret.getBlocks().push_back(std::unique_ptr<Block> (new LoopBlock(condition, body, true)));

			skipWhitespaces(begin, end);

		}else if (matchWithFollowing(begin, end, "switch", '(')) {
			std::string condition = getCondition(begin, end);
			expect(begin, end, "{");

			skipWhitespaces(begin, end);


		}else if(matchWithFollowing(begin, end, "/", '/')) {
			begin++;
			while(*begin != '\n' && begin != end) {
				begin++;
			}
		} else if (matchWithFollowing(begin, end, "/", '*')) {
			begin++;
			while(!matchWithFollowing(begin, end, "*", '/') && begin != end) {
				begin ++;
			}
			begin++;
		}else {

			const char* commandEnd = begin;
			while (*commandEnd != ';' && commandEnd < end) {
				commandEnd++;
			}
			if(begin != end){
				ret.getBlocks().push_back(std::unique_ptr<Block>(new SimpleBlock{cleanSyntax(begin, commandEnd)}));
			}
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
bool Parser::following(const char* begin, const char* end, const char* literal)
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
	bool singlelineComment = false;
	bool multilineComment = false;
	skipWhitespaces(begin, end);
	expect(begin, end, "(");

	const char* conditionBegin = begin;
	while (braces != 0) {
		if (*begin == '\\' && begin + 2 < end && !multilineComment && !singlelineComment) {
			begin = begin + 2;
		}
		if (*begin == '\"' || *begin == '\'' && !multilineComment && !singlelineComment) {
			text = !text;
		}
		if(matchWithFollowing(begin, end, "/", '*') || matchWithFollowing(begin, end, "/", '*')) {
			begin++;
			multilineComment = !multilineComment;
		}
		if(matchWithFollowing(begin, end, "/", '/')) {
			singlelineComment = true;
		}
		if(*begin == '\n') {
			singlelineComment = false;
		}
		if(*begin == '(' && !text ) {
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
	return cleanSyntax(conditionBegin,conditionEnd);
}

std::string Parser::cleanSyntax(const char* begin, const char* end)
{
	bool text = false;
	bool backslash = false;
	const char* start = begin;
	std::string tmp;
	while (begin != end) {

		if((*begin == '\"' || *begin == '\'') && !backslash) {
			text = !text;
		}
		if(*begin == '\\') {
			backslash = !backslash;
		}
		if(!text) {
			switch (*begin) {
				case '=':
					if(matchWithFollowing(begin, end, "=", '=')) {
						tmp.append("=");
					} else{
						if(begin - 1 >= start) {
							if(*(begin - 1) == '<' || *(begin - 1) == '>') {
								tmp.append("=");
							} else {
								tmp.append(" \u2190 ");
							}
						}
					}
					break;
				case '&':
					if(matchWithFollowing(begin, end, "&", '&')) {
						tmp.append(" and ");
					}
					break;
				case '|':
					if(matchWithFollowing(begin, end, "|", '|')) {
						tmp.append(" or ");
					}
					break;
				case '!':
					if(matchWithFollowing(begin, end, "!", '=')) {
						tmp.append(" <> ");
					} else {
						tmp.append(" not ");
					}
					break;
				case '%':
					tmp.append(" mod ");
					break;
				case ';':
					tmp.append(" ");
					break;
				case '+':
					if(matchWithFollowing(begin,end,"+", '+')) {

						if(tmp.find_last_of(" ") != std::string::npos) {
							std::string tmp2 = tmp.substr(tmp.find_last_of(" "),tmp.length());
							tmp.append(" \u2190 " + tmp2 + " + 1");
						} else {
						std::string tmp2 = tmp.substr(0,tmp.length());
						tmp.append(" \u2190 " + tmp2 + " + 1");
						}
					} else if (matchWithFollowing(begin,end,"+", '=')) {
						std::string tmp2 = tmp.substr(0,tmp.length());
						tmp.append(" \u2190 " + tmp2 + " + ");
					} else {
						tmp.append("+");
					}
					break;
				case '-':
					if(matchWithFollowing(begin,end,"-", '-')) {

						if(tmp.find_last_of(" ") != std::string::npos) {
							std::string tmp2 = tmp.substr(tmp.find_last_of(" "),tmp.length());
							tmp.append(" \u2190 " + tmp2 + " - 1");
						} else {
						std::string tmp2 = tmp.substr(0,tmp.length());
						tmp.append(" \u2190 " + tmp2 + " - 1");
						}
					} else if (matchWithFollowing(begin,end,"-", '=')) {
						std::string tmp2 = tmp.substr(0,tmp.length());
						tmp.append(" \u2190 " + tmp2 + " - ");
					} else {
						tmp.append("-");
					}
					break;
				default:
					if(*begin != '\n' && *begin != '\t') {
						tmp += *begin;
					}
			}
		} else {
			tmp += *begin;
		}
		begin++;

	}
	//	bool multipleSpaces = false;
	//	for(unsigned int i = 0; i <= tmp.length(); i++) {
	//		if(tmp[i] == ' ') {
	//			if(multipleSpaces || i == 0 || i == tmp.length()) {
	//				tmp.erase(i);
	//				i--;
	//			}
	//			multipleSpaces = true;
	//		} else {
	//			multipleSpaces = false;
	//		}
	//	}

	return tmp;
}

void Parser::skipBody(const char*& begin, const char* end, int pDepth)
{
	int depth = pDepth;
	bool text = false;
	bool multilineComment = false;
	bool singlelineComment = false;
	while (depth != 0) {
		if(*begin == '\\' && begin < end) {
			begin++;
		}
		if((*begin == '\"' || *begin == '\'') && begin != end && !multilineComment && !singlelineComment) {
			text = !text;
		}
		if(matchWithFollowing(begin, end, "/", '*') || matchWithFollowing(begin, end, "/", '*')) {
			begin++;
			multilineComment = !multilineComment;
		}
		if(matchWithFollowing(begin, end, "/", '/')) {
			singlelineComment = true;
		}
		if(*begin == '\n') {
			singlelineComment = false;
		}
		if (*begin == '{' && !text ) {
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

bool Parser::BothAreSpaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}
