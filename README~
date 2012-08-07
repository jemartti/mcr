Name: Jacob Marttinen
Student Number: 20313145
User-id: jemartti
Assignment: A5

The final executable was compiled on this machine: gl20.student.cs

How to invoke my program: Call ./MultiresolutionCurves from the A5 dir. Supply a command line argument with the filename of a .png file to trace. If no argument is supplied, the application will default to /data/img.png

How to use my extra features:
- Points mode: left click adds points, middle click deletes points, right click + drag edits points, enter sets a line and starts a new one
- Resolution mode: left click drops down to the nearest whole level, middle click brings curve to 2^j + 3 points, right click brings curve up a whole level
- Colour modes (1, 2, 3): Change the colour of the line interpolated from the start, middle, and end of the curve respectively. (L, M, R) click + x drag change (R, G, B) channels.
- Width mode: (L, M, R) click + x drag change the width interpolated at the beginning, middle, and end of the curve respectively.
- Bottom slider changes the curve resolution in continuous increments.

I have created the following data files, which are in the data directory:
- img.png, atv.png (sample images to trace)
- pointsatv, pointschess (sample data points for these images, rename to points to get working)

My program assumes the following:

Objectives:
- Create a user interface for the multiresolution curve editor
- Implement the drawing of endpoint-interpolation cubic B-spline curves from input data points
-Allow for dragging the mouse to define a line, and implement functionality to edit the curve by moving control points
- Implement adaptive curve sampling, ensuring a smooth curve
- Extend this smoothing to continuous levels (multiresolution) using wavelet analysis
- Extend the editing of the curves to continuous levels (multiresolution, allowing editing of both the sweep and the details of the curve)
- Extend the editing of the curves to allow direct manipulation of a curve
- Allow for the editing of colour and size dimensions
- Allow for multiple curves to be drawn, allowing for the tracing of an image
- Demonstrate the drawing functionality by approximating a complicated drawing using at least 20 curves and multiple thicknesses/colours
