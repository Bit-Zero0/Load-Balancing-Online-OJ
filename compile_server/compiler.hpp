#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler
{
    using namespace ns_util;
    using namespace ns_log;

    class Compiler
    {
    public:
        Compiler(){};

        ~Compiler(){};

        static bool Compile(const std::string &file_name)
        {
            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "内部错误，创建子进程失败" << "\n";
                return false;
            }
            else if (pid == 0)
            {
                umask(0);
                int compile_stderr = open(PathUtil::CompileError(file_name).c_str(), O_CREAT | O_WRONLY, 0644);
                if (compile_stderr < 0)
                {
                    LOG(WARNNING) << "没有成功形成stderr文件" << "\n";
                    exit(1);
                }

                dup2(compile_stderr, 2); // 重定向标准错误到_stderr

                // 程序替换，并不影响进程的文件描述符表
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(),
                       PathUtil::Src(file_name).c_str(), "-D", "COMPILER_ONLINE", "-std=c++11", nullptr);

                LOG(ERROR) << "启动编译器g++失败，可能是参数错误"
                           << "\n";
                exit(2);
            }
            else
            {
                waitpid(pid, nullptr, 0);
                if (FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO) << PathUtil::Src(file_name) << " 编译成功!"
                              << "\n";
                    return true;
                }
            }
            LOG(ERROR) << "编译失败，没有形成可执行程序" << "\n";
            return false;
        }
    };
}
