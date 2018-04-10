#define _USE_MATH_DEFINES
#include "SDT.h"
#include "Option.h"
#include "dynpq.h"
#include <png.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
using namespace std;
SDT::SDT()
{
	_latitude = 100;
	_longitude = 100;
	_seednum = 10;
	_addflag = false;

};
SDT::~SDT()
{


};
bool 
SDT::init(int argc, char** argv)
{
	_output_file = string(argv[argc-1]);
	_addflag = ::getopt(argc, argv, "-a");
	if(::getopt(argc,argv,"-i")  == true)
	{
		char input[80];
		if(::getopt(argc,argv, "-i", input,true) == false)return false;
		if(this->read_png_as_binary(input,   _latitude,_longitude,_binary) == false)return false;
		int size = _latitude * _longitude;
		_dist.assign(size, 10000);
	}
	else 
	{
		int lat, lon;
		::getopt(argc, argv, "-lat", &lat);
		::getopt(argc, argv, "-lon", &lon);
		if(set_size(lat,lon) == false) return false;
		::getopt(argc, argv, "-n",   &_seednum);
		if(_seednum < 1 ) 
		{
			fprintf(stderr, "Error : invalide seed num size %d", _seednum);
			return false;
		}

		_binary.assign(_latitude * _longitude, true);
		srand(10);
		//initialization
		for(int i = 0 ; i < _seednum ; i++)
		{
			int x = rand() % _longitude;
			int y = rand() % _latitude;
			int id = y * _longitude + x ; 
			_binary[id] = false; //vdt, cdt
		}
	}

	// create position;
	_pos.reserve(_latitude * _longitude);
	for(int j = 0 ; j< _latitude ; j += 1)
	{
		for(int i = 0 ; i < _longitude ; i += 1)
		{
			double lonpitch = 2 * M_PI / _longitude;
			double latpitch = M_PI / (_latitude-1);

			double cosa = cos( i * lonpitch);
			double sina = sin( i * lonpitch);
			double cosb = cos( -j * latpitch + M_PI_2);
			double sinb = sin( -j * latpitch + M_PI_2);
			_pos.push_back(Vector3d(cosb*cosa,cosb*sina,sinb));
		}
	}
	
	init_node();
	return true;	
};

bool 
SDT::init(int lat, int lon, int num)
{
		if(set_size(lat,lon) == false) return false;
		_seednum = num;
		if(_seednum < 1 ) 
		{
			fprintf(stderr, "Error : invalide seed num size %d", _seednum);
			return false;
		}

		_binary.assign(_latitude * _longitude, true);
		//srand(10);
		//initialization
		for(int i = 0 ; i < _seednum ; i++)
		{
			int x = rand() % _longitude;
			int y = rand() % _latitude;
			int id = y * _longitude + x ; 
			_binary[id] = false; //vdt, cdt
		}

	// create position;
	_pos.reserve(_latitude * _longitude);
	for(int j = 0 ; j< _latitude ; j += 1)
	{
		for(int i = 0 ; i < _longitude ; i += 1)
		{
			double lonpitch = 2 * M_PI / _longitude;
			double latpitch = M_PI / (_latitude-1);

			double cosa = cos( i * lonpitch);
			double sina = sin( i * lonpitch);
			double cosb = cos( -j * latpitch + M_PI_2);
			double sinb = sin( -j * latpitch + M_PI_2);
			_pos.push_back(Vector3d(cosb*cosa,cosb*sina,sinb));
		}
	}
	
	init_node();
	return true;	

}

bool 
SDT::init(string input)
{
	if(this->read_png_as_binary(input,   _latitude,_longitude,_binary) == false)return false;
	int size = _latitude * _longitude;
	_dist.assign(size, 10000);
	
	// create position;
	_pos.reserve(_latitude * _longitude);
	for(int j = 0 ; j< _latitude ; j += 1)
	{
		for(int i = 0 ; i < _longitude ; i += 1)
		{
			double lonpitch = 2 * M_PI / _longitude;
			double latpitch = M_PI / (_latitude-1);

			double cosa = cos( i * lonpitch);
			double sina = sin( i * lonpitch);
			double cosb = cos( -j * latpitch + M_PI_2);
			double sinb = sin( -j * latpitch + M_PI_2);
			_pos.push_back(Vector3d(cosb*cosa,cosb*sina,sinb));
		}
	}
	
	cerr<<"init node";
	init_node();
	cerr<<"done"<<endl;
	return true;	

}


bool SDT::init_node()
{		
	int size = _latitude * _longitude ;
	_dist.assign(size, 10000);
	for(int i = 0 ; i < size ; i++)
	{
		if(!_binary[i])_dist[i] = 0;
	}
	return true;
}
bool SDT::set_size(int lat, int lon)
{
	if(lat < 3 )
	{
		fprintf(stderr, "Error invalid size of vertical resolution\n");	
		return false;
	}
	if(lon < 3 )
	{
		fprintf(stderr, "Error invalid size of horizontal resolution\n");
		return false;
	}
	_latitude = lat;
	_longitude = lon;
	return true;
}
bool 
SDT::finalize_distance()
{
	;
return true;
};
bool 
SDT::run()
{
	vector<int> foreground;
	for(int i = 0 ; i < (int)_pos.size() ; i++)
	{
		if(_dist[i] < 1.0e-10) foreground.push_back(i);
	}

	for(int i = 0 ; i < (int)_pos.size() ; i++)
	{
		if(_dist[i] < 1.0e-10) continue;
		for(int j = 0 ; j < (int)foreground.size() ; j++)
		{
			if(i == j) continue;
			double angle = _pos[i].angle(_pos[foreground[j]]);
			if(angle  < _dist[i])_dist[i] = angle;
		}
	}
	return true;
};
bool SDT::write()
{
	this->finalize_distance();

	unsigned char ** image = new unsigned char*[_latitude];
	int offset = (_addflag)? 1: 0;
	for(int i = 0 ; i < (int)_latitude ; i++) image[i] = new unsigned char [ (_longitude+offset) * 3];
	for(int i = 0 ; i < (int)_dist.size() ; i++)
	{
		int y  = (int)(i / (_longitude));
		int x  = (int)(i % (_longitude));
		unsigned char c  = (unsigned char )(_dist[i] * 255);
		image[y][x]   = c;
		if( offset && x== 0)
		{
			image[y][_longitude]   = c;
		}
	}
	int newL = _longitude + offset;
	save_png(_output_file, _latitude,newL, image);

	for(int i = 0 ; i < (int)_latitude ; i++) delete image[i];
	delete[] image;
	return true;
};

void 
SDT::usage()
{
	fprintf(stderr, "Usage: sdt [-c -v -sv -q -sq] -lat LATITUDE -lon LONGITUDE -n seednum OUTPUT.PNG\n"); 
	fprintf(stderr, "Usage: sdt [-c -v -sv -q -sq] -i INPUT.PNG OUTPUT.PNG\n"); 
	fprintf(stderr, "Option : \n"
		"\t none : Brute-force distance transform\n"	
			"\t -c : chamfer distance transform \n"
			"\t -v : vector spherical distance transform \n"
			"\t -q : quaternion distance transform \n"
			"\t -sv : sequential vector spherical distance transform \n"
			"\t -sq : sequential quaternion spherical distance  transform \n"
			);
}

void 
SDT::get_dist(vector<double>& dist)
{
	this->finalize_distance();
	dist.clear();
	dist.insert(dist.end(), _dist.begin(), _dist.end());
}
void 
SDT::get_boundary(vector<Vector3d>& pnt)
{
	pnt.clear();
	for(int i = 0 ; i < (int)_pos.size() ; i++)
	{
		if(!_binary[i]) pnt.push_back(_pos[i]);
	}

};
bool
SDT::read_png_as_binary(string inputfile, int& latitude, int& longitude, vector<bool>& binary)
{
	FILE*           fp = fopen(inputfile.data(),"rb");
	if(fp == NULL)
	{
		fprintf(stderr, "Error. File %s not found\n",inputfile.data());
		return false;
	}

	png_structp     png_ptr;
	png_infop       info_ptr;
	int             bit_depth, color_type, interlace_type;
	int             i;
	png_uint_32 width ;
	png_uint_32 height ;
	// まずファイルを開きます
	png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);             // info_ptr構造体を確保・初期化します
	png_init_io(png_ptr, fp);                               // libpngにfpを知らせます
	png_read_info(png_ptr, info_ptr);                       // PNGファイルのヘッダを読み込みます
	png_get_IHDR(png_ptr, info_ptr, &width, &height,        // IHDRチャンク情報を取得します
		&bit_depth, &color_type, &interlace_type,
		NULL, NULL);
	unsigned char **image = (png_bytepp)malloc(height * sizeof(png_bytep)); // 以下３行は２次元配列を確保します
	for (i = 0; i < (int)height; i++) image[i] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
	png_read_image(png_ptr, image);   

	// ファイルを閉じます
	latitude = height;
	longitude = width;
	int step = png_get_rowbytes(png_ptr, info_ptr) / height;

	fprintf(stderr, "%d %d \n",width,height);
	binary.reserve(latitude * longitude);

	for(int y = 0 ; y < latitude ; y++)
	{
		for(int x = 0 ; x < longitude ; x++)
		{
			binary.push_back(image[y][x * step] > 0x01);
		}
	}
	free(image);
	return true;
}
bool
SDT::save_png(string inputfile, int& latitude, int& longitude, unsigned char** image)
{
	FILE            *fp;
	png_structp     png_ptr;
	png_infop       info_ptr;

	png_uint_32 width  = (png_uint_32) longitude;
	png_uint_32 height  =(png_uint_32) latitude;

	fp = fopen((char*)inputfile.data(), "wb");                            // まずファイルを開きます
	png_ptr = png_create_write_struct(                      // png_ptr構造体を確保・初期化します
		PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);             // info_ptr構造体を確保・初期化します
	png_init_io(png_ptr, fp);                               // libpngにfpを知らせます
	png_set_IHDR(png_ptr, info_ptr, width, height,          // IHDRチャンク情報を設定します
		8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);                      // PNGファイルのヘッダを書き込みます

	png_write_image(png_ptr, image);                       // 画像データを書き込みます
	png_write_end(png_ptr, info_ptr);                       // 残りの情報を書き込みます
	png_destroy_write_struct(&png_ptr, &info_ptr);          // ２つの構造体のメモリを解放します
	fclose(fp);                                             // ファイルを閉じます
	return true;
}