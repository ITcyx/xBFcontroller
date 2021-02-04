#include "bfcontroller.h"

x::bfreader::bfreader(int cluster)
{
	fn = "";
	if (cluster <= 0)
		cluster = 4096;
	c = cluster;
	cn = -1;
	s = -1;
	buffer = new unsigned char[c];
	bi = -1;
	bj = -1;
	status = code::INIT;
	p = -1;
	s = -1;
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
		p = -1;
		s = -1;
	}
	else
	{
		fn = file_name;
		status = code::NORMAL;
		bf.seekg(0, std::ios::end);
		s = bf.tellg();
		bf.seekg(0, std::ios::beg);
		p = 0;
	}
}

x::bfreader::~bfreader()
{
	bf.close();
	bf.clear();
	delete[]buffer;
}

bool x::bfreader::set_filename(std::string file_name, int cluster)
{
	if (cluster <= 0)
		cluster = 4096;
	if (c != cluster)
	{
		c = cluster;
		delete[]buffer;
		buffer = new unsigned char[c];
	}
	cn = -1;
	if (fn == "")
		bf.open(file_name, std::ios::binary);
	else
	{
		bf.close();
		bf.clear();
		fn = "";
		bf.open(file_name, std::ios::binary);
	}
	if (!bf.good())
	{
		bi = -1;
		bj = -1;
		s = -1;
		p = -1;
		status = code::INIT;
		return false;
	}
	fn = file_name;
	bi = -1;
	bj = -1;
	bf.seekg(0, std::ios::end);
	s = bf.tellg();
	bf.seekg(0, std::ios::beg);
	p = 0;
	status = code::NORMAL;
	return true;
}

bool x::bfreader::set_cluster(int cluster)
{
	if (cluster <= 0)
		return false;
	if (cluster == c)
		return true;
	if (fn == "")
	{
		delete[]buffer;
		buffer = new unsigned char[cluster];
		c = cluster;
		return true;
	}
	cn = cluster;
	return true;
}

bool x::bfreader::set_position(long long position)
{
	if (fn == "")
		return false;
	if (position > s || position < 0)
		return false;
	if (bi < 0)
	{
		p = position;
		bf.seekg(p, std::ios::beg);
		return true;
	}
	bi = -1;
	bj = -1;
	p = position;
	bf.seekg(p, std::ios::beg);
	return true;
}

long long x::bfreader::get_size(int unit)
{
	if (s < 0)
		return s;
	if (unit == code::B)
		return s;
	if (unit >= code::KIB&&unit <= code::PIB)
		return (s >> (10 * (code::KIB - 1023)));
	if (unit >= code::KB&&unit <= code::PB)
	{
		long long t = 1000;
		for (int i = 0; i < (code::KB - 1000); ++i)
			t *= 1000;
		return s / t;
	}
	return -1;
}

int x::bfreader::get_cluster()
{
	return c;
}

long long x::bfreader::get_position()
{
	return p;
}

x::barray_long x::bfreader::read(long long length, long long position)
{
	if (status == code::INIT || length <= 0 || s == 0 || position >= s || (position < 0 && p == s))
		return barray_long();
	barray_long a;
	long long cm, i, j;
	if (position < 0)
	{
		if (p < 0)
			p = 0;
		if (p + length - 1 < s)
			a.set_length(length);
		else
			a.set_length(s - p);
		if (bi < 0)
		{
			if (cn > 0)
			{
				delete[]buffer;
				c = cn;
				cn = -1;
				buffer = new unsigned char[c];
			}
			bf.read((char*)buffer, c);
			cm = a.get_length() / c;
			for (i = 0; i < cm; ++i)
			{
				for (j = 0; j < c; ++j)
					a[i*c + j] = buffer[j];
				bf.read((char*)buffer, c);
				p = bf.tellg();
			}
			for (j = 0; j < a.get_length() - cm * c; ++j)
				a[i*c + j] = buffer[j];
			bi = a.get_length() - cm * c;
			bj = bf.tellg() - p;
			p = bi;
			return a;
		}
		else
		{
			for (j = bi; j < bj&&j < a.get_length(); ++j)
				a[j - bi] = buffer[j];
			// ---------- flag ----------
			bf.read((char*)buffer, c);
			cm = a.get_length() / c;
			for (i = 0; i < cm; ++i)
			{
				for (j = 0; j < c; ++j)
					a[i*c + j] = buffer[j];
				bf.read((char*)buffer, c);
				p = bf.tellg();
			}
			for (j = 0; j < a.get_length() - cm * c; ++j)
				a[i*c + j] = buffer[j];
			bi = a.get_length() - cm * c;
			bj = bf.tellg() - p;
			p = bi;
			return a;
		}
	}
}

void x::bfreader::close()
{
	if (status == code::INIT)
		return;
	bf.close();
	bf.clear();
	fn = "";
	bi = bj = cn = -1;
	s = p = -1;
	status = code::NORMAL;
}