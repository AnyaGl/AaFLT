#pragma once
#include <vector>
#include "DFAWalker.h"

/* 
letter
digit
*/
const DFA IDENTIFIER_DFA = {
	{
		{ 1, -1 },
		{ 1, 1 },
	},
	{ 1 },
	{
		[](char ch) { return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_'; },
		[](char ch) { return ch >= '0' && ch <= '9'; },
	}
};
/* 
1..9
0
*/
const DFA INT_NUMBER_DFA = {
	{
		{ 1, -1 },
		{ 1, 1 },
	},
	{ 1 },
	{
		[](char ch) { return ch >= '1' && ch <= '9'; },
		[](char ch) { return ch == '0'; },
	}
};
/* 
0..9
.
*/
const DFA FIXED_POINT_NUMBER_DFA = {
	{
		{ 1, 2 },
		{ 1, 3 },
		{ 3, -1 },
		{ 3, -1 },
	},
	{ 3 },
	{
		[](char ch) { return ch >= '0' && ch <= '9'; },
		[](char ch) { return ch == '.'; },
	}
};
/* 
0..9
.
e|E
+|-
*/
const DFA FLOAT_POINT_NUMBER_DFA = {
	{
		{ 1, 2, -1, -1 },
		{ 1, 3, -1, -1 },
		{ 3, -1, -1, -1 },
		{ 3, -1, 4, -1 },
		{ -1, -1, -1, 5 },
		{ 6, -1, -1, -1 },
		{ 7, -1, -1, -1 },
		{ -1, -1, -1, -1 },
	},
	{ 7 },
	{
		[](char ch) { return ch >= '0' && ch <= '9'; },
		[](char ch) { return ch == '.'; },
		[](char ch) { return ch == 'e' || ch == 'E'; },
		[](char ch) { return ch == '+' || ch == '-'; },
	}
};

/* 
0
1
b
*/
const DFA BINARY_NUMBER_DFA = {
	{
		{ 1, -1, -1 },
		{ -1, -1, 2 },
		{ -1, 3, -1 },
		{ 3, 3, -1 },
	},
	{ 3 },
	{
		[](char ch) { return ch == '0'; },
		[](char ch) { return ch == '1'; },
		[](char ch) { return ch == 'b'; },
	}
};
/* 
0
1..7
*/
const DFA OCTAL_NUMBER_DFA = {
	{
		{ 1, -1 },
		{ -1, 2 },
		{ 2, 2 },
	},
	{ 2 },
	{
		[](char ch) { return ch == '0'; },
		[](char ch) { return ch >= '1' && ch <= '7'; },
	}
};
/* 
0
1..9|A..F
x
*/
const DFA HEX_NUMBER_DFA = {
	{
		{ 1, -1, -1 },
		{ -1, -1, 2 },
		{ -1, 3, -1 },
		{ 3, 3, -1 },
	},
	{ 3 },
	{
		[](char ch) { return ch == '0'; },
		[](char ch) { return (ch >= '1' && ch <= '9') || (ch >= 'A' && ch <= 'F'); },
		[](char ch) { return ch == 'x'; },
	}
};
/* 
"
\
..
*/
const DFA STRING_FINISH_DFA = {
	{
		{ 2, 1, 0 },
		{ 0, 0, 0 },
		{ -1, -1, -1 },
	},
	{ 2 },
	{
		[](char ch) { return ch == '"'; },
		[](char ch) { return ch == '\\'; },
		[](char ch) { return true; },
	}
};
/* 
'
\
!\ && !'
t || n || r 
*/
const DFA CHAR_FINISH_DFA = {
	{
		{ -1, 1, 2, -1 },
		{ 2, 2, -1, 2 },
		{ 3, -1, -1, -1 },
		{ -1, -1, -1, -1 },
	},
	{ 3 },
	{
		[](char ch) { return ch == '\''; },
		[](char ch) { return ch == '\\'; },
		[](char ch) { return ch != '\\' && ch != '\'' && ch != 't' && ch != 'n' && ch != 'r'; },
		[](char ch) { return ch == 't' || ch == 'n' || ch == 'r'; },
	}
};

/* 
..
*/
const DFA ONE_LINE_COMMENT_DFA = {
	{
		{ 0 },
	},
	{},
	{
		[](char ch) { return true; },
	}
};
/* 
..
*
/
*/
const DFA COMMENT_FINISH_DFA = {
	{
		{ 1, 0, 0 },
		{ 0, 2, 0 },
		{ -1, -1, -1 },
	},
	{ 2 },
	{
		[](char ch) { return ch == '*'; },
		[](char ch) { return ch == '/'; },
		[](char ch) { return true; },
	}
};

const std::vector<std::string> KEYWORDS = {
	"int",
	"double",
	"char",
	"string",
	"bool",
	"if",
	"while",
	"for",
	"true",
	"false"
};

const std::vector<std::string> OPERATION_SIGNS = {
	"-",
	"+",
	"*",
	"/",
	"=",
};

const std::vector<std::string> DELIMITERS = {
	";",
};

const std::vector<std::string> BRACKETS = {
	"(",
	")",
	"{",
	"}",
};

const std::vector<std::string> SPACES = {
	" ",
	"\f",
	"\n",
	"\r",
	"\t",
	"\v",
};

const std::vector<std::string> COMPARISON = {
	"==",
	"!=",
	"<",
	">",
};

const char COMMENT_START = '/';
const char ONE_LINE_COMMENT_START = '/';
const char MULTILINE_COMMENT_START = '*';
const char EQUALITY_START = '=';
const char INEQUALITY_START = '!';
const char STRING_START = '"';
const char CHAR_START = '\'';