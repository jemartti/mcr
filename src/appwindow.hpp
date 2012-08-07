#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP


#include "viewer.hpp"

#include <gtkmm.h>


class AppWindow : public Gtk::Window {
public:
	AppWindow( const char* root );

	double get_adjustval();
  
protected:
	// Handle I/O
	virtual bool on_key_press_event  ( GdkEventKey *ev );
	virtual bool on_key_release_event( GdkEventKey *ev );

private:
	// A "vertical box" which holds everything in our window
	Gtk::VBox       m_vbox;

	// The menubar, with all the menus at the top of the window
	Gtk::MenuBar    m_menubar;
	// Each menu itself
	Gtk::Menu       m_menu_app;
	Gtk::Menu       m_menu_mode;
	Gtk::Menu       m_menu_options;

	// Resolution adjustment slider
	Gtk::Adjustment m_adjust;
	Gtk::HScale     m_scale;

	// The main OpenGL area
	Viewer          m_viewer;

	// The input file, passed to Viewer to be processed
	const char*     m_root;
};


#endif
