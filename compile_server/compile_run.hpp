#pragma once

#include <iostream>
#include <jsoncpp/json/json.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"
#include "compiler.hpp"
#include "runner.hpp"
namespace ns_compile_and_run
{
    using namespace ns_util;
    using namespace ns_log;
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompileAndRun
    {
    public:
	static void CleanTempFile(const std::string &file_name)
        {
            std::string _compile_error = PathUtil::CompileError(file_name);
            if (FileUtil::IsFileExists(_compile_error))
                unlink(_compile_error.c_str());

            std::string _stdin = PathUtil::Stdin(file_name);
            if (FileUtil::IsFileExists(_stdin))
                unlink(_stdin.c_str());

            std::string _stdout = PathUtil::Stdout(file_name);
            if (FileUtil::IsFileExists(_stdout))
                unlink(_stdout.c_str());

            std::string _stderr = PathUtil::Stderr(file_name);
            if (FileUtil::IsFileExists(_stderr))
                unlink(_stderr.c_str());

            std::string _exe = PathUtil::Exe(file_name);
            if (FileUtil::IsFileExists(_exe))
                unlink(_exe.c_str());

            std::string _src = PathUtil::Src(file_name);
            if (FileUtil::IsFileExists(_src))
                unlink(_src.c_str());
        }
		
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
            std::string desc;

            switch (code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "提交的代码是空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                FileUtil::ReadFile(PathUtil::CompileError(file_name), &desc, true); // 代码编译的时候发生了错误
                break;
            case SIGABRT: // 6
                desc = "内存超过范围";
                break;
            case SIGXCPU: // 24
                desc = "CPU使用超时";
                break;
            case SIGFPE: // 8
                desc = "浮点数溢出";
                break;
            default:
                desc = "未知" + std::to_string(code);
                break;
            }

            return desc;
        }

    public:
        static void Start(std::string &in_json, std::string *out_json)
        {
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value);

            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            int status_code = 0;
            int run_result = 0;
            Json::Value out_value;
            std::string file_name; // 需要形成的唯一文件名

            if (code.size() == 0)
            {
                status_code = -1; // 代码为空
                goto END;
            }

            // 形成的文件名只具有唯一性，没有目录没有后缀
            // 毫秒级时间戳+原子性递增唯一值: 来保证唯一性
            file_name = FileUtil::UniqueFileName();

            // 形成临时src文件
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status_code = -2;
                goto END;
            }

            // 如果编译失败
            if (!Compiler::Compile(file_name))
            {
                status_code = -3; // 代码编译的时候发生了错误
                goto END;
            }

            run_result = Runner::Run(file_name, cpu_limit, mem_limit);

            if (run_result < 0)
            {
                status_code = -2; // //未知错误
            }
            else if (run_result > 0)
            {
                status_code = run_result;
            }
            else
            {
                status_code = 0; // 运行成功
            }

        END:
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name);

            if (status_code == 0)
            {
                // 以上整个过程全部成功
                std::string _stdout;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true);
                out_value["stdout"] = _stdout;

                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr, true);
                out_value["stderr"] = _stderr;
            }
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);
			
			CleanTempFile(file_name);
        }
    };
}
