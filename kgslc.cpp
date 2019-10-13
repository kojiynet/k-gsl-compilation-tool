
/*
	kgslc.cpp
	
	k-gsl-compilation-tool
	Ver.0.05 in Development
	
	Copyright (C) 2013-2019 Koji Yamamoto a.k.a. utilcraft
	
	GSLのコンパイル用にMakefileから情報を抽出する。
	いやいや、Unix/Linuxのスクリプトとか、autotoolsとか、GNU makeとか、
	ちょっとわからないんで。。。
	ビルドするのにバッチファイルでよくないでしょうか。。
	っていうコンセプトです。
	
	ToDo:
	
	・GSL2.6のmovstatのMakefile.amに対応していない。空白文字を無視するべし。
	
	・オプション　-Pgsl.lib　とかで、生成されたすべてのライブラリをパックする、とか
	・出力バッチファイル名も指定？ "-Ogslcomp1.bat"
	・コンパイラに渡すオプション "-AOx" Addtional Opition for Compilers
	・自然言語風に出せるオプション？ "-N" （-Pとか-Cとかが無効に）
	・全体的に改良、コメントの改善？
	・リリース記録とか日付とか
	・ディレクトリ名とか、ASCII英数字のみ、という仕様をReadmeに書く
	・Readme以上の内容を、Materialにする。
	
	・XMLで出せるオプション？
	
	【注意】GCCの場合、ライブラリのライブラリをつくるのに、スクリプトが必要になるので、一時ファイルをつくる。
　　　　　　「GCCのとき、batと一緒にできるtmpファイルを消さないでください」と言うべき。
	　　　　GCCのとき、g++ -o test.exe test.cpp libgsl.la libgslcblas.la　とかで使える。
	

*/


/* ********** プリプロセッサ指令・名前空間指定 ********** */

#include <iostream>
#include <fstream>
#include <string>
#include <k09/kutil00.cpp>
#include <k09/kinputfile00.cpp>
#include <k09/koutputfile00.cpp>

using namespace std;


/* ********** 型宣言・クラス宣言 ********** */

class makefile;
class batchfile;


/* ********** 関数宣言 ********** */

int main( int, char *[]);
bool interpretArgs( const clArgs &, string &, string &, compilerId &);


/* ********** 型定義・クラス定義 ********** */

class makefile {
	
	private:
		
		bool ignore;
		bool top;
		
		vector <makefile> submakefiles;
		vector <string> subdirnames;
		
		string wdir;
		string sdir;
		string mydir; // 自分がサブディレクトリにあるmakefileである場合の、
		              // 自分の名
		
		vector <string> includedirs;
		vector <string> csources; // ソースファイルのうち ".c" を除いた部分
		vector <string> libsources;
		string targetlib;
		
		static compilerId compiler;
		
	public:
		
		makefile( void);
		makefile( const makefile &);
		~makefile( void);
		
		makefile & operator=( const makefile &);
		
		bool initialize( bool, const string &, const string &, 
		                 const string & = "");
		void parseLines( const vector <string> &);
		bool pickData( const vector <string> &, const string &, vector <string> &);
		void getSubdirNames( vector <string> &) const;
		
		void getCommands( vector <string> &) const;
		void getCommandsMsc( vector <string> &) const;
		void getCommandsGcc( vector <string> &) const;
		void getSubdirCommands( vector < vector <string> > &,
		                        vector <string> &) const;
		
		static void setCompilerId( compilerId);
		static compilerId getCompilerId( void);
		
};

class batchfile {
	
	private:
		
		string wdir;
		string sdir;
		string filename;
		vector <string> subdirnames;
		
		makefile topmf;
		
	public:
		
		batchfile( void);
		~batchfile( void);
		
		void setDir( const string &, const string &);
		void setFileName( const string &);
		void setTopMakefile( const makefile &);
		bool createBatchFile( void);
		
	
};

/* ********** グローバル変数 ********** */


/* ********** 静的メンバ変数の定義 ********** */

compilerId makefile :: compiler = MSC;


/* ********** 関数定義 ********** */

int main( int argc, char *argv[])
{
	
	clArgs args;
	string wdir, sdir;
	compilerId ci; // compilerId is the enum type defined in kutil.cpp
	bool b;
	makefile topmf;
	batchfile bf;
	
	try {
		
		setAlertExceptOn();
		
		cout << endl <<
			"*****************************" << endl <<
			"kgslc" << endl << 
			"K\'s utility for compiling GSL" << endl <<
			"Version 0.03" << endl << 
			"Written by Koji Yamamoto" << endl <<
			"*****************************" << endl << endl;
		
		args.initialize( argc, argv);
		b = interpretArgs( args, wdir, sdir, ci);
		if ( b == false){
			cout <<
				"Please specify:"
				 << endl << 
				" Source directory with the switch \"-S<dir>\"."
				 << endl << endl <<
				"Optional switches:" << endl << endl <<
				"    \"-W<dir>\" : Specify workspace directory." << endl <<
				"                  (The default is current directory, \".\")" << endl << endl <<
				"    \"-Cm\" : Specify MSC as the compiler to use. (default)" << endl <<
				"    \"-Cg\" : Specify GCC as the compiler to use." << endl << endl;
			return 0;
		}
		
		cout <<
			"Specified source directory: " << sdir << endl << 
			"Specified workspace directory: " << wdir << endl;
		
		if ( ci == MSC){
			cout << "Specified compiler: MSC" << endl << endl;
		} else if ( ci == GCC){
			cout << "Specified compiler: GCC" << endl << endl;
		}
		
		makefile :: setCompilerId( ci);
		
		b = topmf.initialize( true, wdir, sdir);
		if ( b == false){
			cout << "At least one of Makefiles can not be read correctly." << endl
			     << "The name of speficied directory might not be correct." << endl << endl;
			return 1;
		}
		
		bf.setDir( wdir, sdir);
		bf.setFileName( "kgslc.bat");
		bf.setTopMakefile( topmf);
		
		b = bf.createBatchFile();
		if ( b == false){
			cout << "The name of speficied directory might not be correct." << endl << endl;
			return 1;
		}
		
	} catch ( exception &e){
		
		cout << "Error occurred:" << endl
		     << " " << e.what() << endl;
		
		return 1;
		
	} catch ( ...){
		
		cout << "Error occurred." << endl;
		
		return 1;
		
	}
	
	return 0;
	
}

/*
	オプション'S'で指定されるソースファイルディレクトリ名をsに入れる。
	（ソースファイルディレクトリとして指定されるべきなのは、トップレベルのMakefile.amがあるところ）
	オプション'W'で指定されるワークスペースディレクトリ名をwに入れる。
	省略されるとカレントディレクトリである"."となる。
	ディレクトリ名からは最後の'\\'を削除しておく。
	'S'のオプションが1つだけあればtrue、なければfalseを返す。
	オプション'C'で指定されるコンパイラのIDをciに入れる。
	　"-Cm"か"-CM"が指定されればMSC
	　"-Cg"か"-CG"が指定されればGCC
	とする。
	その他の'C'オプションは無視する。デフォルトはMSC。
*/
bool interpretArgs( const clArgs &ca, string &w, string &s, compilerId &ci)
{
	
	vector <string> temp;
	int pos;
	
	if ( ca.countArgsCns( 'S') != 1)
		return false;
	
	if ( ca.countArgsCns( 'W') > 1){
		cout << "The switch \"-W\" was specified too many times and ignored." << endl << endl;
	} else if ( ca.countArgsCns( 'W') == 1){
		ca.getStringsCns( temp, 'W');
		w.assign( temp[ 0]);
		if ( w[ w.length() - 1] == '\\'){
			w.erase( w.length() - 1, 1);
		}
	} else {
		w.assign( ".");
	}
	
	ca.getStringsCns( temp, 'S');
	s.assign( temp[ 0]);
	if ( s[ s.length() - 1] == '\\'){
		s.erase( s.length() - 1, 1);
	}
	
	ci = MSC;
	if ( ca.countArgsCns( 'C') == 1){
		ca.getStringsCns( temp, 'C');
		if ( temp.size() == 1 && temp[ 0].length() == 1){
			
			if ( temp[ 0].compare( 0, 1, "m") == 0
			     || temp[ 0].compare( 0, 1, "M") == 0){
			
				ci = MSC;
				
			} else if ( temp[ 0].compare( 0, 1, "g") == 0
			            || temp[ 0].compare( 0, 1, "G") == 0){
				
				ci = GCC;
				
			} else {
				
				cout << "The argument with \"-C\" is invalid and ignored." << endl << endl;
				
			}
			
		} else {
			
			cout << "The argument with \"-C\" is invalid and ignored." << endl << endl;
			
		}
	}
	
	return true;
	
}


/* ********** クラスメンバ関数定義 ********** */

/* ********** makefileクラスのメンバ関数 ********** */

makefile :: makefile( void)
 : ignore( false), top( true), submakefiles(), subdirnames(),
   wdir(), sdir(), mydir(),
   includedirs(), csources(), libsources(), targetlib()
{}

makefile :: makefile( const makefile &obj)
{
	
	ignore = obj.ignore;
	top = obj.top;
	submakefiles = obj.submakefiles;
	subdirnames = obj.subdirnames;
	wdir = obj.wdir;
	sdir = obj.sdir;
	mydir = obj.mydir;
	includedirs = obj.includedirs;
	csources = obj.csources;
	libsources = obj.libsources;
	targetlib = obj.targetlib;
	
}

makefile :: ~makefile( void){}

makefile & makefile :: operator=( const makefile &obj)
{
	
	ignore = obj.ignore;
	top = obj.top;
	submakefiles = obj.submakefiles;
	subdirnames = obj.subdirnames;
	wdir = obj.wdir;
	sdir = obj.sdir;
	mydir = obj.mydir;
	includedirs = obj.includedirs;
	csources = obj.csources;
	libsources = obj.libsources;
	targetlib = obj.targetlib;
	
	return ( *this);
	
}

// Makefile.amファイルを読み込む。
// tは、トップレベルのmakefileのとき、true、SUBDIRのとき、false
// wはワークスペースディレクトリ名
// sはソースディレクトリ名
// myは自分がサブディレクトリの中のmakefileの場合の、そのディレクトリ名。
// 　デフォルト引数は""。tがtrueなら無視される。
// 正常終了でtrue、異常終了でfalseを返す。
bool makefile :: initialize( bool t, const string &w, const string &s,
                             const string &my)
{
	
	string fn;
	kinputfile kif;
	vector <string> sv;
	makefile tempmf;
	bool b;
	
	ignore = false;
	top = t;
	wdir.assign( w);
	sdir.assign( s);
	
	includedirs.clear();
	includedirs.push_back( wdir);
	includedirs.push_back( sdir);
	
	if ( top == true){
		fn = s + "\\Makefile.am";
	} else {
		mydir.assign( my);
		includedirs.push_back( sdir + "\\" + mydir);
		fn = s + "\\" + mydir + "\\Makefile.am";
	}
	
	kif.setFileName( fn);
	b = kif.open();
	if ( b == false){
		return false;
	}
	kif.readAllLines( sv);
	kif.close();
	
	parseLines( sv);
	
	// subdirがある場合、さらにオブジェクトを生成する
	if ( subdirnames.empty() == false){
		for ( int i = 0; i < subdirnames.size(); i++){
			if ( subdirnames[ i] != string( ".")){
				if ( top == false){
					subdirnames[ i] = mydir + "\\" + subdirnames[ i];
				}
				b = tempmf.initialize( false, wdir, sdir, subdirnames[ i]);
				if ( b == false){
					return false;
				}
			}
			submakefiles.push_back( tempmf);
		}
	}
	
	return true;
	
}

void makefile :: parseLines( const vector <string> &lines)
{
	
	int pos;
	vector <string> tempv;
	string ulname;
	bool b;
	
	pickData( lines, "SUBDIRS", subdirnames);
	
	b = pickData( lines, "lib_LTLIBRARIES", tempv);
	if ( b == false){
		pickData( lines, "noinst_LTLIBRARIES", tempv);
	}
	
	if ( tempv.size() > 1){
		cout << "Warning: There are two or more target libs in one of Makefiles." << endl;
		targetlib.assign( tempv[ 0]);
	} else if ( tempv.size() < 1) {
		ignore = true;
		return;
	} else {
		targetlib.assign( tempv[ 0]);
	}
	
	// "libgsl.la"を"libgsl_la"にする、など。
	ulname.assign( targetlib);
	pos = ulname.find( ".");
	if ( pos != string :: npos){
		ulname[ pos] = '_';
	}
	
	pickData( lines, ulname + "_SOURCES", csources);
	for ( int i = 0; i < csources.size(); i++){
		
		pos = csources[ i].find( ".c");
		if ( pos != string :: npos){
			csources[ i].erase( pos);
		} else {
			
			pos = csources[ i].find( ".h");
			if ( pos != string :: npos){
				csources.erase( csources.begin() + i);
				i--;
			}
			
		}
		
	}
	
	// "xxx_LIBADD"の指定にある"$(SUBLIBS)"を拾う。
	// "$(SUBLIBS)"以外の指定は無視する。
	// ※トップレベルのMakefile.amで"$(GSL_LIBADD)"が指定されていて、
	// 　これはcblasのことらしい。configureファイルに記載があった。
	// ※utilのMakefile.amで"@LIBOBJS@"が指定されているが、これはわからない。
	if ( pickData( lines, ulname + "_LIBADD", tempv) == true){
		for ( int j = 0; j < tempv.size(); j++){
			if ( tempv[ j].compare( "$(SUBLIBS)") == 0){
				
				pickData( lines, "SUBLIBS", libsources);
				
				// "xxx/yyy.la"という形式を"yyy.la"にする。
				for ( int i = 0; i < libsources.size(); i++){
					pos = libsources[ i].find( "/");
					if ( pos != string :: npos){
						libsources[ i].erase( 0, pos + 1);
					}
				}
			}
		}
		
	}
	
}

// ファイルの内容を表現するlinesから、keyで始まる行を探して、
// keyに該当する情報をretに入れて返す。
// 該当するkeyがなければfalseを返す。
bool makefile :: pickData( const vector <string> &lines, const string &key, vector <string> &ret)
{
	
	int n, keylen;
	stringstream ss;
	string temp;
	
	ret.clear();
	n = lines.size();
	keylen = key.length();
	
	for ( int i = 0; i < n; i++){
		
		if ( lines[ i].length() >= keylen && lines[ i].compare( 0, keylen, key) == 0){
			ss.str( lines[ i]);
			do {
				temp.assign( "");
				ss >> temp;
					if ( temp.length() > 0 
				     && ( temp.length() < keylen || temp.compare( 0, keylen, key) != 0)
				     && temp.compare( 0, 1, "=") != 0){
					
					ret.push_back( temp);
					
				}
			} while ( ss.eof() == false);
			break;
		}
	}
	
	if ( ret.size() == 0)
		return false;
	
	return true;
	
}

void makefile :: getSubdirNames( vector <string> &ret) const
{
	
	ret = subdirnames;
	
}

void makefile :: getCommands( vector <string> &ret) const
{
	
	if ( compiler == MSC){
		getCommandsMsc( ret);
		return;
	} else if ( compiler == GCC){
		getCommandsGcc( ret);
		return;
	}
	
	alert( "makefile :: getCommands()");
	
}

// MSC用のコマンドを得る。
void makefile :: getCommandsMsc( vector <string> &ret) const
{
	
	string temp;
	int pos;
	
	ret.clear();
	
	/* ********** コンパイル命令 ********** */
	
	temp = "cl -c";
	
	for ( int i = 0; i < includedirs.size(); i++){
		temp += " -I" + includedirs[ i];
	}
	
	for ( int i = 0; i < csources.size(); i++){
		temp += " " + sdir + "\\";
		if ( top == false)
			temp += mydir + "\\";
		temp += csources[ i];
		temp += ".c";
	}
	
	ret.push_back( temp);
	
	
	/* ********** ライブラリ化命令 ********** */
	
	temp = "lib";
	
	for ( int i = 0; i < csources.size(); i++){
		temp += " " + csources[ i] + ".obj";
	}
	
	for ( int i = 0; i < libsources.size(); i++){
		temp += " " + libsources[ i];
	}
	
	temp += " -out:" + targetlib;
	
	ret.push_back( temp);
	
	
	/* ********** 中間生成ファイルの削除命令 ********** */
	
	temp = "del";
	
	for ( int i = 0; i < csources.size(); i++){
		temp += " " + csources[ i] + ".obj";
	}
	
	for ( int i = 0; i < libsources.size(); i++){
		temp += " " + libsources[ i];
	}
	
	ret.push_back( temp);
	
}

// GCC用のコマンドを得る。
// ライブラリにライブラリを追加する場合、arコマンドにスクリプトファイルを
// 与えないといけないので、一時ファイルを作成する。
void makefile :: getCommandsGcc( vector <string> &ret) const
{
	
	string temp;
	int pos;
	
	string tfname; // tf = tempfile
	static int tfcount = 0;
	vector <string> tflines;
	string tfline;
	koutputfile kof;
	
	ret.clear();
	tfname.clear();
	
	/* ********** コンパイル命令 ********** */
	
	temp = "gcc -c";
	
	for ( int i = 0; i < includedirs.size(); i++){
		temp += " -I" + includedirs[ i];
	}
	
	for ( int i = 0; i < csources.size(); i++){
		temp += " " + sdir + "\\";
		if ( top == false)
			temp += mydir + "\\";
		temp += csources[ i];
		temp += ".c";
	}
	
	ret.push_back( temp);
	
	
	/* ********** ライブラリ化命令 ********** */
	
	/* ********** すでに作成されたライブラリを含めるかどうかで分岐する ********** */
	
	if ( libsources.size() > 0){
		
		
		/* ********** arコマンドのためのスクリプトファイルの作成 ********** */
		
		tflines.clear();
		
		tfline = "create " + targetlib;
		tflines.push_back( tfline);
		
		if ( csources.size() > 0){
			for ( int i = 0; i < csources.size(); i++){
				tfline = "addmod ";
				tfline += csources[ i] + ".o";
				tflines.push_back( tfline);
			}
		}
		
		if ( libsources.size() > 0){
			for ( int i = 0; i < libsources.size(); i++){
				tfline = "addlib ";
				tfline += libsources[ i];
				tflines.push_back( tfline);
			}
		}
		
		tflines.push_back( "save");
		tflines.push_back( "end");
		
		string s0;
		intToString( s0, tfcount, 2);
		temp = "tmp" + s0;
		getTempFileName( tfname, temp, "txt", 2);
		tfcount++;
		
		kof.setFileName( tfname);
		
		kof.open( false, false, false); // すでにファイルが存在する場合にはエラー。
		
		kof.writeLines( tflines);
		
		kof.close();
		
		/* ********** arコマンド ********** */
		
		temp = "ar -M < " + tfname;
		
		ret.push_back( temp);
		
		
	} else {
		
		temp = "ar -rs ";
		temp += targetlib;
		
		for ( int i = 0; i < csources.size(); i++){
			temp += " " + csources[ i] + ".o";
		}
		
		ret.push_back( temp);
		
	}
	
	
	/* ********** 中間生成ファイルの削除命令 ********** */
	
	temp = "del";
	
	for ( int i = 0; i < csources.size(); i++){
		temp += " " + csources[ i] + ".o";
	}
	
	for ( int i = 0; i < libsources.size(); i++){
		temp += " " + libsources[ i];
	}
	
	if ( tfname.empty() == false){
		temp += " ";
		temp += tfname;
	}
	
	ret.push_back( temp);
	
}

void makefile :: getSubdirCommands( vector < vector <string> > &ret,
                                    vector <string> &dirnames) const
{
	
	vector <string> temp;
	
	ret.clear();
	dirnames.clear();
	
	for ( int i = 0; i < submakefiles.size(); i++){
		
		if ( submakefiles[ i].ignore == false){
			
			submakefiles[ i].getCommands( temp);
			ret.push_back( temp);
			dirnames.push_back( subdirnames[ i]);
			
		}
		
	}

}

// static member function
void makefile :: setCompilerId( compilerId ci)
{
	
	compiler = ci;
	
}

// static member function
compilerId makefile :: getCompilerId( void)
{
	
	return compiler;
	
}


/* ********** makefileクラスのメンバ関数 ********** */

batchfile :: batchfile( void) : wdir(), filename(), subdirnames(), topmf(){}

batchfile :: ~batchfile( void){}

void batchfile :: setDir( const string &w, const string &s)
{
	
	wdir.assign( w);
	sdir.assign( s);
	
}

void batchfile :: setFileName( const string &f)
{
	
	filename.assign( f);
	
}

void batchfile :: setTopMakefile( const makefile &m)
{
	
	topmf = m;
	topmf.getSubdirNames( subdirnames);
	
}

bool batchfile :: createBatchFile( void)
{
	
	compilerId ci;
	vector <string> lines;
	vector <string> temp;
	vector < vector <string> > tempvv; // <vector <string>>としてしまうと、演算子>>と間違われてエラーになりうる。
	vector <string> dirv;
	string fn;
	koutputfile kof;
	int n;
	bool b;
	
	
	lines.clear();
	
	ci = makefile :: getCompilerId();
	
	
	/* ********** バッチファイル：最初のコメント ********** */
	
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "rem * This file was created by \"kgsl\" - K\'s Utility for Compiling GSL");
	lines.push_back( "rem *");
	lines.push_back( "rem * Before running this batch file, please make sure you copy \"config.h.in\"");
	lines.push_back( "rem * file to the workspace directory, and, rename it \"config.h\", and, ");
	lines.push_back( "rem * modify it to suit your environment.");
	lines.push_back( "rem *");
	lines.push_back( "rem * Please read readme.txt for more detail.");
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "");
	
	
	/* ********** バッチファイル：設定に関する記述 ********** */
	
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "rem * This file was created with the following settings:");
	if ( ci == MSC){
		lines.push_back( "rem *  Compiler            = MSC");
	} else if ( ci == BCC){
		lines.push_back( "rem *  Compiler            = BCC");
	} else if ( ci == DMC){
		lines.push_back( "rem *  Compiler            = DMC");
	} else if ( ci == GCC){
		lines.push_back( "rem *  Compiler            = GCC");
	}
	lines.push_back( "rem *  Workspace Directory = " + wdir);
	lines.push_back( "rem *  Source Directory    = " + sdir);
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "");
	
	
	/* ********** バッチファイル：ヘッダファイルのコピー命令 ********** */
	
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "rem Making new directory for header files and copy them there.");
	lines.push_back( "mkdir " + wdir + "\\gsl");
	lines.push_back( "copy " + sdir + "\\gsl*.h " + wdir + "\\gsl\\");
	for ( int i = 0; i < subdirnames.size(); i++){
		lines.push_back( "copy " + sdir + "\\" + subdirnames[ i] + "\\gsl*.h " + wdir + "\\gsl\\");
	}
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "");
	
	
	/* ********** バッチファイル：サブディレクトリに関する命令 ********** */
	
	topmf.getSubdirCommands( tempvv, dirv);
	for ( int i = 0; i < tempvv.size(); i++){
		
		lines.push_back( "rem **************************************************************************");
		lines.push_back( "rem Compiling/Library-Building etc.");
		lines.push_back( "rem For Subdirectory: \"" + dirv[ i] + "\"");
		
		for ( int j = 0; j < tempvv[ i].size(); j++){
			lines.push_back( tempvv[ i][ j]);
		}
		
		lines.push_back( "rem **************************************************************************");
		lines.push_back( "");
		
	}
	
	
	/* ********** バッチファイル：トップディレクトリに関する命令 ********** */
	
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "rem Compiling/Library-Building etc.");
	lines.push_back( "rem For Top directory");
	topmf.getCommands( temp);
	for ( int i = 0; i < temp.size(); i++){
		lines.push_back( temp[ i]);
	}
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "");
	
	
	/* ********** バッチファイル：config.hをヘッダディレクトリにコピーする ********** */
	
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "rem Copying \"config.h\" to header-files directory.");
	lines.push_back( "copy " + wdir + "\\config.h " + wdir + "\\gsl\\");
	lines.push_back( "rem **************************************************************************");
	lines.push_back( "");
	
	
	/* ********** バッチファイルの書き出し ********** */
	
	fn.assign( wdir).append( "\\").append( filename);
	
	kof.setFileName( fn);
	
	// すでに同じファイル名のファイルがある場合にはaskするモードでオープン
	b = kof.open( false, false, true);
	if ( b == false){
		cout << "Tried to create batch file \"" << fn << "\"," << endl
		     << "but it might not have been made due to some error." << endl << endl;
		return  false;
	}
	
	b = kof.writeLines( lines);
	
	kof.close();
	
	if ( b == false){
		cout << "Tried to create batch file \"" << fn << "\"," << endl
		     << "but it might not have been made due to some error." << endl << endl;
		return false;
	}
	
	cout << "Created batch file: " << fn << endl << endl;
	if ( ci == GCC){
		cout << "!!Caution!!" << endl << 
		        "Please do not erase \"tmp\" files before running the batch file." << endl << endl;
	}
	
	return true;
	
}

