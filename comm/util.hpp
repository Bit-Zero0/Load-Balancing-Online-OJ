#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <fstream>
#include <atomic>

const std::string temp_path = "./temp/";

namespace ns_util
{
	class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);

            return std::to_string(_time.tv_sec);
        }

        static std::string GetTimeMs()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);

            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    };
	
    class PathUtil
    {
    public:
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path = temp_path;
            path += file_name;
            path += suffix;
            return path;
        }

    public:
		//// 编译时需要有的临时文件
        // 构建源文件路径+后缀的完整文件名
        static std::string Src(const std::string &file_name)
        {
            return AddSuffix(file_name, "cpp");
        }

        // 构建可执行程序的完整路径+后缀名
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name, "exe");
        }

		static std::string CompileError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }


		//// 运行时需要的临时文件
        // 构建该程序对应的标准错误完整的路径+后缀名
        static std::string Stderr(const std::string &file_name)
        {
            return AddSuffix(file_name, "stderr");
        }
		
		static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }

        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }
    };

    class FileUtil
    {
    public:
        static bool IsFileExists(const std::string &path_name)
        {
            struct stat buf;
            if (stat(path_name.c_str(), &buf) == 0) // get file status,if zero is returned , on success
            {
                // 获取属性成功，文件已经存在
                return true;
            }

            return false;
        }
		
		static std::string UniqueFileName()
        {
            static std::atomic_uint id(0);
            ++id;
            std::string ms = TimeUtil::GetTimeMs();
            std::string unique_id = std::to_string(id);
            return ms + "_" + unique_id;
        }

        static bool WriteFile(const std::string &target, const std::string &content)
        {
            std::ofstream out(target);
            if (!out.is_open())
            {
                return false;
            }

            out.write(content.c_str(), content.size());
            out.close();
            return true;
        }

        static bool ReadFile(const std::string target, std::string *content, bool KeepSymbol = false)
        {
            (*content).clear();

            std::ifstream in(target);
            if (in.is_open())
            {
                return false;
            }

            std::string line;

            while (std::getline(in, line))
            {
                (*content) += line;
                (*content) += (KeepSymbol ? "\n" : "");
            }

            in.close();
            return true;
        }
    };
}