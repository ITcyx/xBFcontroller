#include "bfcontroller.h"

x::bfreader::bfreader(int cluster)
{
	fn = "";
	if (cluster <= 0)
		cluster = 4096;
	c = cluster;
	cn = -1;
	buffer = new unsigned char[c];
	bi = -1;
	bj = -1;
	status = code::INIT;
}

x::bfreader::bfreader(std::string file_name, int cluster)
{
	if (cluster <= 0)
		cluster = 4096;
	c = cluster;
	cn = -1;
	buffer = new unsigned char[c];
	bi = -1;
	bj = -1;
	bf.open(file_name, std::ios::binary);
	if (!bf.good())
	{
		fn = "";
		status = code::INIT;
	}
	else
	{
		fn = file_name;
		status = code::NORMAL;
	}
}

x::bfreader::~bfreader()
{
	delete[]buffer;
}


bool x::bfreader::set_filename(std::string file_name, int cluster)
{
	if (fn == "")
	{
		bf.open(file_name);
		if (!bf.good())
		{
			bi = -1;
			bj = -1;
			cn = -1;
			if (cluster <= 0)
				cluster = 4096;
			if (c != cluster)
			{
				c = cluster;
				delete[]buffer;
				buffer = new unsigned char[c];
			}
			status = code::INIT;
			return false;
		}
	}
	else
	{
		bf.close();
		fn = "";
		bf.open(file_name);
		if (!bf.good())
		{
			bi = -1;
			bj = -1;
			cn = -1;
			if (cluster <= 0)
				cluster = 4096;
			if (c != cluster)
			{
				c = cluster;
				delete[]buffer;
				buffer = new unsigned char[c];
			}
			status = code::INIT;
			return false;
		}
	}
	fn = file_name;
	bi = -1;
	bj = -1;
	cn = -1;
	if (cluster <= 0)
		cluster = 4096;
	if (c != cluster)
	{
		c = cluster;
		delete[]buffer;
		buffer = new unsigned char[c];
	}
	status = code::NORMAL;
	return true;
}

bool x::bfreader::set_cluster(int cluster)
{
	if (cluster <= 0)
		return false;
	cn = cluster;
	return true;
}

std::string x::bfreader::read(int length)
{
	std::string a;

	return a;
}