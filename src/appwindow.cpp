#include "appwindow.hpp"

#include <iostream>


AppWindow::AppWindow( const char* root )
	: m_adjust( 100.0, 0.0, 100.0 ), m_root( root )
{
	set_title( "Multiresolution Curves" );

	// A utility class for constructing things that go into menus, which
	// we'll set up next.
	using Gtk::Menu_Helpers::MenuElem;
	using Gtk::Menu_Helpers::CheckMenuElem;
	using Gtk::Menu_Helpers::RadioMenuElem;
	using Gtk::CheckMenuItem;

	// Set up the application menu
	// The slot we use here just causes AppWindow::hide() on this,
	// which shuts down the application.
	m_menu_app.items().push_back( MenuElem("_Save", Gtk::AccelKey( "s" ),
			sigc::mem_fun( m_viewer, &Viewer::save )) );
	m_menu_app.items().push_back( MenuElem("_Load", Gtk::AccelKey( "l" ),
			sigc::mem_fun( m_viewer, &Viewer::load )) );
	m_menu_app.items().push_back( MenuElem("Cle_ar", Gtk::AccelKey( "a" ),
			sigc::mem_fun( m_viewer, &Viewer::clear )) );
	m_menu_app.items().push_back( MenuElem("_Quit", Gtk::AccelKey( "q" ),
			sigc::mem_fun( *this, &AppWindow::hide )) );

	// Set up the mode menu
	Gtk::RadioButtonGroup m_modegroup;
	sigc::slot1<void, Viewer::Mode> mode_slot =
			sigc::mem_fun( m_viewer, &Viewer::set_mode );
	m_menu_mode.items().push_back( RadioMenuElem(m_modegroup,
			"_Points", Gtk::AccelKey( "p" ),
			sigc::bind( mode_slot, Viewer::POINTS )) );
	m_menu_mode.items().push_back( RadioMenuElem(m_modegroup,
			"_Resolution", Gtk::AccelKey( "r" ),
			sigc::bind( mode_slot, Viewer::RESOLUTION )) );
	m_menu_mode.items().push_back( RadioMenuElem(m_modegroup,
			"Colour1", Gtk::AccelKey( "7" ),
			sigc::bind( mode_slot, Viewer::COLOUR1 )) );
	m_menu_mode.items().push_back( RadioMenuElem(m_modegroup,
			"Colour2", Gtk::AccelKey( "8" ),
			sigc::bind( mode_slot, Viewer::COLOUR2 )) );
	m_menu_mode.items().push_back( RadioMenuElem(m_modegroup,
			"Colour3", Gtk::AccelKey( "9" ),
			sigc::bind( mode_slot, Viewer::COLOUR3 )) );
	m_menu_mode.items().push_back( RadioMenuElem(m_modegroup,
			"_Width", Gtk::AccelKey( "w" ),
			sigc::bind( mode_slot, Viewer::WIDTH )) );

	// Set up the options menu
	m_menu_options.items().push_back( CheckMenuElem("_Endpoint Interpolation",
			Gtk::AccelKey( "e" ),
			sigc::mem_fun( m_viewer, &Viewer::tog_ei )) );
	m_menu_options.items().push_back( CheckMenuElem("Display Points",
			Gtk::AccelKey( "1" ),
			sigc::mem_fun( m_viewer, &Viewer::tog_dispp )) );
	m_menu_options.items().push_back( CheckMenuElem("Display Curve",
			Gtk::AccelKey( "2" ),
			sigc::mem_fun( m_viewer, &Viewer::tog_dispc )) );
	m_menu_options.items().push_back( CheckMenuElem("Display Curve Hull",
			Gtk::AccelKey( "3" ),
			sigc::mem_fun( m_viewer, &Viewer::tog_dispch )) );
	m_menu_options.items().push_back( CheckMenuElem("Display Image",
			Gtk::AccelKey( "4" ),
			sigc::mem_fun( m_viewer, &Viewer::tog_dispi )) );
	m_menu_options.items().push_back( CheckMenuElem("Colour/Width Toggle",
			Gtk::AccelKey( "5" ),
			sigc::mem_fun( m_viewer, &Viewer::tog_dispcw )) );


	// Set up the menu bar
	m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem
			("_Application", m_menu_app) );
	m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem
			("_Mode", m_menu_mode) );
	m_menubar.items().push_back( Gtk::Menu_Helpers::MenuElem
			("_Options", m_menu_options) );

	// Set up our adjustment slider
	m_scale.set_adjustment( m_adjust );
	m_scale.set_draw_value( false );
	m_adjust.signal_value_changed().connect
			( sigc::mem_fun( m_viewer, &Viewer::adjust_slot ) );

	// Pack in our widgets

	// First add the vertical box as our single "top" widget
	add( m_vbox );

	// Put the menubar on the top, and make it as small as possible
	m_vbox.pack_start( m_menubar, Gtk::PACK_SHRINK );

	// Put the viewer below the menubar. pack_start "grows" the widget
	// by default, so it'll take up the rest of the window.
	m_viewer.set_size_request( 300, 300 );
	m_vbox.pack_start( m_viewer );
	m_vbox.pack_start( m_scale, Gtk::PACK_SHRINK );

	show_all();

	// Set default state
	static_cast<CheckMenuItem*>( &m_menu_options.items()[0] )->set_active();
	static_cast<CheckMenuItem*>( &m_menu_options.items()[1] )->set_active();
	static_cast<CheckMenuItem*>( &m_menu_options.items()[2] )->set_active();
	static_cast<CheckMenuItem*>( &m_menu_options.items()[3] )->set_active();
	static_cast<CheckMenuItem*>( &m_menu_options.items()[5] )->set_active();

	m_viewer.set_window( this );
	m_viewer.set_img( m_root );
}

double AppWindow::get_adjustval()
{
	return m_adjust.get_value();
}

bool AppWindow::on_key_press_event( GdkEventKey *ev )
{
	if ( ev->keyval == 65293 )
	{
		m_viewer.new_line();
	}
    return true;
}

bool AppWindow::on_key_release_event( GdkEventKey* /*ev*/ )
{
	return true;
}

