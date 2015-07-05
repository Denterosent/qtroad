#ifndef PARSER_HPP
#define PARSER_HPP

#include "StructureChart.hpp"
#include "ClassChart.hpp"

struct Result
{
		ClassChart classChart;
		std::vector<std::unique_ptr<StructureChart>> structureCharts;
};

class Parser
{
	public:
		Parser (const char* begin, const char* end);
		const Result& getResult();

	private:

		void parseClasses(const char* begin, const char* end);

		void parseStructures(const char* begin, const char* end);
		BlockSequence parseFunctionBody(const char*& begin, const char* end);
		static void skipWhitespaces(const char*& c, const char* end);
		static void skipWhitespacesBackwards(const char*& c, const char* begin);
		static bool match(const char*& begin, const char* end, const char* literal);
		static bool matchWithFollowing(const char*& begin, const char* end, const char* literal, const char following);
		static bool following(const char* begin, const char* end, const char* literal); // like match but without jumping to the last position of tmp
		static void expect(const char*& begin, const char* end, const char* literal);
		std::string getCondition (const char*& begin, const char* end);
		std::string cleanSyntax (const char* begin, const char* end);
		void skipBody(const char*& begin, const char* end, int pDepth);
		void skipIf(const char*& begin, const char* end);
		bool BothAreSpaces(char lhs, char rhs);

		Result result;

		std::map<std::string, Class*> classMap;

		void getName(const char*& begin, const char* end);
		void parseClass(const char*& begin, const char* end);
		Operation parseOperation(const char* begin, const char* end, Visibility visibility, bool& skip);
		void parseTypeAndName(const char* begin, const char* end, std::string& name, std::string& type, bool argumentMode = false);
};

#endif // PARSER_HPP

