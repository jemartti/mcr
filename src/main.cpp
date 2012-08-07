#include "appwindow.hpp"

#include <gtkglmm.h>
#include <gtkmm.h>


int main( int argc, char** argv )
{
	// Construct our main loop
	Gtk::Main kit( argc, argv );

	// Initialize OpenGL
	Gtk::GL::init( argc, argv );

	const char* filename = "./data/img.png";
	if ( argc >= 2 )
	{
		filename = argv[1];
	}

	// Construct our (only) window
	AppWindow window( filename );

	// And run the application!
	Gtk::Main::run( window );
}
