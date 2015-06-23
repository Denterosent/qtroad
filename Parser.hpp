#ifndef PARSER_HPP
#define PARSER_HPP

#include "StructureChart.hpp"
#include "ClassChart.hpp"

struct Result
{
		//ClassChart classChart;
		std::vector<std::unique_ptr<StructureChart>> structureCharts;
};

class Parser
{
	public:
		Parser (const char* begin, const char* end);
		const Result& getResult();

	private:

		void parseStructures(const char* begin, const char* end);
		BlockSequence parseFunctionBody(const char*& begin, const char* end);
		static void skipWhitespaces(const char*& c);
		static bool match(const char*& begin, const char* end, const char* literal);
		static bool matchWithFollowing(const char*& begin, const char* end, const char* literal, const char following);
		static bool following(const char*& begin, const char* end, const char* literal); // like match but without jumping to the last position of tmp
		static void expect(const char*& begin, const char* end, const char* literal);
		std::string getCondition (const char*& begin, const char* end);
		void skipBody(const char*& begin, const char* end, int pDepth);
		void skipIf(const char*& begin, const char* end);

		Result result;


};

#endif // PARSER_HPP

