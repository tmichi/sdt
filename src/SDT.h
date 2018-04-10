#ifndef __SDT__
#define __SDT__ 1
#include <cstdio>
#include <vector>
#include <vecmath.h>
using namespace kh_vecmath ;
using namespace std;
/*
 * spherical chamfer distance transform
 */
class SDT
{
protected:
	bool _addflag;
	int _latitude;
	int _longitude;
	int _seednum;
	vector<Vector3d> _pos;
	vector<double>   _dist;
	vector<bool>     _binary;
	
	string _input_file;
	string _output_file;
public:
	SDT();
	~SDT();
	virtual bool init(int argc, char** argv);

	virtual bool init(int lat, int lon, int num);
	virtual bool init(string input);

	virtual bool run();
	bool write();

	void usage();
	
	virtual void get_dist(vector<double>& dist);
	virtual void get_boundary(vector<Vector3d>& pnt);

protected:
	virtual bool init_node();
	virtual bool finalize_distance();

	bool set_size(int lat, int lon);
	bool read_png_as_binary(string inputfile, int& latitude, int& longitude, vector<bool>& binary);
	bool save_png(string inputfile, int& latitude, int& longitude, unsigned char** image);

};

#endif //

