#include "File.h"

//Konstruktor
Datei::Datei(const string name, const string path, const uint64_t size){

	this->name = name;
	this->path = path;
	this->size = size;

}

bool Datei::isEqual(const Datei& d) {

	return (*this == d && this->size == d.size) ? true : false;
}
bool Datei::operator==(const Datei& d) const {

	return (this->name == d.name);
}
bool Datei::operator>(const Datei& f) const {

	return (this->name > f.name);
}
bool Datei::operator<(const Datei& f) const {

	return (this->name < f.name);
}