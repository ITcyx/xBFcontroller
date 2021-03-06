#pragma once
#ifndef XBFCONTROLLER_H
#define XBFCONTROLLER_H
#include "code.h"
#include "datastructure.h"
#include <string>
#include <fstream>

namespace x
{
	// ********** 以下的未经过测试 ********** //
	// bfreader 读取二进制文件
	class bfreader
	{
	private:
		std::ifstream bf;
		std::string fn;
		unsigned char *buffer;
		int c, status, bi, bj, cn;
		long long p, s;
	public:
		// 构造函数、析构函数
		bfreader(int cluster = 4096);  // cluster ，簇，默认值为4096KiB，ifstream一次读取的大小为一个簇的大小
		bfreader(const std::string& file_name, int cluster = 4096);  // file_name ，文件名，构造函数直接指定将要写入的文件名
		~bfreader();

		// 功能性函数
		bool set_filename(const std::string& file_name, int cluster = 4096);  // 设置将要写入的文件名，若文件不存在或无法写入，返回false
		bool set_cluster(int cluster = 4096);  // 设置簇的大小，不会改变当前的buffer池的内容与大小，将在下一次写入中采用新的簇的值。若输入的值小于等于零，将不会更新簇的大小且返回false
		bool set_position(const long long& position);  // position 设置写入位置，当超过文件大小或为负数或者其他无法操作文件等情况，返回false
		long long get_size(const int& unit = code::B) const;  // unit ，单位，默认值为Byte。本函数为获取当前文件的大小。如果为负数，即未打开文件
		int get_cluster() const;  // 返回cluster的值
		long long get_position() const;  // 返回当前读到的字符相对文件头的距离（0即正好读取到文件头，-1即代表没加载文件)
		barray_long read(const long long& length = 1, const long long& position = -1);  // length ，从当前读取的位置开始，读取指定长度的内容。若指定长度超过文件剩余大小，将只返回文件剩余大小的内容；若length小于等于0，则返回空barray；若指定的文件因某些原因无法读取，将返回空barray。position ，设置读取位置，如果为负数或超过当前文件大小，将从当前位置读起。默认值-1
		void close();  // 关闭文件
	};

	class bfwriter
	{
	private:
		std::ofstream bf;
		std::string fn;
		unsigned char *buffer;
		int c, status, bi;
		long long p, s;
	public:
		// 构造函数、析构函数
		bfwriter(int cluster = 4096);  // cluster ，簇，默认值为4096KiB，ifstream一次写入的大小为一个簇的大小
		bfwriter(const std::string& file_name, int cluster = 4096);  // file_name ，文件名，构造函数直接指定将要写入的文件名
		~bfwriter();

		// 功能性函数
		bool set_filename(const std::string& file_name, int cluster = 4096);  // 设置将要写入的文件名，若文件不存在或无法写入，返回false
		bool set_cluster(int cluster = 4096);  // 设置簇的大小，不会改变当前的buffer池的内容与大小，将在下一次写入中采用新的簇的值。若输入的值小于等于零，将不会更新簇的大小且返回false
		bool set_position(const long long& position);  // position 设置写入位置，当超过文件大小或为负数或者其他无法操作文件等情况，返回false
		long long get_size(const int& unit = code::B) const;  // unit ，单位，默认值为Byte。本函数为获取当前文件的大小。如果为负数，即未打开文件
		int get_cluster() const;  // 返回cluster的值
		long long get_position() const;  // 返回当前写到的字符相对文件头的距离（0即正好写入到文件头，-1即代表没加载文件)
		bool write(const barray_long& content, const long long& position = -1, const int& operation = code::COVER);  // content ，从写入的位置开始，往后写入content的内容。position ，设置写入的位置，如果为负数或超过当前文件大小，将从当前位置写起。默认值-1
		bool flush();  // 清空buffer池
		void close();  // 关闭文件
	};
}


#endif
