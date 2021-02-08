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

x::bfreader::bfreader(const std::string& file_name, int cluster)
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

bool x::bfreader::set_filename(const std::string& file_name, int cluster)
{
	if (cluster <= 0)
		cluster = 4096;
	if (c != cluster)
	{
		c = cluster;
		delete[]buffer;
		buffer = new unsigned char[c];
		bi = bj = -1;
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

bool x::bfreader::set_position(const long long& position)
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

long long x::bfreader::get_size(const int& unit) const
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

int x::bfreader::get_cluster() const
{
	return c;
}

long long x::bfreader::get_position() const
{
	return p;
}

x::barray_long x::bfreader::read(const long long& length, const long long& position)
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
				p = bf.tellg();
				for (j = 0; j < c; ++j)
					a[i*c + j] = buffer[j];
				bf.read((char*)buffer, c);
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
			for (j = bi; j < bj&&j < (a.get_length() + bi); ++j)
				a[j - bi] = buffer[j];
			long long g = j - bi;
			bi = j;
			if (a.get_length() - g > 0)
			{
				if (cn > 0)
				{
					delete[]buffer;
					c = cn;
					cn = -1;
					buffer = new unsigned char[c];
				}
				cm = (a.get_length() - g) / c;
				bf.read((char*)buffer, c);
				for (i = 0; i < cm; ++i)
				{
					p = bf.tellg();
					for (j = 0; j < c; ++j)
						a[i*c + j + g] = buffer[j];
					bf.read((char*)buffer, c);
				}
				for (j = 0; j < a.get_length() - g - cm * c; ++j)
					a[i*c + j + g] = buffer[j];
				bi = a.get_length() - g - cm * c;
				bj = bf.tellg() - p;
				p = bi;
				return a;
			}
			p = bi;
			return a;
		}
	}
	if (cn > 0)
	{
		delete[]buffer;
		c = cn;
		cn = -1;
		buffer = new unsigned char[c];
	}
	bf.seekg(position, std::ios::beg);
	p = bf.tellg();
	if (p + length - 1 < s)
		a.set_length(length);
	else
		a.set_length(s - p);
	bf.read((char*)buffer, c);
	cm = a.get_length() / c;
	for (i = 0; i < cm; ++i)
	{
		p = bf.tellg();
		for (j = 0; j < c; ++j)
			a[i*c + j] = buffer[j];
		bf.read((char*)buffer, c);
	}
	for (j = 0; j < a.get_length() - cm * c; ++j)
		a[i*c + j] = buffer[j];
	bi = a.get_length() - cm * c;
	bj = bf.tellg() - p;
	p = bi;
	return a;
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










x::bfwriter::bfwriter(int cluster)
{
	fn = "";
	if (cluster <= 0)
		cluster = 4096;
	c = cluster;
	s = -1;
	buffer = new unsigned char[c];
	bi = -1;
	status = code::INIT;
	p = -1;
	s = -1;
}

x::bfwriter::bfwriter(const std::string& file_name, int cluster)
{
	if (cluster <= 0)
		cluster = 4096;
	c = cluster;
	buffer = new unsigned char[c];
	bf.open(file_name, std::ios::binary | std::ios::app);
	bf.close();
	bf.clear();
	bf.open(file_name, std::ios::binary | std::ios::in);
	if (!bf.good())
	{
		fn = "";
		status = code::INIT;
		p = -1;
		s = -1;
		bi = -1;
	}
	else
	{
		fn = file_name;
		status = code::NORMAL;
		bf.seekp(0, std::ios::end);
		s = bf.tellp();
		bf.seekp(0, std::ios::beg);
		p = 0;
		bi = 0;
	}
}

x::bfwriter::~bfwriter()
{
	bf.close();
	bf.clear();
	delete[]buffer;
}

bool x::bfwriter::set_filename(const std::string& file_name, int cluster)
{
	if (cluster <= 0)
		cluster = 4096;
	if (c != cluster)
	{
		if (bi <= 0)
		{
			c = cluster;
			delete[]buffer;
			buffer = new unsigned char[c];
		}
		else
		{
			bf.write((char*)buffer, bi);
			bf.flush();
			c = cluster;
			delete[]buffer;
			buffer = new unsigned char[c];
			bi = 0;
		}
	}
	if (fn == "")
	{
		bf.open(file_name, std::ios::binary | std::ios::app);
		bf.close();
		bf.clear();
		bf.open(file_name, std::ios::binary | std::ios::in);
	}
	else
	{
		if (bi > 0)
		{
			bf.write((char*)buffer, bi);
			bf.flush();
			bi = 0;
		}
		bf.close();
		bf.clear();
		fn = "";
		bf.open(file_name, std::ios::binary | std::ios::app);
		bf.close();
		bf.clear();
		bf.open(file_name, std::ios::binary | std::ios::in);
	}
	if (!bf.good())
	{
		bi = -1;
		s = -1;
		p = -1;
		status = code::INIT;
		return false;
	}
	fn = file_name;
	bi = 0;
	bf.seekp(0, std::ios::end);
	s = bf.tellp();
	bf.seekp(0, std::ios::beg);
	p = 0;
	status = code::NORMAL;
	return true;
}

bool x::bfwriter::set_cluster(int cluster)
{
	if (cluster <= 0)
		return false;
	if (cluster == c)
		return true;
	if (bi <= 0)
	{
		c = cluster;
		delete[]buffer;
		buffer = new unsigned char[c];
		return true;
	}
	bf.write((char*)buffer, bi);
	bf.flush();
	c = cluster;
	delete[]buffer;
	buffer = new unsigned char[c];
	bi = 0;
	return true;
}

bool x::bfwriter::set_position(const long long& position)
{
	if (fn == "")
		return false;
	if (position > s || position < 0)
		return false;
	if (bi == 0)
	{
		p = position;
		bf.seekp(p, std::ios::beg);
		return true;
	}
	bf.write((char*)buffer, bi);
	bf.flush();
	bi = 0;
	p = position;
	bf.seekp(p, std::ios::beg);
	return true;
}

long long x::bfwriter::get_size(const int& unit) const
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

int x::bfwriter::get_cluster() const
{
	return c;
}

long long x::bfwriter::get_position() const
{
	return p;
}

bool x::bfwriter::write(const x::barray_long& content, const long long& position, const int& operation)
{
	if (operation != code::COVER || operation != code::INSERT)
		return false;
	if (fn == "")
		return false;
	if (content.get_length() == 0)
		return true;
	if (position > s)
		return false;
	if (position != p && position >= 00 && bi > 0)
	{
		bf.write((char*)buffer, bi);
		bf.flush();
		bi = 0;
	}
	if (position >= 0)
	{
		p = position;
		bf.seekp(p, std::ios::beg);
	}
	long long bj, t;
	if (operation == code::COVER || p == s)
	{
		for (bj = 0; bj < content.get_length(); ++bj)
		{
			if (bi == c)
			{
				bf.write((char*)buffer, bi);
				bf.flush();
				bi = 0;
			}
			buffer[bi] = content[bj];
			++bi;
		}
		p = bf.tellp();
		p += bi;
		if (s < p)
			s = p;
		return true;
	}
	if (bi > 0)
	{
		bf.write((char*)buffer, bi);
		bf.flush();
		bi = 0;
	}
	std::ofstream bfo;
	std::ifstream bfi;
	std::string bfon = "";
	while (true)
	{
		bfo.open(fn + bfon + ".xcache", std::ios::binary | std::ios::app | std::ios::out, _SH_DENYWR);
		if (bfo.good())
			break;
		bfo.clear();
		if (bfon.length() == 0)
			bfon += "a";
		else
		{
			if (bfon[bfon.length() - 1] < 'z')
				++bfon[bfon.length() - 1];
			else
				bfon += "a";
		}
		if (bfon.length() > 50)
			return false;
	}
	bfi.open(fn, std::ios::binary);
	bfi.seekg(p, std::ios::beg);
	t = (s - p) / c;
	for (bj = 0; bj < t; ++bj)
	{
		bfi.read((char*)buffer, c);
		bfo.write((char*)buffer, c);
	}
	if (bj < t*c)
	{
		bfi.read((char*)buffer, bj - t * c);
		bfo.write((char*)buffer, bj - t * c);
	}
	bfo.flush();
	bfi.close();
	bfi.clear();
	for (bj = 0; bj < content.get_length(); ++bj)
	{
		if (bi == c)
		{
			bf.write((char*)buffer, bi);
			bf.flush();
			bi = 0;
		}
		buffer[bi] = content[bj];
		++bi;
	}
	if (bi > 0)
	{
		bf.write((char*)buffer, bi);
		bf.flush();
		bi = 0;
	}
	p = bf.tellp();
	bfi.open(fn + bfon + ".xcache", std::ios::binary);
	for (bj = 0; bj < t; ++bj)
	{
		bfi.read((char*)buffer, c);
		bf.write((char*)buffer, c);
		bf.flush();
	}
	if (bj < t*c)
	{
		bfi.read((char*)buffer, bj - t * c);
		bf.write((char*)buffer, bj - t * c);
		bf.flush();
	}
	bfi.close();
	bfi.clear();
	bfo.close();
	bfo.clear();
	p = bf.tellp();
	s = 0;
	bi = 0;
	return true;
}

void x::bfwriter::close()
{
	if (bi > c)
	{
		bf.write((char*)buffer, bi);
		bf.flush();
		bi = 0;
	}
	bf.close();
	bf.clear();
	fn = "";
	p = -1;
	s = -1;
	bi = -1;
}

bool x::bfwriter::flush()
{
	if (bi > 0)
	{
		bf.write((char*)buffer, bi);
		bf.flush();
		bi = 0;
		return true;
	}
	return false;
}
