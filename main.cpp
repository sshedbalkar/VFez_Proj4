///////////////////////////////////////////////////////////////////////
// Provides the framework a raytracer.
//
// Gary Herron
//
// Copyright © 2012 DigiPen Institute of Technology
////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>

using namespace std;

#ifdef _WIN32
    // Includes for Windows
    #include <windows.h>
    #include <cstdlib>
    #include <limits>
    #include <crtdbg.h>
#else
    // Includes for Linux
    #include <stdlib.h>
#endif

#include "raytrace.h"

// Read a scene file by parsing each line as a command and calling
// scene->Command(...) with the results.
void ReadScene(const string inName, Scene* scene)
{
    ifstream input(inName.c_str());
    if (input.fail()) {
        fprintf(stderr, "File not found: %s\n", inName.c_str());
        exit(-1); }

    // For each line in file
    for (string line; getline(input, line); ) {
        int found;
        // Remove the characters we don't want to parse
        while ((found=line.find_first_of("VQ(,)")) != string::npos)
            line[found] = ' ';

        // Parse the command
        stringstream lineStream(line);
        string command;
        lineStream >> command;

        // Parse as a list of floats
        vector<double> doubles;
        for (double f; lineStream >> f; ) {
            doubles.push_back(f); }

        // (Re)Parse as a list of strings
        stringstream lineStream2(line);
        vector<string> strings;
        for (string s; lineStream2 >> s; ) {
            strings.push_back(s); }

        // Pass the data on
        scene->Command(command, doubles, strings); }

    input.close();
}

// Convert a float to a two byte integer, with high byte first.
void tobytes(unsigned char*& ptr, const double f)
{
    unsigned int i = 65535.0*f;
    if (i>65535) i=65535;
    if (i<0) i=0;
    *ptr++ = i/256;
    *ptr++ = i%256;
}

// Convert a float to a two byte integer, with high byte first.
void tobyte(unsigned char*& ptr, const double f)
{
    unsigned int i = 255.0*f;
    if (i>255) i=255;
    if (i<0) i=0;
    *ptr++ = i/256;
    *ptr++ = i%256;
}

// Write the image as a PPM (16 bit) image.  
void WriteImage(const string outName, const int width, const int height, V* image, const int pass)
{
    FILE *f;
    unsigned char* bytes = new unsigned char[2*3*width*height];
    unsigned char* ptr = bytes;

    // Open file
    f  =  fopen(outName.c_str(), "wb");

    // Write file header
    fprintf(f, "P6\n%d %d\n%d\n", width, height, 65535);
    ptr = bytes;
        
    // Loop through rows top to bottom, then pixels left to right,
    // then R,G,B,  converting each to a two byte quantity.
    for (int y=height-1;  y>=0;  --y) {
        for (int x=0;  x<width;  ++x) {
            V& pixel = image[y*width + x];
            tobytes(ptr, pixel.x/pass);
            tobytes(ptr, pixel.y/pass);
            tobytes(ptr, pixel.z/pass); } }

    // Write bytes
    fwrite(bytes, 1, 2*3*width*height, f);
    fclose(f);
    free(bytes);
}


////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    // Default values for command line arguments
    vector<string> inNames;
    string outName = "";

    Scene* scene = new Scene();

    // Read the command line arguments
    int i = 1;
    while (argc > i) {
        string arg = argv[i++];
        inNames.push_back(arg);
        outName = outName + (outName.size() ? "-" : "") + arg; }

    // Write the image using ppm format (inefficient, but trivially easy)
    outName.replace(outName.size()-3, outName.size(), "ppm");

    // Read the scene, calling scene.Command for each line.
    for (int unsigned i=0;  i<inNames.size();  i++) 
        ReadScene(inNames[i], scene);
    unsigned w = scene->width;
	unsigned h = scene->height;
    // Allocate and clear an image array
    V *image =  new V[w*h];
	V *img2 =   new V[w*h];
    for (unsigned int y=0;  y<h;  y++)
        for (unsigned int x=0;  x<w;  x++)
            image[y*scene->width + x] = V(0,0,0);

    // RayTrace the image
	int pass = 5000, p=0;
	while( ++p < pass )
	{
		scene->TraceImage(image, p);

		// Write the image
		if( p %10 == 0 )
		{
			for (unsigned int y = 0; y < h; ++y) 
			{ 
				for (unsigned int x = 0; x < w; ++x) 
				{
					img2[y*w + x] = image[y*w + x] / (float)p;
				}
			}
			WriteImage(outName, w, h, img2, 1);
		}
	}
	for (unsigned int y = 0; y < h; ++y) 
	{ 
		for (unsigned int x = 0; x < w; ++x) 
		{
			img2[y*w + x] = image[y*w + x] / (float)pass;
		}
	}
	WriteImage(outName, scene->width, scene->height, img2, 1);
}
