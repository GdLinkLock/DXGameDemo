#ifndef _MINI_INCLUDE_SIMPLE_PROFILE_H_
#define _MINI_INCLUDE_SIMPLE_PROFILE_H_

#pragma warning(disable:4996)

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <fstream>
#include <string>

namespace sprfl
{
	class CSimpleProfile
	{
	public:
		CSimpleProfile(char* filename):
	    m_total_begin(0),m_total_end(0),
		m_time1(0),m_time2(0)
		{
			m_filename = filename;
		}

		void begin()
		{			
			m_fs.clear();
			m_fs.open(m_filename.c_str(),std::ios::out);

			if(!m_fs.is_open())
			{
				assert(0);
			}

			m_total_begin = GetTickCount();
			m_time1 = m_total_begin;
		}

		void getTime(char* mark)
		{
			m_time2 = GetTickCount();
			std::string mark_s = mark;
			std::string time_s = mark_s + " time: " + toString(m_time2 - m_time1) + "ms \r\n";
			m_fs.write(time_s.c_str(),time_s.size());
			m_fs.flush();
			m_time1 = m_time2;
		}

		void end(char* mark)
		{
			m_total_end = GetTickCount();
			std::string mark_s = mark;
			std::string time_s = mark_s + " total time: " + toString(m_total_end - m_total_begin) + "ms \r\n";
			m_fs.write(time_s.c_str(),time_s.size());
			m_fs.flush();
			m_fs.close();

			m_time1 = 0;
			m_time2 = 0;
			m_total_begin = 0;
			m_total_end = 0;
		}

		void setFilename(char* name)
		{
			m_filename = name;
		}

	private:

		std::string toString(DWORD n)
		{
			char temp[128];
			sprintf(temp,"%d",n);
			return std::string(temp);
		}

		std::fstream m_fs;
		std::string m_filename;
		DWORD m_total_begin;
		DWORD m_total_end;
		DWORD m_time1;
		DWORD m_time2;
	};
}

#endif