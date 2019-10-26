
/*
	
	kinputfile
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2016-2019 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	ファイル読み込みモジュール
	
	TODO:
	using namespace stdをグローバルにはしない。
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kinputfile_cpp_include_guard
#define kinputfile_cpp_include_guard

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <k09/kutil00.cpp>
#include <k09/kstr00.cpp>


/* ********** Using Directives ********** */

using namespace std;


/* ********** Class Declarations ********** */

class kinputfile;


/* ********** Enum Definitions ********** */


/* ********** Function Declarations ********** */


/* ********** Class Definitions ********** */

class kinputfile {
	
private:
	
	ifstream ifs;
	string fn;
	
public:
	
	kinputfile( void) : ifs(), fn(){}
	kinputfile( const string &);
	~kinputfile( void);
	
	void setFileName( const string &);
	bool open( void);
	void close( void);
	bool readLine( string &);
	void readAllLines( vector <string> &);
	void getSeparatedStrings( vector < vector <string> > &, const string &);
	
//		char ***getSeparatedStrings( const char *sep, int *nlinep, int **ncolpp);
		
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */


/* ********** Definitions of Member Functions ********** */

kinputfile :: kinputfile( const string &n)
{
	
	setFileName( n);
	open();
	
}

kinputfile :: ~kinputfile( void)
{
	
	close();
	
}

void kinputfile :: setFileName( const string &n)
{
	
	fn.assign( n);
	
}

// 正常終了でtrue、異常終了でfalseを返す。
bool kinputfile :: open( void)
{
	
	// このオブジェクトで前に別のファイルをopenしたことがある場合に備えて、clear()する。
	ifs.clear();
	
	// バイナリモードで開かないと、UNIX形式のファイルをうまく扱えない。
	ifs.open( fn.c_str(), ios_base :: in | ios_base :: binary);
	
	if ( ifs.fail() == true){
		alert( "kinputfile :: open()");
		return false;
	}
	
	return true;
	
}

void kinputfile :: close( void)
{
	
	ifs.close();
	
}


// ファイルから1行読み込んで、渡された文字列オブジェクトretに入れる。
// EOFに達したらfalseを返す。
// 3つの改行形式（\r\n、\rのみ、\nのみ）に対応している。
bool kinputfile :: readLine( string &ret)
{
	
	char c;
	stringstream ss;
	bool eof;
	
	if ( ifs.good() == false){
		alert( "kinputfile :: readLine()", "File stream is not good.");
		return false;
	}
	
	ss.str( "");
	ss.clear();
	eof = false;
	
	while ( true){
		
		ifs.get( c);
		
		if ( ifs.eof() == true){
			eof = true;
			break;
		}
		
		// \rを検出した場合。
		// \r\nで改行されている場合と、\rのみで改行されている場合の両方に対応する。
		if ( c == '\r'){
			
			ifs.get( c);
			if ( ifs.eof() == true){
				eof = true;
				break;
			}
			
			// \rの次の文字が\nでないなら、\rのみで改行が表現されている
			if ( c != '\n'){
				ifs.seekg( -1, ios_base :: cur);
			}
			
			break;
			
		}
		
		// \nを検出した場合、無条件にこれは改行。
		if ( c == '\n'){
			
			break;
			
		}
		
		ss << c;
		
	}
	
	ret.assign( ss.str());
	
	return ( !eof);
	
}

// ファイルを全部読み込んで、各行をvector <string> retに入れて返す。
void kinputfile :: readAllLines( vector <string> &ret)
{
	
	string temp;
	bool b;
	
	ret.clear();
	
	ifs.seekg( 0);
	
	do {
		b = readLine( temp);
		ret.push_back( temp);
	} while ( b == true);
	
}

// （現在位置以降の）ファイルを読み込んで、
// 文字列sepで各行を区切って、vector <string>にして、
// それをさらにvectorにしたもの（要素は行数だけある）を
// wordsに入れる。
// 行によって列数がまちまちになっている可能性がある
void
kinputfile :: 
getSeparatedStrings( vector < vector <string> > &ret, const string &sep)
{
	
	vector <string> lines;
	vector <string> tempwords;
	
	ret.clear();
	
	readAllLines( lines);
	
	ret.resize( lines.size());
	
	for ( int i = 0; i < lines.size(); i++){
		tokenize( tempwords, lines[ i], sep);
		ret[ i] = tempwords;
	}
	
}


#endif /* kinputfile_cpp_include_guard */
