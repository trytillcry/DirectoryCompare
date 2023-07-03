#pragma once
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <fstream>
#include "File.h"
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

//Parser fuer string zu Datei
///<param name='str'>: String Referenz der Form Pfad|name|Groeﬂe in Bytes</param>
///<param name='isdup'>: Flag Parameter der festlegt dass das Element als Duplikat eingefuegt werden soll</param>
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

//Befuellt das Set mit den Dateinamen und kennzeichnet Duplikate durch Anhaengen von Fragezeichen
//und gibt Anzahl an eingefuegten Elementen an
///<param name='set'>: Pointer zu Zielmenge</param>
///<param name='in'>: zu parsender Inputstring</param>
///<param name='dup'>: Anzahl der Duplikate</param>
unsigned int fillset(FILESET* set, const string& in, unsigned int& dup) {

	int linecounter = 0;
	std::stringstream strom = std::stringstream(in, std::ios_base::in);
	string tmp = "";

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

//Zaehlt alle vorkommen von c in Zeichenfolge str
///<param name='str'>: Anfang der Zeichenfolgy</param>
///<param name='c'>: Zu suchendes Zeichen</param>
unsigned int counteach(const char* str, const char c) {

	unsigned int returncount = 0;

	while (*str != '\0') {
		
		if (*str == c) 	++returncount;

		++str;
	};

	return returncount;
}

//Ueberprueft ob die angegebene Pfadangabe in Anfuehrungszeichen steht und entfernt diese gegebenfalls
///<param name='pathptr'>: Adresse des Pfadstrings</param>
void checkpathstyle(wchar_t* pathptr) {
	int pos = 0;
	if (pathptr[0] == L'\"') {

		while (pos < MAX_PATH && pathptr[pos + 2] != L'\0') {
			pathptr[pos] = pathptr[pos + 1];
			++pos;
		}
		if (pathptr[pos + 1] != L'\"') {
			cout << "Something went wrong with path style in function checkpathstyle()" << endl;
			system("pause");
			exit(EXIT_FAILURE);
		}
		else {
			pathptr[pos] = L'\0';
			return;
		}
	}
	else return;
}

//Ermittelt welche Elemente aus pl1 in pl2 nicht vorhanden sind
///<param name='tocheck'>: Menge der zu kontrollierenden Datein</param>
///<param name='cntrl'>: Menge der Dateien die auf Gleichheit ueberprueft werden</param>
///<param name='difarr'>: Array fuer Adressen der Fehlenden</param>
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

static int CALLBACK BrowseFolderCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED) {
		LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
		::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
	}
	return 0;
}

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

//Oeffnet Child Process + Pipe und fuehrt aus. Speichert return in String
///<param name='batch'>: Pfad zu Batchfile welches ausgefuehrt werden soll</param>
string exec(const char const * batch) {

	char buffer[128];
	string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(batch, "r"), _pclose);

	if (!pipe) 	throw std::runtime_error("popen() failed!");

	while (fgets((char*)&buffer, sizeof(buffer) / sizeof(char), pipe.get()) != nullptr) result += (char*)&buffer;

	return result;
}