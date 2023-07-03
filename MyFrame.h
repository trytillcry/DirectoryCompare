#pragma once
#include "wx/wx.h"
#include "wx/taskbar.h"
#include "wx/bannerwindow.h"
#include "File.h"
class Datei;


class MyFrame : public wxFrame {

	/*------------Eventhandlers------------*/
	
	//When the "End" Button or the "X" was clicked
	template<typename T>
	void closeapp(T&);
	
	//To get additional Fileinfo by mousclick on a listentry
	template <char side>
	void listclickhandler(wxCommandEvent&);
	
	//Nimmt FILESET und eine Zeilennummer und gibt Pfad und Größe der Datei zurück
	std::pair<wxString, wxString> getInfo(Datei**, unsigned int);

public:
	
	//Pointers copied from cmain instance for passing lists to tooltip info
	Datei** dif1 = nullptr;
	Datei** dif2 = nullptr;

	//sizers
	wxBoxSizer* sizer = nullptr;
	wxBoxSizer* topsizer = nullptr;

	//Listboxen
	wxListBox* lbox = nullptr;
	wxListBox* rbox = nullptr;
	
	//Informationen für Taskleisten- und Fenstericon
	const wxString bmpname = "ttcicon.ico";
	const wxIcon icon = wxIcon(bmpname,wxBITMAP_TYPE_ICO);
	wxTaskBarIcon* tbicon = nullptr;

	//Banner (unterer Fensterabschnitt)
	const wxString imgname = "ttclogo.bmp";
	const wxBitmap img = wxBitmap(imgname,wxBITMAP_TYPE_ANY);
	const wxBitmapBundle logo = wxBitmapBundle(img);
	wxBannerWindow* banner = nullptr;

	//Button fuer Programmende
	wxButton* button1 = nullptr;

	//public constructor
	MyFrame();
	
	wxDECLARE_EVENT_TABLE();
};