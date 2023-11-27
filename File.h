#pragma once

#include <string>
#include <set>

using std::string;

class Datei {

	uint64_t size;
	
	string 
		path,
		name;

	//constructor (filename, absolute path, bytes)
	Datei(string,string,uint64_t);

public:

	//Getter
	uint64_t getsize() { return size; }
	string getname() { return name; }
	string getpath() { return path; }
	
	//Gleichheit fuer Sortierung (Namesgleichheit)
	bool operator==(const Datei&) const;
	//Groeﬂer,Kleiner Relation
	bool operator<(const Datei&) const;
	bool operator>(const Datei&) const;
	//echte Gleichheit
	bool isEqual(const Datei&);
	//siehe funct.h
	friend Datei strtofile(string& s, bool duplicate);
};

typedef std::set<Datei> FILESET;