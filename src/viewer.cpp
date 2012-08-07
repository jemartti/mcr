#include "algebra.hpp"
#include "appwindow.hpp"
#include "Eigen/Dense"
#include "includes/SOIL.h"
#include "viewer.hpp"

#include <fstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <math.h>
#include <sstream>
#include <string>

using namespace Eigen;


bool FAST = false;


Viewer::Viewer()
{
	Glib::RefPtr<Gdk::GL::Config> glconfig;

	// Ask for an OpenGL Setup with
	//  - red, green and blue component colour
	//  - a depth buffer to avoid things overlapping wrongly
	//  - double-buffered rendering to avoid tearing/flickering
	glconfig = Gdk::GL::Config::create( Gdk::GL::MODE_RGB   |
									    Gdk::GL::MODE_DEPTH |
									    Gdk::GL::MODE_DOUBLE );
	if ( glconfig == 0 ) {
		// If we can't get this configuration, die
		std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
		abort();
	}

	// Accept the configuration
	set_gl_capability( glconfig );

	// Register the fact that we want to receive these events
	add_events( Gdk::BUTTON1_MOTION_MASK    |
			    Gdk::BUTTON2_MOTION_MASK    |
			    Gdk::BUTTON3_MOTION_MASK    |
			    Gdk::BUTTON_PRESS_MASK      |
			    Gdk::BUTTON_RELEASE_MASK    |
				Gdk::BUTTON_MOTION_MASK		|
				Gdk::POINTER_MOTION_MASK	|
			    Gdk::VISIBILITY_NOTIFY_MASK );

	m_ei            = false;
	m_mode          = POINTS;
	m_dispp         = false;
	m_dispc         = false;
	m_dispch        = false;
	m_dispi         = false;
	m_dispcw        = false;

	line temp       = line();
	temp.m_degree   = 3;
	temp.m_numknots = temp.m_points.size() + temp.m_degree + 1;
	temp.m_colour1  = point( 0.0, 1.0, 0.0 );
	temp.m_colour2  = point( 0.0, 1.0, 0.0 );
	temp.m_colour3  = point( 0.0, 1.0, 0.0 );
	temp.m_width1   = 1.0f;
	temp.m_width2   = 1.0f;
	temp.m_width3   = 1.0f;
	temp.m_maxt     = temp.m_points.size();
	temp.m_maxj     = -1;
	temp.m_curjp    = temp.m_maxj;
	m_lines.push_back( temp );
	m_i             = m_lines.size() - 1;

	m_button        = 0;
	m_move          = false;

	m_x0            = 0;
}

Viewer::~Viewer()
{
}

void Viewer::set_mode( Mode mode )
{
	m_mode = mode;
}
void Viewer::set_window( AppWindow* window )
{
	m_window = window;
}
void Viewer::set_img( const char* root )
{
	m_root = root;
}

void Viewer::adjust_slot()
{
	level_change( m_window->get_adjustval() );
	invalidate();
}

void Viewer::tog_ei()
{
	m_ei = !m_ei;
	invalidate();
}
void Viewer::tog_dispp()
{
	m_dispp = !m_dispp;
	invalidate();
}
void Viewer::tog_dispc()
{
	m_dispc = !m_dispc;
	invalidate();
}
void Viewer::tog_dispch()
{
	m_dispch = !m_dispch;
	invalidate();
}
void Viewer::tog_dispi()
{
	m_dispi = !m_dispi;
	invalidate();
}
void Viewer::tog_dispcw()
{
	m_dispcw = !m_dispcw;
	invalidate();
}

void Viewer::new_line()
{
	if ( m_lines[m_i].m_points.size() )
	{
		line temp       = line();
		temp.m_degree   = 3;
		temp.m_numknots = temp.m_points.size() + temp.m_degree + 1;
		temp.m_colour1  = point( 0.0, 1.0, 0.0 );
		temp.m_colour2  = point( 0.0, 1.0, 0.0 );
		temp.m_colour3  = point( 0.0, 1.0, 0.0 );
		temp.m_width1   = 1.0f;
		temp.m_width2   = 1.0f;
		temp.m_width3   = 1.0f;
		temp.m_maxt     = temp.m_points.size();
		temp.m_maxj     = -1;
		temp.m_curjp    = temp.m_maxj;
		m_lines.push_back( temp );
		m_i             = m_lines.size() - 1;

		m_mode          = POINTS;

		m_button        = 0;
		m_move          = false;

		m_x0            = 0;
	}
}

void Viewer::clear()
{
	// Clear lines
	m_lines.clear();

	// Make our new initial line
	line temp       = line();
	temp.m_degree   = 3;
	temp.m_numknots = temp.m_points.size() + temp.m_degree + 1;
	temp.m_colour1  = point( 0.0, 1.0, 0.0 );
	temp.m_colour2  = point( 0.0, 1.0, 0.0 );
	temp.m_colour3  = point( 0.0, 1.0, 0.0 );
	temp.m_width1   = 1.0f;
	temp.m_width2   = 1.0f;
	temp.m_width3   = 1.0f;
	temp.m_maxt     = temp.m_points.size();
	temp.m_maxj     = -1;
	temp.m_curjp    = temp.m_maxj;
	m_lines.push_back( temp );
	m_i             = m_lines.size() - 1;

	m_mode   = POINTS;

	m_button = 0;
	m_move   = false;

	m_x0     = 0;

	invalidate();
}

void Viewer::save()
{
	std::ofstream pointsfile;
	pointsfile.open( "./data/points" );

	if( pointsfile.is_open() )
	{
		std::vector<line>::iterator jt;
		for( jt = m_lines.begin(); jt!= m_lines.end(); ++jt )
		{
			// m_points
			pointsfile << jt->m_points.size() << std::endl;
			std::vector<point>::iterator it;
			for( it = jt->m_points.begin(); it != jt->m_points.end(); ++it )
			{
				pointsfile << it->m_x << " " << it->m_y << " ";
				pointsfile << it->m_z << std::endl;
			}

			pointsfile << jt->m_numknots << std::endl;
			pointsfile << jt->m_degree << std::endl;
			pointsfile << jt->m_colour1.m_x << " " << jt->m_colour1.m_y;
			pointsfile << " " << jt->m_colour1.m_z << std::endl;
			pointsfile << jt->m_colour2.m_x << " " << jt->m_colour2.m_y;
			pointsfile << " " << jt->m_colour2.m_z << std::endl;
			pointsfile << jt->m_colour3.m_x << " " << jt->m_colour3.m_y;
			pointsfile << " " << jt->m_colour3.m_z << std::endl;
			pointsfile << jt->m_width1 << std::endl;
			pointsfile << jt->m_width2 << std::endl;
			pointsfile << jt->m_width3 << std::endl;
			pointsfile << jt->m_maxt << std::endl;
			pointsfile << jt->m_maxj << std::endl;
			pointsfile << jt->m_curjp << std::endl;

			pointsfile << jt->m_c_jp.rows() << " " << jt->m_c_jp.cols();
			pointsfile << std::endl;
			pointsfile << jt->m_c_jp << std::endl;

			// m_points
			pointsfile << jt->m_di.size() << std::endl;
			std::vector<MatrixXf>::iterator kt;
			for( kt = jt->m_di.begin(); kt != jt->m_di.end(); ++kt )
			{
				pointsfile << kt->rows() << " " << kt->cols();
				pointsfile << std::endl;
				pointsfile << (*kt) << std::endl;
			}
		}
		pointsfile << -100000 << std::endl;
		pointsfile.close();
	}
}
void Viewer::load()
{
	m_lines.clear();
	m_mode   = POINTS;

	m_button = 0;
	m_move   = false;

	m_x0     = 0;

	std::ifstream pointsfile( "./data/points" );
	double x, y, z;

	if( pointsfile.is_open() )
	{
		while( !pointsfile.eof() )
		{
			std::vector<point> points;
			int num;
			pointsfile >> num;
			if ( num != -100000 )
			{
				for( int i = 0; i < num; i += 1 )
				{
					pointsfile >> x >> y >> z;
					points.push_back( point(x, y, z) );
				}

				int numknots;
				pointsfile >> numknots;
				int degree;
				pointsfile >> degree;

				pointsfile >> x >> y >> z;
				point colour1( x, y, z );
				pointsfile >> x >> y >> z;
				point colour2( x, y, z );
				pointsfile >> x >> y >> z;
				point colour3( x, y, z );

				float width1;
				pointsfile >> width1;
				float width2;
				pointsfile >> width2;
				float width3;
				pointsfile >> width3;
				double maxt;
				pointsfile >> maxt;
				int maxj;
				pointsfile >> maxj;
				int curjp;
				pointsfile >> curjp;

				pointsfile >> x >> y;
				Eigen::MatrixXf c_jp( x, y );
				for( int k = 0; k < x; k += 1 )
				{
					for( int l = 0; l < y; l += 1 )
					{
						pointsfile >> z;
						c_jp(k, l) = z;
					}
				}

				double t;
				pointsfile >> t;
				std::vector<Eigen::MatrixXf> di;
				for( int m = 0; m < t; m += 1 )
				{
					pointsfile >> x >> y;
					Eigen::MatrixXf tempm( x, y );
					for( int n = 0; n < x; n += 1 )
					{
						for( int o = 0; o < y; o += 1 )
						{
							pointsfile >> z;
							tempm(n, o) = z;
						}
					}
					di.push_back( tempm );
				}

				line temp       = line( points, numknots, degree, colour1,
										colour2, colour3, width1, width2,
										width3, maxt, maxj, curjp, c_jp, di );
				m_lines.push_back( temp );
			}
		}
		pointsfile.close();
	}
	else
	{
		clear();
	}
	m_i = m_lines.size() - 1;
	invalidate();
}

void Viewer::invalidate()
{
	// Force a rerender
	Gtk::Allocation allocation = get_allocation();
	get_window()->invalidate_rect( allocation, false );
}

void Viewer::on_realize()
{
	// Do some OpenGL setup.
	// First, let the base class do whatever it needs to
	Gtk::GL::DrawingArea::on_realize();

	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if ( !gldrawable )
	{
		return;
	}

	if ( !gldrawable->gl_begin(get_gl_context()) )
	{
		return;
	}

	glShadeModel( GL_SMOOTH );
	glClearColor( 0.4, 0.4, 0.4, 0.0 );
	glEnable( GL_DEPTH_TEST );

	gldrawable->gl_end();
}

bool Viewer::on_expose_event( GdkEventExpose* /*event*/ )
{
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if ( !gldrawable )
	{
		return false;
	}

	if ( !gldrawable->gl_begin(get_gl_context()) )
	{
		return false;
	}

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Set up for perspective drawing
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport( 0, 0, get_width(), get_height() );
	gluPerspective( 40.0, (GLfloat)get_width() / (GLfloat)get_height(),
			        0.1, 1000.0 );

	// change to model view for drawing
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	// Draw stuff
	draw_curve();

	// Swap the contents of the front and back buffers so we see what we
	// just drew. This should only be done if double buffering is enabled.
	gldrawable->swap_buffers();

	gldrawable->gl_end();

	return true;
}

bool Viewer::on_configure_event( GdkEventConfigure* event )
{
	Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

	if ( !gldrawable )
	{
		return false;
	}

	if ( !gldrawable->gl_begin(get_gl_context()) )
	{
		return false;
	}

	// Set up perspective projection, using current size and aspect
	// ratio of display
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport( 0, 0, event->width, event->height );
	gluPerspective( 40.0, (GLfloat)event->width / (GLfloat)event->height,
			        0.1, 1000.0 );

	// Reset to modelview matrix mode
	glMatrixMode( GL_MODELVIEW );

	gldrawable->gl_end();

	return true;
}

bool Viewer::on_button_press_event( GdkEventButton* event )
{
	m_x0 = event->x;

	if ( event->button == 1 )
	{
		m_button = 1;
		if ( m_mode == POINTS )
		{
			add_point( event->x, event->y );
		}
		if ( m_mode == RESOLUTION )
		{
			level_down();
		}
		invalidate();
	}
	if ( event->button == 2 )
	{
		m_button = 2;
		if ( m_mode == POINTS )
		{
			picking ( event->x, event->y );
			if ( m_move )
			{
				remove_point( m_picked );
			}
		}
		if ( m_mode == RESOLUTION )
		{
			level_change( m_lines[m_i].m_curjp * 100.0 / m_lines[m_i].m_maxt );
		}
		invalidate();
	}
	if ( event->button == 3 )
	{
		m_button = 3;
		if ( m_mode == POINTS )
		{
			picking ( event->x, event->y );
		}
		if ( m_mode == RESOLUTION )
		{
			level_up();
		}
		invalidate();
	}

	return true;
}

bool Viewer::on_button_release_event( GdkEventButton* event )
{
	if ( event->button == 1 )
	{
	}
	if ( event->button == 2 )
	{
		m_move = false;
	}
	if ( event->button == 3 )
	{
		if ( m_move )
		{
			m_move = false;
		}
	}

	m_x0     = 0;
	m_button = 0;
	return true;
}

bool Viewer::on_motion_notify_event( GdkEventMotion* event )
{
	double dist = abs(event->x - m_x0);
	if ( m_button == 1 )
	{
		if ( m_mode == COLOUR1 ) m_lines[m_i].m_colour1.m_x =
				dist / (double)get_width();
		if ( m_mode == COLOUR2 ) m_lines[m_i].m_colour2.m_x =
				dist / (double)get_width();
		if ( m_mode == COLOUR3 ) m_lines[m_i].m_colour3.m_x =
				dist / (double)get_width();
		if ( m_mode == WIDTH   ) m_lines[m_i].m_width1      =
				dist / (double)get_width() * 30.0;
		invalidate();
	}
	if ( m_button == 2 )
	{
		if ( m_mode == COLOUR1 ) m_lines[m_i].m_colour1.m_y =
				dist / (double)get_width();
		if ( m_mode == COLOUR2 ) m_lines[m_i].m_colour2.m_y =
				dist / (double)get_width();
		if ( m_mode == COLOUR3 ) m_lines[m_i].m_colour3.m_y =
				dist / (double)get_width();
		if ( m_mode == WIDTH   ) m_lines[m_i].m_width2      =
				dist / (double)get_width() * 30.0;
		invalidate();
	}
	if ( m_button == 3 )
	{
		if ( m_mode == COLOUR1 ) m_lines[m_i].m_colour1.m_z =
				dist / (double)get_width();
		if ( m_mode == COLOUR2 ) m_lines[m_i].m_colour2.m_z =
				dist / (double)get_width();
		if ( m_mode == COLOUR3 ) m_lines[m_i].m_colour3.m_z =
				dist / (double)get_width();
		if ( m_mode == WIDTH   ) m_lines[m_i].m_width2      =
				dist / (double)get_width() * 30.0;
		if ( m_move )
		{
			m_picked->m_x = event->x / get_width();
			m_picked->m_y = event->y / get_height();
		}
		invalidate();
	}

	return true;
}

void Viewer::draw_image()
{
	if ( m_dispi )
	{
		int current_width  = get_width();
		int current_height = get_height();

		GLuint myTexture;
		myTexture = SOIL_load_OGL_texture( m_root, 0, 1,
										   SOIL_FLAG_POWER_OF_TWO |
										   SOIL_FLAG_INVERT_Y );

		glEnable(GL_TEXTURE_2D);

		glLoadIdentity();

		glColor3f( 1.0, 1.0, 1.0 );

		glBindTexture( GL_TEXTURE_2D, myTexture );
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0f, 0.0f );
			glVertex2f( 0.0f, 0.0f );
			glTexCoord2f( 1.0f, 0.0f );
			glVertex2f( current_width, 0.0f );
			glTexCoord2f( 1.0f, 1.0f );
			glVertex2f( current_width, current_height );
			glTexCoord2f( 0.0f, 1.0f );
			glVertex2f( 0.0f, current_height );
		glEnd();
	}
}

void Viewer::draw_curve()
{
	int current_width  = get_width();
	int current_height = get_height();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glViewport( 0, 0, current_width, current_height );
	glOrtho( 0.0, (float)current_width, 0.0, (float)current_height, -0.1,
			 0.1 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable( GL_LIGHTING );
	glEnable( GL_LINE_SMOOTH );
	glColor3f( 1.0, 1.0, 1.0 );
	glScaled( current_width, current_height, 1.0 );
	glTranslated( 0.0, 0.5, 0.0 );
	glScaled( 1.0, -1.0, 1.0 );
	glTranslated( 0.0, -0.5, 0.0 );

	std::vector<line>::iterator it;
	for( it = m_lines.begin(); it != m_lines.end(); ++it ) {
		// Set our pertinent point details
		it->m_numknots = it->m_points.size() + it->m_degree + 1;

		// Set line resolution
		int resolution;
		if ( it->m_points.size() < 4 ) resolution = 0;
		else                           resolution = it->m_numknots * 15;

		// Create output point array
		point *out_pts;
		out_pts = new point[resolution];

		bspline( it->m_numknots, it->m_degree, resolution, it->m_points,
				 out_pts );

		// draw control points
		if ( m_dispp )
		{
			glColor3f( 1.0f, 0.0f, 0.0f );
			glPointSize( 3.0f );
			glBegin( GL_POINTS );
			for ( int i = 0; i < it->m_numknots - it->m_degree - 1; i += 1 )
			{
				glVertex3d( it->m_points[i].m_x,
						    it->m_points[i].m_y,
						    it->m_points[i].m_z );
			}
			glEnd();
		}

		// begin drawing our curve
		if ( m_dispc )
		{
			if ( m_dispcw )
			{
				double t;

				for( int i = 0; i < resolution - 1; i += 1 )
				{
					t = (double)i / resolution;

					// interpolate line width
					glLineWidth( pow(t, 2)         * it->m_width1 +
								 2 *(1 - t) * t    * it->m_width2 +
								 pow(( 1 - t ), 2) * it->m_width3 );

					// interpolate colour
					float xc, yc, zc;
					xc = pow(t, 2)         * it->m_colour1.m_x +
						 2 *(1 - t) * t    * it->m_colour2.m_x +
						 pow(( 1 - t ), 2) * it->m_colour3.m_x;
					yc = pow(t, 2)         * it->m_colour1.m_y +
						 2 *(1 - t) * t    * it->m_colour2.m_y +
						 pow(( 1 - t ), 2) * it->m_colour3.m_y;
					zc = pow(t, 2)         * it->m_colour1.m_z +
						 2 *(1 - t) * t    * it->m_colour2.m_z +
						 pow(( 1 - t ), 2) * it->m_colour3.m_z;
					glColor3f( xc, yc, zc );

					glBegin(GL_LINE_STRIP);
						glVertex3d( out_pts[i].m_x,
									out_pts[i].m_y,
									out_pts[i].m_z );
						glVertex3d( out_pts[i + 1].m_x,
									out_pts[i + 1].m_y,
									out_pts[i + 1].m_z );
					glEnd();
				}
			}
			else
			{
				glColor3f( 0.0f, 1.0f, 0.0f );
				glBegin(GL_LINE_STRIP);
				for( int i = 0; i < resolution; i += 1 )
				{
					glVertex3d( out_pts[i].m_x,
								out_pts[i].m_y,
								out_pts[i].m_z );
				}
				glEnd();
			}
		}

		// draw curve hull
		if ( m_dispch )
		{
			glColor3f( 0.3f, 0.0f, 0.5f );
			glLineWidth( 1.0f );
			glBegin( GL_LINE_STRIP );
			for( int i = 0; i < it->m_numknots - it->m_degree - 1; i += 1 )
			{
				glVertex3d( it->m_points[i].m_x,
							it->m_points[i].m_y,
							it->m_points[i].m_z );
			}
			glEnd();
		}

		delete out_pts;
	}

	draw_image();
}

void Viewer::add_point( int x, int y )
{
	while ( m_lines[m_i].m_curjp < m_lines[m_i].m_maxj )
	{
		level_up();
	}

	double x_t = (double)x / get_width();
	double y_t = (double)y / get_height();
	m_lines[m_i].m_points.push_back( point(x_t, y_t, 0) );

	if ( m_lines[m_i].m_points.size() == pow(2, m_lines[m_i].m_maxj + 1) + 3 )
	{
		m_lines[m_i].m_maxj  += 1;
		m_lines[m_i].m_curjp  = m_lines[m_i].m_maxj;

		m_lines[m_i].m_c_jp   = MatrixXf( pow(2, m_lines[m_i].m_curjp) + 3, 3 );
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_c_jp.block( i, 0, 1, 3 ) <<
					m_lines[m_i].m_points[i].m_x,
					m_lines[m_i].m_points[i].m_y,
					m_lines[m_i].m_points[i].m_z;
		}
	}

	m_lines[m_i].m_numknots = m_lines[m_i].m_points.size() +
			                  m_lines[m_i].m_degree + 1;
	m_lines[m_i].m_maxt     = log( m_lines[m_i].m_points.size() - 3 ) / log(2);
}

void Viewer::remove_point( std::vector<point>::iterator it )
{
	while ( m_lines[m_i].m_curjp < m_lines[m_i].m_maxj )
	{
		level_up();
	}

	m_lines[m_i].m_points.erase( it );

	if ( m_lines[m_i].m_points.size() == pow(2, m_lines[m_i].m_maxj - 1) + 3 )
	{
		m_lines[m_i].m_maxj  -= 1;
		m_lines[m_i].m_curjp  = m_lines[m_i].m_maxj;

		m_lines[m_i].m_c_jp = MatrixXf( pow(2, m_lines[m_i].m_curjp) + 3, 3 );
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_c_jp.block( i, 0, 1, 3 ) <<
					m_lines[m_i].m_points[i].m_x,
					m_lines[m_i].m_points[i].m_y,
					m_lines[m_i].m_points[i].m_z;
		}
	}

	m_lines[m_i].m_numknots = m_lines[m_i].m_points.size() +
							  m_lines[m_i].m_degree + 1;
	m_lines[m_i].m_maxt     = log( m_lines[m_i].m_points.size() - 3 ) / log(2);
}

void Viewer::picking( int x, int y )
{
	int rad = 3;

	std::vector<point>::iterator it;
	for(   it  = m_lines[m_i].m_points.begin();
		   it != m_lines[m_i].m_points.end();
		 ++it ) {
	    if ( abs(x - ( it->m_x * get_width()  )) <= rad &&
	    	 abs(y - ( it->m_y * get_height() )) <= rad )
	    {
	    	m_move   = true;
	    	m_picked = it;
	    	break;
	    }
	}

	if( !m_move )
	{
		int count = 0;
		std::vector<line>::iterator jt;
		for( jt = m_lines.begin(); jt!= m_lines.end(); ++jt )
		{
			std::vector<point>::iterator kt;
			for( kt = jt->m_points.begin(); kt != jt->m_points.end(); ++kt )
			{
				if ( abs(x - ( kt->m_x * get_width()  )) <= rad &&
					 abs(y - ( kt->m_y * get_height() )) <= rad )
				{
					m_move   = true;
					m_picked = kt;
					m_i      = count;
					break;
				}
			}
			if( m_move )
			{
				break;
			}
			count += 1;
		}
	}
}

// number of knots, degree, resolution, input points, output points
void Viewer::bspline( int n, int d, int r, std::vector<point> ipts,
		              point *opts )
{
	// Fill in knot values
	int   *u;
	u = new int[n];
	for ( int i = 0; i < n; i += 1 )
	{
		// If we interpolate endpoints, set the knots accordingly
		if ( m_ei )
		{
			if      ( i <= 3     ) u[i] = 0;
			else if ( i >= n - 4 ) u[i] = n-1;
			else                   u[i] = i;
		}
		else
		{
			u[i] = i;
		}
	}

	point  computed;
	double increment = (double)( u[n - d - 1] - u[d] ) / r;
	double t         = u[d];
	for ( int i = 0; i < r; i += 1 )
	{
		compute_point( n, d, u, t, ipts, &computed );
		opts[i].m_x = computed.m_x;
		opts[i].m_y = computed.m_y;
		opts[i].m_z = computed.m_z;
		t += increment;
	}

	// If we endpoint interpolate, add the extra last point
	if ( m_ei && r > 0 )
	{
		opts[r - 1].m_x = ipts[n - d - 2].m_x;
		opts[r - 1].m_y = ipts[n - d - 2].m_y;
		opts[r - 1].m_z = ipts[n - d - 2].m_z;
	}

	delete u;
}

// basis i, degree, time, knots
double Viewer::blend( int i, int d, double t, int *u )
{
	double value;

	// Base case
	if ( d == 0 )
	{
		if ( (u[i] <= t) && (t < u[i + 1]) )
		{
			value = 1;
		}
		else
		{
			value = 0;
		}
	}
	// Default case (recursive)
	else
	{
		bool t1 = (u[i + d + 1] == u[i + 1]);
		bool t2 = (u[i + d]     == u[i]    );

		// Do division by zero checks
		if ( t1 && t2 )
		{
			value = 0;
		}
		else if ( t2 )
		{
			value = ( u[i + d + 1] - t ) / ( u[i + d + 1] - u[i + 1] ) *
					blend( i + 1, d - 1, t, u );
		}
		else if ( t1 )
		{
			value = ( t - u[i] ) / ( u[i + d] - u[i] ) *
					blend( i, d - 1, t, u );
		}
		else
		{
			value = ( t - u[i] ) / ( u[i + d] - u[i] ) *
					blend( i, d - 1, t, u ) +
					( u[i + d + 1] - t ) / ( u[i + d + 1] - u[i + 1] ) *
					blend( i + 1, d - 1, t, u );
		}
	}

	return value;
}

// number of knots, degree, knots, time, input points, output point
void Viewer::compute_point( int n, int d, int *u, double t,
							std::vector<point> ipts, point *computed )
{
	double bi;

	computed->m_x = 0;
	computed->m_y = 0;
	computed->m_z = 0;

	// Sum S(t)
	for ( int i = 0; i <= n - d - 2; i += 1 )
	{
		// Compute the basis function and add to sum
		bi = blend( i, d, t, u );
		computed->m_x = computed->m_x + ipts[i].m_x * bi;
		computed->m_y = computed->m_y + ipts[i].m_y * bi;
		computed->m_z = computed->m_z + ipts[i].m_z * bi;
	}
}


MatrixXf Viewer::get_pi( int i )
{
	MatrixXf pi( pow(2, i) + 3, pow(2, i - 1) + 3 );

	switch ( i )
	{
	case 1:
		pi << 16, 0, 0, 0,
			  8,  8, 0, 0,
			  0,  8, 8, 0,
			  0,  0, 8, 8,
			  0,  0, 0, 16;
		break;
	case 2:
		pi << 16, 0,  0,  0,  0,
			  8,  8,  0,  0,  0,
			  0,  12, 4,  0,  0,
			  0,  3,  10, 3,  0,
			  0,  0,  4,  12, 0,
			  0,  0,  0,  8,  8,
			  0,  0,  0,  0,  16;
		break;
	default:
		pi.setZero();
		pi.block( 0, 0, 2, 1 ) << 16, 8;
		pi.block( 1, 1, 3, 1 ) << 8,  12, 3;
		pi.block( 2, 2, 4, 1 ) << 4,  11, 8, 2;
		for ( int j = 0; j < pow(2, i - 1) - 3; j += 1 )
		{
			pi.block( 2 * j + 3, j + 3, 5, 1 ) << 2, 8, 12, 8, 2;
		}
		pi.block( pow(2, i) - 3, pow(2, i - 1),     4, 1 ) << 2, 8,  11, 4;
		pi.block( pow(2, i) - 1, pow(2, i - 1) + 1, 3, 1 ) << 3, 12, 8;
		pi.block( pow(2, i) + 1, pow(2, i - 1) + 2, 2, 1 ) << 8, 16;

		break;
	}

	pi *= 1.0 / 16.0;
	return pi;
}

MatrixXf Viewer::get_qi( int i )
{
	MatrixXf qi( pow(2, i) + 3, pow(2, i - 1) );

	switch ( i )
	{
	case 1:
		qi <<  1,
			  -2,
			   3,
			  -2,
			   1;
		qi *= 1.0 / 3.0;
		break;
	case 2:
		qi << -1368,  0,
			   2064,  240,
			  -1793, -691,
			   1053,  1053,
			  -691,  -1793,
			   240,   2064,
			   0,    -1368;
		qi *= 1.0 / 2064.0;
		break;
	case 3:
		qi << -394762.0 / 574765.0,         0,
				  0,                          0,
			   1,                          -7166160.0   / 28124263.0,
				  0,                          0,
			  -33030599.0  / 41383080.0,    333497715.0 / 478112471.0,
				  6908335.0   / 478112471.0,  0,
			   633094403.0 / 1655323200.0, -881412943.0 / 956224942.0,
				 -74736797.0  / 956224942.0,  27877.0     / 1655323200.0,
			  -19083341.0  / 137943600.0,   1,
				  8833647.0   / 28124263.0,  -864187.0    / 413830800.0,
			   4681957.0   / 165532320.0,  -689203555.0 / 956224942.0,
				 -689203555.0 / 956224942.0,  4681957.0   / 165532320.0,
			  -864187.0    / 413830800.0,   8833647.0   / 28124263.0,
				  1,                         -19083341.0  / 137943600.0,
			   27877.0     / 1655323200.0, -74736797.0  / 956224942.0,
				 -881412943.0 / 956224942.0,  633094403.0 / 1655323200.0,
			   0,                           6908335.0   / 478112471.0,
				  333497715.0 / 478112471.0, -33030599.0  / 41383080.0,
			   0,                           0,
				 -7166160.0   / 28124263.0,   1,
			   0,                           0,
				  0,                         -394762.0    / 574765.0;
		break;
	default:
		qi.setZero();
		qi.block( 0, 0, 8,  1 ) << -394762.0    / 574765.0,
									1,
								   -33030599.0  / 41383080.0,
								    633094403.0 / 1655323200.0,
								   -19083341.0  / 137943600.0,
								    4681957.0   / 165532320.0,
								   -864187.0    / 413830800.0,
								    27877.0     / 1655323200.0;
		qi.block( 1, 1, 9,  1 ) << -1050072320.0   / 4096633377.0,
									2096854390.0   / 2989435167.0,
								   -11070246427.0  / 11957740668.0,
								    1,
								   -157389496903.0 / 221218202358.0,
								    1732435193.0   / 5821531641.0,
								   -27809640281.0  / 442436404716.0,
								    171326708.0    / 36869700393.0,
								   -1381667.0      / 36869700393.0;
		qi.block( 2, 2, 10, 1 ) <<  307090.0   / 19335989.0,
								   -6643465.0  / 77343956.0,
								    6646005.0  / 19335989.0,
								   -29839177.0 / 38671978.0,
								    1,
								   -58651607.0 / 77343956.0,
								    6261828.0  / 19335989.0,
								   -13281990   / 19335989.0,
								    98208.0    / 19335989.0,
								   -792.0      / 19335989;
		for ( int j = 0; j < pow(2, i - 1) - 6; j += 1 )
		{
			qi.block( 2 * j + 3, j + 3, 11, 1 ) <<
									   -1.0    / 24264.0,
										31.0   / 6066.0,
									   -559.0  / 8088.0,
									    988.0  / 3033.0,
									   -9241.0 / 12132.0,
									    1,
									   -9241.0 / 12132.0,
									    988.0  / 3033.0,
									   -559.0  / 8088.0,
									    31.0   / 6066.0,
									   -1.0    / 24264.0;
		}
		qi.block( pow(2, i) - 9, pow(2, i - 1) - 3, 10, 1 ) <<
								   -792.0      / 19335989.0,
								    98208.0    / 19335989.0,
								   -13281990   / 19335989.0,
								    6261828.0  / 19335989.0,
								   -58651607.0 / 77343956.0,
								    1,
								   -29839177.0 / 38671978.0,
								    6646005.0  / 19335989.0,
								   -6643465.0  / 77343956.0,
								    307090.0   / 19335989.0;
		qi.block( pow(2, i) - 7, pow(2, i - 1) - 2, 9, 1 ) <<
								   -1381667.0      / 36869700393.0,
								    171326708.0    / 36869700393.0,
								   -27809640281.0  / 442436404716.0,
								    1732435193.0   / 5821531641.0,
								   -157389496903.0 / 221218202358.0,
								    1,
								   -11070246427.0  / 11957740668.0,
								    2096854390.0   / 2989435167.0,
								   -1050072320.0   / 4096633377.0;
		qi.block( pow(2, i) - 5, pow(2, i - 1) - 1, 8, 1 ) <<
								    27877.0     / 1655323200.0,
								   -864187.0    / 413830800.0,
								    4681957.0   / 165532320.0,
								   -19083341.0  / 137943600.0,
								    633094403.0 / 1655323200.0,
								   -33030599.0  / 41383080.0,
								    1,
								   -394762.0    / 574765.0;
		break;
	}

	return qi;
}

MatrixXf Viewer::get_ii( int i )
{
	MatrixXf ii( pow(2, i) + 3, pow(2, i) + 3 );

	if ( i < 3 )
	{
		ii = get_pi( i + 1 ).transpose() * get_ii( i + 1 ) * get_pi( i + 1 );
	}
	else
	{
		ii.setZero();
		ii.block( 0, 0, 4, 1 ) << 1440, 882,  186,  12;
		ii.block( 0, 1, 5, 1 ) << 882,  2232, 1575, 348,  3;
		ii.block( 0, 2, 6, 1 ) << 186,  1575, 3294, 2264, 239,  2;
		ii.block( 0, 3, 7, 1 ) << 12,   348,  2264, 4832, 2382, 240, 2;
		ii.block( 1, 4, 7, 1 ) << 3,    239,  2382, 4832, 2382, 240, 2;
		for ( int j = 5; j < pow(2, i) - 2; j += 1 )
		{
			ii.block( j - 3, j, 7, 1 ) << 2, 240, 2382, 4832, 2382, 240, 2;
		}
		ii.block( pow(2, i) - 5, pow(2, i) - 2, 7, 1 ) <<
				2,  240, 2382, 4832, 2382, 239, 3;
		ii.block( pow(2, i) - 4, pow(2, i) - 1, 7, 1 ) <<
				2,  240, 2382, 4832, 2264, 348, 12;
		ii.block( pow(2, i) - 3, pow(2, i),     6, 1 ) <<
				2,  239, 2264, 3294, 1575, 186;
		ii.block( pow(2, i) - 2, pow(2, i) + 1, 5, 1 ) <<
				3,  348, 1575, 2232, 882;
		ii.block( pow(2, i) - 1, pow(2, i) + 2, 4, 1 ) <<
				12, 186, 882,  1440;
		ii *= 1 / ( pow(2, i) * 10080.0 );
	}

	return ii;
}

MatrixXf Viewer::get_ji( int i )
{
	return get_qi( i + 1 ).transpose() * get_ii( i + 1 ) * get_qi( i + 1 );
}

MatrixXf Viewer::get_ai( int i )
{
	MatrixXf pi = get_pi( i );
	MatrixXf qi = get_qi( i );
	MatrixXf piqi( pow(2, i) + 3, pow(2, i) + 3 );
	piqi << pi, qi;
	piqi        = piqi.inverse();

	MatrixXf ai = piqi.block( 0, 0, pow(2, i - 1) + 3, pow(2, i) + 3 );

	return ai;
}

MatrixXf Viewer::get_bi( int i )
{
	MatrixXf pi = get_pi( i );
	MatrixXf qi = get_qi( i );
	MatrixXf piqi( pow(2, i) + 3, pow(2, i) + 3 );
	piqi << pi, qi;
	piqi        = piqi.inverse();

	MatrixXf bi = piqi.block( pow(2, i - 1) + 3, 0,
							  pow(2, i) - pow(2, i - 1), pow(2, i) + 3 );

	return bi;
}

void Viewer::level_change( double newl )
{
	double newlevel = ( newl * m_lines[m_i].m_maxt ) / 100.0;

	double t;

	if ( newlevel > m_lines[m_i].m_maxj )
	{
		// Do nothing
	}
	else if ( newlevel == m_lines[m_i].m_maxj )
	{
		m_lines[m_i].m_maxt  = m_lines[m_i].m_maxj;

		while( newlevel > m_lines[m_i].m_curjp )
		{
			m_lines[m_i].m_curjp += 1;
			m_lines[m_i].m_c_jp   = get_pi( m_lines[m_i].m_curjp ) *
									m_lines[m_i].m_c_jp +
									get_qi( m_lines[m_i].m_curjp ) *
									m_lines[m_i].m_di.back();
			m_lines[m_i].m_di.pop_back();
		}

		m_lines[m_i].m_points.clear();
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_points.push_back( point(m_lines[m_i].m_c_jp( i, 0 ),
												   m_lines[m_i].m_c_jp( i, 1 ),
												   m_lines[m_i].m_c_jp( i, 2 ))
										   );
		}
	}
	else if ( newlevel == 0 )
	{
		MatrixXf cjm = get_ai( m_lines[m_i].m_curjp ) * m_lines[m_i].m_c_jp;
		m_lines[m_i].m_points.clear();
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp - 1) + 3; i += 1 )
		{
			m_lines[m_i].m_points.push_back( point(cjm( i, 0 ),
												   cjm( i, 1 ),
												   cjm( i, 2 )) );
		}
	}
	else if ( newlevel == m_lines[m_i].m_curjp )
	{
		m_lines[m_i].m_points.clear();
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_points.push_back( point(m_lines[m_i].m_c_jp( i, 0 ),
												   m_lines[m_i].m_c_jp( i, 1 ),
												   m_lines[m_i].m_c_jp( i, 2 ))
										   );
		}
	}
	else if ( newlevel == m_lines[m_i].m_curjp - 1 )
	{
		m_lines[m_i].m_di.push_back( get_bi(m_lines[m_i].m_curjp) *
								     m_lines[m_i].m_c_jp );
		m_lines[m_i].m_c_jp   = get_ai( m_lines[m_i].m_curjp ) *
									    m_lines[m_i].m_c_jp;
		m_lines[m_i].m_curjp -= 1;

		m_lines[m_i].m_points.clear();
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_points.push_back( point(m_lines[m_i].m_c_jp( i, 0 ),
												   m_lines[m_i].m_c_jp( i, 1 ),
												   m_lines[m_i].m_c_jp( i, 2 ))
										   );
		}
	}
	else if ( newlevel > m_lines[m_i].m_curjp )
	{
		while( newlevel > m_lines[m_i].m_curjp )
		{
			m_lines[m_i].m_curjp += 1;
			m_lines[m_i].m_c_jp   = get_pi( m_lines[m_i].m_curjp ) *
									m_lines[m_i].m_c_jp +
									get_qi( m_lines[m_i].m_curjp ) *
									m_lines[m_i].m_di.back();
			m_lines[m_i].m_di.pop_back();
		}
		if ( newlevel == m_lines[m_i].m_curjp )
		{
			m_lines[m_i].m_points.clear();
			for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
			{
				m_lines[m_i].m_points.push_back( point(
						m_lines[m_i].m_c_jp( i, 0 ),
						m_lines[m_i].m_c_jp( i, 1 ),
						m_lines[m_i].m_c_jp( i, 2 )) );
			}
		}
		else
		{
			t = m_lines[m_i].m_curjp - newlevel;
			t = 1 - t;
			MatrixXf cj  = get_ai( m_lines[m_i].m_curjp ) * m_lines[m_i].m_c_jp;
			MatrixXf dj  = get_bi( m_lines[m_i].m_curjp ) * m_lines[m_i].m_c_jp;
			MatrixXf cjt =     get_pi( m_lines[m_i].m_curjp ) * cj +
						   t * get_qi( m_lines[m_i].m_curjp ) * dj;
			m_lines[m_i].m_points.clear();
			for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
			{
				m_lines[m_i].m_points.push_back( point(cjt( i, 0 ),
													   cjt( i, 1 ),
													   cjt( i, 2 )) );
			}
		}
	}
	else if ( newlevel < m_lines[m_i].m_curjp )
	{
		while( newlevel < (m_lines[m_i].m_curjp - 1) )
		{
			m_lines[m_i].m_di.push_back( get_bi(m_lines[m_i].m_curjp) *
										 m_lines[m_i].m_c_jp );
			m_lines[m_i].m_c_jp   = get_ai( m_lines[m_i].m_curjp ) *
									m_lines[m_i].m_c_jp;
			m_lines[m_i].m_curjp -= 1;
		}
		if ( newlevel == m_lines[m_i].m_curjp )
		{
			m_lines[m_i].m_points.clear();
			for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
			{
				m_lines[m_i].m_points.push_back( point(
						m_lines[m_i].m_c_jp( i, 0 ),
						m_lines[m_i].m_c_jp( i, 1 ),
						m_lines[m_i].m_c_jp( i, 2 )) );
			}
		}
		else
		{
			t = m_lines[m_i].m_curjp - newlevel;
			t = 1 - t;
			MatrixXf cj  = get_ai( m_lines[m_i].m_curjp ) * m_lines[m_i].m_c_jp;
			MatrixXf dj  = get_bi( m_lines[m_i].m_curjp ) * m_lines[m_i].m_c_jp;
			MatrixXf cjt =     get_pi( m_lines[m_i].m_curjp ) * cj +
						   t * get_qi( m_lines[m_i].m_curjp ) * dj;
			m_lines[m_i].m_points.clear();
			for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
			{
				m_lines[m_i].m_points.push_back( point(cjt( i, 0 ),
													   cjt( i, 1 ),
													   cjt( i, 2 )) );
			}
		}
	}
	else
	{
		// I don't know.
	}
}

void Viewer::level_down()
{
	if ( m_lines[m_i].m_curjp > 0 )
	{
		MatrixXf cj( pow(2, m_lines[m_i].m_curjp) + 3, 3 );
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			cj.block( i, 0, 1, 3 ) << m_lines[m_i].m_points[i].m_x,
									  m_lines[m_i].m_points[i].m_y,
									  m_lines[m_i].m_points[i].m_z;
		}

		MatrixXf cjm;
		MatrixXf djm;
		if ( FAST )
		{
			// Decomposes in linear time
			MatrixXf rs1 = get_pi( m_lines[m_i].m_curjp ).transpose() *
						   get_ii( m_lines[m_i].m_curjp ) * cj;
			MatrixXf rs2 = get_qi( m_lines[m_i].m_curjp ).transpose() *
						   get_ii( m_lines[m_i].m_curjp ) * cj;
			MatrixXf ls1 = get_ii( m_lines[m_i].m_curjp - 1 );
			MatrixXf ls2 = get_ji( m_lines[m_i].m_curjp - 1 );
			cjm          = ls1.fullPivLu().solve(rs1);
			djm          = ls2.fullPivLu().solve(rs2);
		}
		else
		{
			cjm = get_ai( m_lines[m_i].m_curjp ) * cj;
			djm = get_bi( m_lines[m_i].m_curjp ) * cj;
		}

		m_lines[m_i].m_di.push_back( djm );

		m_lines[m_i].m_curjp -= 1;

		m_lines[m_i].m_points.clear();
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_points.push_back( point(cjm( i, 0 ),
												   cjm( i, 1 ),
												   cjm( i, 2 )) );
		}

		m_lines[m_i].m_numknots = pow( 2, m_lines[m_i].m_curjp ) + 3 +
				                  m_lines[m_i].m_degree + 1;
	}
}

void Viewer::level_up()
{
	if ( m_lines[m_i].m_curjp < m_lines[m_i].m_maxj )
	{
		MatrixXf cjm( pow(2, m_lines[m_i].m_curjp) + 3, 3 );
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			cjm.block( i, 0, 1, 3 ) << m_lines[m_i].m_points[i].m_x,
									   m_lines[m_i].m_points[i].m_y,
									   m_lines[m_i].m_points[i].m_z;
		}

		m_lines[m_i].m_curjp += 1;

		MatrixXf pj  = get_pi( m_lines[m_i].m_curjp );
		MatrixXf qj  = get_qi( m_lines[m_i].m_curjp );
		MatrixXf djm = m_lines[m_i].m_di.back();
		m_lines[m_i].m_di.pop_back();

		MatrixXf cj  = pj * cjm + qj * djm;

		m_lines[m_i].m_points.clear();
		for ( int i = 0; i < pow(2, m_lines[m_i].m_curjp) + 3; i += 1 )
		{
			m_lines[m_i].m_points.push_back( point(cj( i, 0 ),
												   cj( i, 1 ),
												   cj( i, 2 )) );
		}

		m_lines[m_i].m_numknots = pow( 2, m_lines[m_i].m_curjp ) + 3 +
								  m_lines[m_i].m_degree + 1;
	}
}
