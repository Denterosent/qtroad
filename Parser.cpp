#include "Parser.hpp"
#include <algorithm>
#include <iostream>

Parser::Parser(const char *begin, const char *end)
{
	parseStructures(begin, end);
}

void Parser::parseStructures(const char* begin, const char* end)
{
	const char* functionNameBegin;
	const char* functionNameEnd;
	const char* tmp = begin;

	while (begin <= end) {
		bool functionFound = false;
		const char* bodyBegin;
		const char* bodyEnd;

		if (following(begin, end, "class")) {
			parseClass(begin, end);
			continue;
		}
		if(matchWithFollowing(begin, end, ":", ':') ) {
			begin++;
			functionNameBegin = begin;
			while(begin != end && !following(begin,end,"(")){
				begin++;
			}
			functionNameEnd = begin;
			getCondition(begin,end);
			skipWhitespaces(begin,end);
			if(match(begin,end,"{")){
				std::vector<Declaration> declarations;
				if(match(begin,end, "$$$")){
					std::string type;
					std::string name;
					bool switched = false;
					while(begin != end && !match(begin,end, "$$$")){
						if(*begin != '/'){
							if(*begin == ' '){
								switched = !switched;
							}else if(*begin != '\n'){
								if(!switched){
									type += *begin;
								} else {
									name += *begin;
								}
							}
						}else {
							Declaration declaration(name,type);
							declarations.push_back(declaration);
							type.clear();
							name.clear();
						}
						begin++;

					}
				}
				bodyBegin = begin;
				skipBody(begin,end,1);
				bodyEnd = begin - 1;
				functionFound = true;

				result.structureCharts.push_back(std::unique_ptr<StructureChart>(new StructureChart(std::string(functionNameBegin,functionNameEnd), std::move(declarations),  parseFunctionBody(bodyBegin, bodyEnd))));
			}
		}
		if(begin<=end){
			begin++;
		}
	}
	//result.structureCharts.push_back(std::unique_ptr<StructureChart>(new StructureChart("Struktogramm", {},  parseFunctionBody(tmp, end))));
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


		}else if (match(begin, end, "#")) {
			begin++;
			while(*begin != '\n' && begin != end) {
				begin++;
			}
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
	while (c != end && (*c == ' ' || *c == '\t' || *c == '\n'))
		c++;
}

void Parser::skipWhitespacesBackwards(const char*& c, const char* begin)
{
	while (c != begin && c-1 != begin && (*(c-1) == ' ' || *(c-1) == '\t' || *(c-1) == '\n'))
		c--;
}

bool Parser::match(const char*& begin, const char* end, const char* literal)
{
	const char* tmp = begin;

	while (*literal != '\0') {
		if (tmp == end || *tmp != *literal) {
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
	const char* literalpos = literal;
	const char* tmp = begin;
	while (*literalpos != '\0') {
		if (tmp == end || *tmp != *literalpos) {
			throw std::runtime_error(std::string("Missing ") + literalpos);
		}
		tmp++;
		literalpos++;
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

void Parser::getName(const char*& begin, const char* end)
{
	while (begin != end && ((*begin >= 'A' && *begin <= 'Z') || (*begin >= 'a' && *begin <= 'z') || (*begin >= '0' && *begin <= '9'))) {
		begin++;
	}
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

void Parser::parseClass(const char*& begin, const char* end)
{
	Visibility visibility = Visibility::public_;
	if (!match(begin, end, "struct")) {
		visibility = Visibility::private_;
		expect(begin, end, "class");
	}
	skipWhitespaces(begin, end);
	const char* classNameBegin = begin;
	getName(begin, end);
	const char* classNameEnd = begin;
	std::string className(classNameBegin, classNameEnd);
	Class* c;
	if (classMap.find(className) == classMap.end()) {
		c = new Class(className);
		classMap[className] = c;
		result.classChart.addClass(c);
	} else {
		c = classMap[className];
	}
	skipWhitespaces(begin, end);
	if (match(begin, end, ":")) {
		do {
			skipWhitespaces(begin, end);
			if (!match(begin, end, "private")) {
				if (!match(begin, end, "protected")) {
					expect(begin, end, "public");
				}
			}
			skipWhitespaces(begin, end);
			const char* otherClassNameBegin = begin;
			getName(begin, end);
			const char* otherClassNameEnd = begin;
			std::string otherClassName(otherClassNameBegin, otherClassNameEnd);
			if (classMap.find(otherClassName) == classMap.end()) {
				throw std::runtime_error("Inherits undefined class");
			}
			result.classChart.addEdge(new Inheritance{classMap[otherClassName], c});
			skipWhitespaces(begin, end);
		} while (match(begin, end, ","));
	}
	if (match(begin, end, ";")) {
		skipWhitespaces(begin, end);
		return;
	}
	expect(begin, end, "{");
	skipWhitespaces(begin, end);
	while (*begin != '}') {
		if (match(begin, end, "private")) {
			visibility = Visibility::private_;
			skipWhitespaces(begin, end);
			expect(begin, end, ":");
		} else if (match(begin, end, "protected")) {
			visibility = Visibility::protected_;
			skipWhitespaces(begin, end);
			expect(begin, end, ":");
		} else if (match(begin, end, "public")) {
			visibility = Visibility::public_;
			skipWhitespaces(begin, end);
			expect(begin, end, ":");
		} else {
			const char* declBegin = begin;
			while (begin != end && *begin != '{' && *begin != ':' && *begin != ';') {
				begin++;
			}
			const char* declEnd = begin;
			if (begin == end) {
				throw std::runtime_error("Unexpected end");
			}
			if (*begin == ':') {
				while (*begin != '{') {
					begin++;
					if (begin == end) {
						throw std::runtime_error("Unexpected end");
					}
				}
			}
			if (*begin == '{') {
				begin++;
				skipBody(begin, end, 1);
			} else if (*begin == ';') {
				expect(begin, end, ";");
			}
			skipWhitespacesBackwards(declEnd, declBegin);
			if (std::string(declBegin, declEnd).find_first_of('(') != std::string::npos) {
				c->addOperation(parseOperation(declBegin, declEnd, visibility));
			} else {
				std::string type;
				std::string name;
				parseTypeAndName(declBegin, declEnd, name, type);
				if (type.back() == '*') {
					std::string className = type.substr(0, type.length() - 1);
					if (classMap.find(className) != classMap.end()) {
						result.classChart.addEdge(new Association(classMap[className], c, 1, name));
					}
				} else {
					c->addAttribute(Attribute(name, Type::createFromCppName(type), visibility));
				}
			}
		}
		skipWhitespaces(begin, end);
	}
	expect(begin, end, "}");
	skipWhitespaces(begin, end);
	expect(begin, end, ";");
	skipWhitespaces(begin, end);
}

void Parser::parseClasses(const char* begin, const char* end)
{
	while (begin != end) {
		skipWhitespaces(begin, end);
		parseClass(begin, end);
	}
}

void Parser::parseTypeAndName(const char* begin, const char* end, std::string& name, std::string& type)
{
	skipWhitespaces(begin, end);
	skipWhitespacesBackwards(end, begin);
	const char* nameBegin = end;
	while (*(nameBegin-1) != ' ') {
		nameBegin--;
	}
	const char* typeEnd = nameBegin;
	skipWhitespacesBackwards(typeEnd, begin);
	name = std::string(nameBegin, end);
	type = std::string(begin, typeEnd);
}

Operation Parser::parseOperation(const char* begin, const char* end, Visibility visibility)
{
	std::vector<Argument> arguments;
	std::string name;
	std::string type;

	const char* retBegin = begin;

	while (begin != end && *begin != '(') {
		begin++;
	}
	if (*begin != '(') {
		throw std::runtime_error("Invalid operation");
	}

	parseTypeAndName(retBegin, begin, name, type);

	begin++;
	if (begin == end) {
		throw std::runtime_error("Invalid operation");
	}
	skipWhitespaces(begin, end);
	while (*begin != ')') {
		const char* argBegin = begin;
		while (begin != end && *begin != ',' && *begin != ')') {
			begin++;
		}
		if (begin == end) {
			throw std::runtime_error("Invalid operation");
		}
		std::string argName;
		std::string argType;
		parseTypeAndName(argBegin, begin, argName, argType);
		arguments.emplace_back(argName, Type::createFromCppName(argType));
		if (*begin == ',') {
			begin++;
		}
		skipWhitespaces(begin, end);
	}
	begin++;
	skipWhitespaces(begin, end);
	if (begin != end) {
		throw std::runtime_error("Invalid operation");
	}

	return Operation(name, Type::createFromCppName(type), arguments, visibility);
}

bool Parser::BothAreSpaces(char lhs, char rhs)
{
	return (lhs == rhs) && (lhs == ' ');
}
