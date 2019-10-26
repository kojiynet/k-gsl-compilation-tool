
/*
	kstr
	Ver. k09.00
	
	Written by Koji Yamamoto
	Copyright (C) 2014-2019 Koji Yamamoto
	In using this, please read the document which states terms of use.
	
	String-related Utilities
	
	When functions dealing with characters,
	they are applicable only to ASCII characters.
	
	TODO:
	using namespace stdをグローバルにはしない。
	
*/


/* ********** Preprocessor Directives ********** */

#ifndef kstr_cpp_include_guard
#define kstr_cpp_include_guard

#include <string>
#include <sstream>
#include <vector>

#include <k09/kutil00.cpp>


/* ********** Using Directives ********** */

using namespace std;


/* ********** Type Declarations: enum, class, etc. ********** */

class PairedPos;

/* ********** Function Declarations ********** */

void tokenize( vector <string> &, const string &, const string &);
void extractWords( vector <string> &, const string &, const string &, const string &);
void separateQuote( vector <string> &, vector <bool> &, const string &, const string &, const string &);
void omitCommentBetween( vector <string> &, const vector <string> &, string, string);
void omitCommentToEndl( vector <string> &, const vector <string> &, string);

/* ********** Type Definitions: enum, class, etc. ********** */

// Used in function "omitCommentBetween()"
class PairedPos {
	
public:
	
	int startline, startpos, endline, endpos;
	
	PairedPos( void){}
	PairedPos( int sl0, int sp0, int el0, int ep0)
	: startline( sl0), startpos( sp0), endline( el0), endpos( ep0)
	{}
	
	~PairedPos( void){}
	
};


/* ********** Global Variables ********** */


/* ********** Definitions of Static Member Variables ********** */


/* ********** Function Definitions ********** */

/*
	文字列baseを、区切り文字集合sepによってトークンに分けて、
	文字列ベクターのretに入れる。
*/
void tokenize( vector <string> &ret, const string &base, const string &sep)
{
	
	int p, startp, endp;
	
	p = 0;
	ret.clear();
	
	do {
		
		startp = base.find_first_not_of( sep, p);
		if ( startp == string :: npos)
			break;
		
		endp = base.find_first_of( sep, startp);
		if ( endp == string :: npos)
			endp = base.length();
		
		ret.push_back( string( base.substr( startp, ( endp - startp))));
		
		p = endp + 1;
		
	} while ( p < base.length());
	
}

/*
	文字列sのうち、leftとrightに囲まれた文字列を抽出して、
	stringのvectorとしてretに入れる。
	正確には、leftに含まれるいずれかの文字と、rightに含まれるいずれかの文字に
	囲まれた文字列を抽出する。
	例：行列の"{ 1.0, 2.0}, { 4.0, 5.0}"という表記から、
	　　{}で囲まれた部分を取り出す、とか。
*/
void extractWords(
	vector <string> &ret, const string &s, const string &left,
	const string &right
)
{
	
	int p, startp, endp;
	
	p = 0;
	ret.clear();
	
	while ( true){
		
		if ( p >= s.length())
			break;
		
		startp = s.find_first_of( left, p);
		if ( startp == string :: npos)
			break;
		endp = s.find_first_of( right, startp + 1);
		if ( endp == string :: npos){
			alert( "extractWords()");
			break;
		}
		
		ret.push_back( string( s.substr( startp + 1, ( endp - startp - 1))));
		
		p = endp + 1;
		
	}
	
}

/*
	First, this identifies quatations,
	which are surrounded by "left" and "right", in "str".
	Then, this decomposes "str" into non-quatation parts and quatations,
	and stores them separately into "ret".
	"isQuate" will be a vector parallel to "ret"
	and indicate whether the string is a quatation.
	This ignores incomplete quatations.
	Quatations symbols, i.e. "left" and "right", are omitted from "ret".
*/
void separateQuote(
	vector <string> &ret, vector <bool> &isQuote, const string &str,
	const string &left, const string &right)
{
	
	int p, startp, endp;
	
	p = 0;
	ret.clear();
	isQuote.clear();
	
	while ( true){
		
		if ( p >= str.size())
			break;
		
		startp = str.find_first_of( left, p);
		if ( startp == string :: npos){
			ret.push_back( str.substr( p, string :: npos));
			isQuote.push_back( false);
			break;
		}
		
		endp = str.find_first_of( right, startp + 1);
		if ( endp == string :: npos){
			// There is an incomplete quatation.
			ret.push_back( str.substr( p, string :: npos));
			isQuote.push_back( false);
			break;
		}
		
		// A quatation is found.
		// We store a non-quatation part and a quatation.
		ret.push_back( str.substr( p, startp - p));
		isQuote.push_back( false);
		ret.push_back( str.substr( startp + 1, endp - startp - 1));
		isQuote.push_back( true);
		
		p = endp + 1;
		
	}
	
}

/*
	originで与えられた文字列集合を行の連なりと見なし、
	leftとrightで囲まれた部分を（leftとrightを含めて）コメントとして、
	削除して、retに入れる。
	ネストされたコメントを判別する。
	終端のないコメントは削除しない。
*/
// 例：omitCommentBetween( offcomment, infile, "/*", "*/");
void omitCommentBetween(
	vector <string> &ret, const vector <string> &origin,
	string left, string right
)
{
	
	// LinePosクラスをつくり、lineとposを同時に管理して、
	// 関数で位置関係を比較できるようにする？
	
	vector <int> startline;
	vector <int> startpos;
	vector <int> endline;
	vector <int> endpos;
	vector <PairedPos> pairs;
	vector <int> unpairedsl;
	vector <int> unpairedsp;
	int n;
	int p;
	int sidx, eidx;
	vector <string> mask;
	stringstream ss;
	
	if ( &ret == &origin){
		// two vectors have in fact the same address
		alert( "omitCommentBetween()");
		return;
	}
	
	n = origin.size();
	
	// finding left string
	for ( int i = 0; i < n; i++){
		
		p = 0;
		while ( p < origin[ i].size()){
			
			p = origin[ i].find( left, p);
			if ( p == string :: npos){
				break;
			} else {
				startline.push_back( i);
				startpos.push_back( p);
			}
			p += left.size();
			
		}
		
	}
	
	// finding right string
	for ( int i = 0; i < n; i++){
		
		p = 0;
		while ( p < origin[ i].size()){
			
			p = origin[ i].find( right, p);
			if ( p == string :: npos){
				break;
			} else {
				endline.push_back( i);
				endpos.push_back( p);
			}
			p += right.size();
			
		}
		
	}
	
	// making pairs
	sidx = 0; // index for startline and startpos
	eidx = 0; // index for endline and endpos
	while ( eidx < endline.size()){
		
		if ( unpairedsl.size() < 1){
			
			if ( sidx >= startline.size()){
				
				eidx = endline.size();
				
			} else if ( startline[ sidx] > endline[ eidx]
			            || ( startline[ sidx] == endline[ eidx] && startpos[ sidx] > endpos[ eidx])){
				
				eidx++;
				
			} else {
				
				unpairedsl.push_back( startline[ sidx]);
				unpairedsp.push_back( startpos[ sidx]);
				sidx++;
				
			}
			
		} else {
			
			if ( sidx >= startline.size()
			     || startline[ sidx] > endline[ eidx]
			     || ( startline[ sidx] == endline[ eidx] && startpos[ sidx] > endpos[ eidx])){
				
				pairs.push_back(
				 PairedPos( unpairedsl.back(), unpairedsp.back(), endline[ eidx], endpos[ eidx])
				);
				unpairedsl.erase( unpairedsl.end() - 1);
				unpairedsp.erase( unpairedsp.end() - 1);
				eidx++;
				
			} else {
				
				unpairedsl.push_back( startline[ sidx]);
				unpairedsp.push_back( startpos[ sidx]);
				sidx++;
				
			}
			
		}
		
	}
	
	// specifying part to copy
	mask.clear();
	mask.resize( n);
	for ( int i = 0; i < n; i++){
		mask[ i] = string( origin[ i].size(), 'T');
	}
	
	for ( int pairid = 0; pairid < pairs.size(); pairid++){
		
		for ( int i = 0; i < n; i++){
			for ( p = 0; p < origin[ i].size(); p++){
				if ( pairs[ pairid].startline == pairs[ pairid].endline){
					if ( i == pairs[ pairid].startline){
						if ( p >= pairs[ pairid].startpos && p < pairs[ pairid].endpos + right.size()){
							
							mask[ i][ p] = 'F';
						}
					}
				} else if ( i == pairs[ pairid].startline && p >= pairs[ pairid].startpos){
					
					mask[ i][ p] = 'F';
					
				} else if ( i > pairs[ pairid].startline && i < pairs[ pairid].endline){
					
					mask[ i][ p] = 'F';
					
				} else if ( i == pairs[ pairid].endline && p < pairs[ pairid].endpos + right.size()){
					
					mask[ i][ p] = 'F';
					
				}
			}
		}
		
	}
	
	// making ret
	ret.clear();
	ret.resize( n);
	for ( int i = 0; i < n; i++){
		ss.str( "");
		for ( p = 0; p < origin[ i].size(); p++){
			if ( mask[ i][ p] == 'T'){
				ss << origin[ i][ p];
			}
		}
		ret[ i] = ss.str();
	}
	
}

/*
	originで与えられた文字列集合を行の連なりと見なし、
	leftから行の終端までの部分を（leftを含めて）コメントとして
	削除したものを、retに入れて返す。
*/
// 例：omitCommentToEndl( offcomment, infile, "//");
void omitCommentToEndl(
	vector <string> &ret, const vector <string> &origin,
	string left
)
{
	
	vector <int> startline;
	vector <int> startpos;
	int n;
	int p;
	vector <string> mask;
	stringstream ss;
	
	if ( &ret == &origin){
		// two vectors have in fact the same address
		alert( "omitCommentToEndl()");
		return;
	}
	
	n = origin.size();
	
	// finding left string
	for ( int i = 0; i < n; i++){
		
		p = origin[ i].find( left);
		if ( p != string :: npos){
			startline.push_back( i);
			startpos.push_back( p);
		}
		
	}
	
	// specifying part to copy
	mask.resize( n);
	for ( int i = 0; i < n; i++){
		mask[ i] = string( origin[ i].size(), 'T');
	}
	
	for ( int id = 0; id < startline.size(); id++){
		
		for ( int i = 0; i < n; i++){
			for ( p = 0; p < origin[ i].size(); p++){
				if ( i == startline[ id] && p >= startpos[ id]){
					mask[ i][ p] = 'F';
				}
			}
		}
		
	}
	
	// making ret
	ret.clear();
	ret.resize( n);
	for ( int i = 0; i < n; i++){
		ss.str( "");
		for ( p = 0; p < origin[ i].size(); p++){
			if ( mask[ i][ p] == 'T'){
				ss << origin[ i][ p];
			} else {
				break;
			}
		}
		ret[ i] = ss.str();
	}
	
}


/* ********** Definitions of Member Functions ********** */


#endif /* kstr_cpp_include_guard */
