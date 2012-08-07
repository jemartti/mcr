#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP


#include "Eigen/Dense"

#include <gtkglmm.h>
#include <gtkmm.h>
#include <vector>


class AppWindow;


// Basic 3D point structure
struct point {
public:
	point()
	{
	}
	point( double x, double y, double z )
	: m_x( x ), m_y( y ), m_z( z )
	{
	}

	double m_x;
	double m_y;
	double m_z;
};


// Basic line structure
struct line {
public:
	line()
	{
	}
	line( std::vector<point>           points,
		  int                          numknots,
		  int				           degree,
		  point                        colour1,
		  point						   colour2,
		  point                        colour3,
		  float                        width1,
		  float                        width2,
		  float                        width3,
		  double                       maxt,
		  int                          maxj,
		  int                          curjp,
		  Eigen::MatrixXf              c_jp,
		  std::vector<Eigen::MatrixXf> di )
	:   m_points( points ),   m_numknots( numknots ), m_degree( degree ),
		m_colour1( colour1 ), m_colour2( colour2 ),   m_colour3( colour3 ),
		m_width1( width1 ),   m_width2( width2 ),     m_width3( width3 ),
		m_maxt( maxt ), m_maxj( maxj ), m_curjp( curjp ), m_c_jp( c_jp ),
		m_di( di )
	{
	}

	std::vector<point>           m_points;
	int                          m_numknots;
	int				             m_degree;
	point                        m_colour1, m_colour2, m_colour3;
	float                        m_width1,  m_width2,  m_width3;
	// Resolution information
	double                       m_maxt;
	int                          m_maxj;
	int                          m_curjp;
	Eigen::MatrixXf              m_c_jp;
	std::vector<Eigen::MatrixXf> m_di;
};


// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
	enum Mode {
		POINTS,
		RESOLUTION,
		COLOUR1,
		COLOUR2,
		COLOUR3,
		WIDTH
	};

	Viewer();
	virtual ~Viewer();

	void set_mode ( Mode mode );
	void set_window( AppWindow* window );
	void set_img( const char* root );

	void adjust_slot();

	void tog_ei();
	void tog_dispp();
	void tog_dispc();
	void tog_dispch();
	void tog_dispi();
	void tog_dispcw();

	void new_line();

	void clear();

	void save();
	void load();

	void invalidate();
  
protected:
	// Called when GL is first initialized
	virtual void on_realize();
	// Called when our window needs to be redrawn
	virtual bool on_expose_event(GdkEventExpose* event);
	// Called when the window is resized
	virtual bool on_configure_event(GdkEventConfigure* event);
	// Called when a mouse button is pressed
	virtual bool on_button_press_event(GdkEventButton* event);
	// Called when a mouse button is released
	virtual bool on_button_release_event(GdkEventButton* event);
	// Called when the mouse moves
	virtual bool on_motion_notify_event(GdkEventMotion* event);

	void draw_image();
	void draw_curve();

private:
	// Application state variables
	bool                         m_ei;
	Mode                         m_mode;
	// Whether or not to display the points, curve, and hull
	bool                         m_dispp, m_dispc, m_dispch, m_dispi, m_dispcw;
	AppWindow*                   m_window;
	const char*                  m_root;

	// Multiple line information
	std::vector<line>            m_lines;
	int                          m_i;

	// Picking information
	int                          m_button;
	bool                         m_move;
	std::vector<point>::iterator m_picked;

	// Width/Colour information
	double                       m_x0;

	void   add_point( int x, int y );
	void   remove_point( std::vector<point>::iterator it );
	void   picking( int x, int y );

	void   bspline( int n, int d, int r, std::vector<point> ipts, point *opts );
	double blend( int i, int d, double t, int *u );
	void   compute_point( int n, int d, int *u, double t,
						  std::vector<point> ipts, point *computed );

	Eigen::MatrixXf get_pi( int i );
	Eigen::MatrixXf get_qi( int i );
	Eigen::MatrixXf get_ii( int i );
	Eigen::MatrixXf get_ji( int i );
	Eigen::MatrixXf get_ai( int i );
	Eigen::MatrixXf get_bi( int i );

	void level_change( double newl );
	void level_down();
	void level_up();
};


#endif
