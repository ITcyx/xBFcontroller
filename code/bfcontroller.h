#pragma once
#ifndef XBFCONTROLLER_H
#define XBFCONTROLLER_H
#include "code.h"
#include <string>
#include <fstream>

namespace x
{
	// bfreader 读取二进制文件
	class bfreader
	{
	private:
		std::ifstream bf;
		std::string fn;
		unsigned char *buffer;
		int c, status, bi, bj;
		long long p, cn;
	public:
		// 构造函数、析构函数
		bfreader(int cluster = 4096);  // cluster ，簇，默认值为4096KiB，ifstream一次读取的大小为一个簇的大小
		bfreader(std::string file_name, int cluster = 4096);  // file_name ，文件名，构造函数直接指定将要读取的文件名
		~bfreader();

		// 功能性函数
		bool set_filename(std::string file_name, int cluster = 4096);  // 设置将要读取的文件名，若文件不存在或无法读取，返回false
		bool set_cluster(int cluster = 4096);  // 设置簇的大小，不会改变当前的buffer池的内容与大小，将在下一次读取中采用新的簇的值。若输入的值小于等于零，将不会更新簇的大小且返回false
		int get_size(int unit = code::B);  // unit ，单位，默认值为Byte。本函数为获取当前文件的大小
		int get_cluster();  // 返回cluster的值
		int get_position();  // 返回当前读到到的字符相对文件头的距离（0即正好读取到文件头，
		std::string read(int length = 1);  // 从当前读取的位置开始，读取指定长度的内容。若指定长度超过文件剩余大小，将只返回文件剩余大小的内容；若指定的文件因某些原因无法读取，将返回空字符串
		bool close();
	};

	class bfwriter
	{

	};
}


#endif
