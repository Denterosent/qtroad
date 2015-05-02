#ifndef PARSER_HPP
#define PARSER_HPP

#include "StructureChart.hpp"
#include "ClassChart.hpp"

struct Result
{
		//ClassChart classChart;
		boost::ptr_vector<StructureChart> structureCharts;
};

class Parser
{
	public:
		Parser (const char* begin, const char* end);

	private:
		void parseStructures(const char* begin, const char* end);
		BlockSequence parseFunctionBody(const char* begin, const char* end);
		static void skipWhitespaces(const char*& c);
		static bool match(const char*& begin, const char* end, const char* literal);
		static void expect(const char*& begin, const char* end, const char* literal);
};

#endif // PARSER_HPP

