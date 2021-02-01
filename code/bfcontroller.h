#pragma once
#ifndef XBFCONTROLLER_H
#define XBFCONTROLLER_H
#include "code.h"
#include <string>
#include <fstream>

namespace x
{
	class bfreader
	{
	private:
		std::ifstream bf;
		std::string fn;
		unsigned char *buffer;
		int c, cn, status, bi, bj;
	public:
		// 构造函数、析构函数
		bfreader(int cluster = 4096);  // cluster ，簇，默认值为4096KiB，ifstream一次读取的大小为一个簇的大小
		bfreader(std::string file_name, int cluster = 4096);  // file_name ，文件名，构造函数直接指定将要读取的文件名
		~bfreader();

		// 功能性函数
		bool set_file_name(std::string file_name, int cluster = 4096);  // 设置将要读取的文件名，若文件不存在或无法读取，返回false
		bool set_cluster(int cluster = 4096);  // 设置簇的大小，不会改变当前的buffer池的内容与大小，将在下一次读取中采用新的簇的值。若输入的值小于等于零，将不会更新簇的大小且返回false
		std::string read(int length=1);  // 从当前读取的位置开始，读取指定长度的内容。若指定长度超过文件剩余大小，将只返回文件剩余大小的内容；若指定的文件因某些原因无法读取，将返回空字符串
		int size(int unit = code::B);  // unit ，单位，默认值为Byte。本函数为获取当前文件的大小
		int cluster_value();  // 返回cluster的值
		bool clean();  //
		bool close();
	};

	class bfwriter
	{

	};
}


#endif
