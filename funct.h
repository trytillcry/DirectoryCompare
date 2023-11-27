#pragma once
#include "File.h"
#include <algorithm>
#include <fstream>
#include <fcntl.h>
#include <memory>
#include <iostream>
#include <windows.h>
#include <stdexcept>
#include <shlobj_core.h>
#include <sstream>


using std::cout;
using std::wcout;
using std::cin;
using std::endl;

#define charactersize sizeof(char)
#define max_filename_length (1024 / charactersize)
#define min_filename_length (6 / charactersize)

const string batchpath = "filenamelist.bat"; 
unsigned int duplicates1 = 0, duplicates2 = 0;

//Parser to parse the given string to a Datei object. If isdup true it prefix the name with an '?'
///<param name='str'>: String&  PATH|NAME|BYTES</param>
///<param name='isdup'>: duplicate flag</param>
Datei strtofile(string& str, bool isdup = 0){

	const char* p = str.data();
	
	unsigned char field = 1;

	string 
		name = "", 
		path = "";
	
	uint64_t size = 0;

	if (isdup) name = "?";

	while (field<4) {

		if (*p == '|') {
			++p;
			++field;
			continue;
		}

		switch (field) {
			case 1: path += *p; break;
			case 2: name += *p; break;
			case 3: size = std::stoull(string(p)); ++field; break;
			}

		++p;

	}

	return Datei(name, path, size);
}

//Fills the Set with the filenames and returns the number of insertions
///<param name='set'>: pointer to set</param>
///<param name='in'>: input string to parse</param>
///<param name='dup'>: number of duplicates</param>
unsigned int fillset(FILESET* set, const string& in, unsigned int& dup) {

	int linecounter = 0;
	string tmp = "";

	std::stringstream strom = std::stringstream(in, std::ios_base::in);

	while (1) {

		std::getline(strom, tmp);

		if (strom.eof()) break;

		if (!set->insert(strtofile(tmp)).second) { //If Element is a duplicate...
			++dup;
			set->insert(strtofile(tmp, true)); //...add it again with duplicate flag set
		}
		
		++linecounter;
	}
	
	return linecounter;
}

//Counts the occurrences of a given character in a string
///<param name='str'>: Pointer to the input string</param>
///<param name='c'>: char to count</param>
unsigned int counteach(const char* str, const char c) {

	unsigned int returncount = 0;

	while (*str != '\0') {
		
		if (*str == c) 	++returncount;

		++str;
	};

	return returncount;
}

//Determines which elements of pl1 are not in pl2
///<param name='tocheck'>: Set of Files to check</param>
///<param name='cntrl'>: Set of Files to check against pl1</param>
///<param name='difarr'>: Array for adresses of the Missing Files</param>
unsigned int listdiff(const FILESET& tocheck,const FILESET& cntrl, Datei** difarr) {

	FILESET::iterator dat = tocheck.begin();
	unsigned int destpos=0;

	while (dat != tocheck.end()) {
		
		if (cntrl.find(*dat) == cntrl.end()){
			difarr[destpos++] = const_cast<Datei*>(&(*dat)); 
		}
		dat++;
	}

	difarr[destpos] = nullptr;
	return destpos;
}

void listdiffwrap(const FILESET& tocheck, const FILESET& cntrl, Datei** difarr, unsigned int* ndifsptr) {

	*ndifsptr = listdiff(tocheck, cntrl, difarr);
	
}

//Callback Function used in choosefolder()
static int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
		::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
	}
	return 0;
}

//Opens a Window with a Dir Tree, waits for the User to select a directory, returns the absolute path
std::wstring choosefolder(int (CALLBACK* callback)(HWND, UINT, LPARAM, LPARAM), LPWSTR title = NULL) {

	wchar_t  folderpath[MAX_PATH];
	const char* rootp = NULL;

	BROWSEINFO browseinfo = { 0 };
	browseinfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON | BIF_NOTRANSLATETARGETS;
	browseinfo.lpfn = callback;
	browseinfo.lParam = reinterpret_cast<LPARAM>(&rootp);

	PIDLIST_ABSOLUTE pid = SHBrowseForFolderA((LPBROWSEINFOA)&browseinfo);
	SHGetPathFromIDList(pid, (LPWSTR)folderpath);

	return std::wstring(folderpath);
}

//Opens Child Prompt + Pipe and executes the passed command , returns the output as string
string exec(const char * const command) {

	char buffer[128];
	string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command, "r"), _pclose);

	if (!pipe) 	throw std::runtime_error("popen() failed!");

	while (fgets((char*)&buffer, sizeof(buffer) / sizeof(char), pipe.get()) != nullptr) result += (char*)&buffer;

	return result;
}

//Reads all filenames and the number of files, stores these information in a Pair pointed by pairptr
void readcontent(const std::wstring folderpath, std::pair<unsigned int, string>* pairptr) {

	//Array for converted folderpaths
	char path[MAX_PATH];

	//Conversion of the folder names wstring -> cstring 
	#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#endif
	wcstombs(path, folderpath.data(), MAX_PATH);
	#undef _CRT_SECURE_NO_WARNINGS
	
	//Building the command for exec() the folder iteration
	const string command = batchpath + "  \"" + path + "\"";

	//Child Batch retrieves Filenames and writes it in a String
	const string cmdout = exec(command.data());

	//Determines the number of files based on lineseperator count
	const unsigned int nrows = counteach(cmdout.data(), '\n');

	pairptr->first = nrows;
	pairptr->second = cmdout;

	return;
}