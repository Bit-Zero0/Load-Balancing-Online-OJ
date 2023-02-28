#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <cassert>
#include <fstream>
#include <jsoncpp/json/json.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"
#include "../comm/httplib.h"

namespace ns_controller
{
    using namespace std;
    using namespace ns_model;
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_view;
    using namespace httplib;

    class Machine
    {
    public:
        std::string ip;
        int port;
        uint64_t load;
        std::mutex *mtx;

    public:
        Machine() : ip(""), port(0), load(0), mtx(nullptr)
        {
        }
        ~Machine()
        {
        }

    public:
        // 提升主机负载
        void IncLoad()
        {
            if (mtx)
                mtx->lock();

            ++load;

            if (mtx)
                mtx->unlock();
        }
        // 减少主机负载
        void DecLoad()
        {
            if (mtx)
                mtx->lock();

            --load;

            if (mtx)
                mtx->unlock();
        }

        // 获取主机负载,没有太大的意义，只是为了统一接口
        uint64_t Load()
        {
            uint64_t _load = 0;
            if (mtx)
                mtx->lock();

            _load = load;

            if (mtx)
                mtx->unlock();

            return _load;
        }
    };

    const std::string service_machine = "./conf/service_machine.conf";

    class LoadBalance
    {
    private:
        std::vector<Machine> machines; // 每一台主机都有自己的下标，充当当前主机的id
        std::vector<int> online;       // 所有在线的主机id
        std::vector<int> offline;      // 所有离线的主机id
        std::mutex mtx;

    public:
        LoadBalance()
        {
            assert(LoadConf(service_machine));
            LOG(INFO) << "加载 " << service_machine << " 成功"
                      << "\n";
        };

        ~LoadBalance() {}

    public:
        bool LoadConf(const std::string &machine_conf)
        {
            std::ifstream in(machine_conf);
            if (!in.is_open())
            {
                LOG(FALAT) << " 加载: " << machine_conf << " 失败"
                           << "\n";
                return false;
            }

            std::string line;
            while (std::getline(in, line))
            {
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, ":");
                if (tokens.size() != 2)
                {
                    LOG(WARNNING) << " 切分 " << line << " 失败"
                                 << "\n";
                    continue;
                }

                Machine m;
                m.ip = tokens[0];
                m.port = atoi(tokens[1].c_str());
                m.load = 0;
                m.mtx = new std::mutex;

                online.push_back(machines.size()); // 从0下标开始push
                machines.push_back(m);
            }

            in.close();
            return true;
        }

        // id: 输出型参数   m : 输出型参数
        bool SmartChoice(int *id, Machine **m)
        {
            mtx.lock();
            int online_num = online.size();

            if (online_num == 0)
            {
                mtx.unlock();
                LOG(FATAL) << " 所有的后端编译主机已经离线, 请运维的同事尽快查看"
                           << "\n";
                return false;
            }

            // 通过遍历的方式，找到所有负载最小的机器
            *id = online[0];
            *m = &machines[online[0]];
            uint64_t min_load = machines[online[0]].Load();
            for (int i = 0; i < online_num; i++)
            {
                uint64_t curr_load = machines[online[i]].Load();
                if (min_load > curr_load)
                {
                    min_load = curr_load;
                    *id = online[i];
                    *m = &machines[online[i]];
                }
            }

            mtx.unlock();
            return true;
        }

        void OfflineMachine(int which)
        {
            // 当我们需要离线一个Machine时，有可能会有人正在连接此Machine，所以要加锁
            mtx.lock();
            for (auto iter = online.begin(); iter != online.end(); iter++)
            {
                if (*iter == which)
                {
                    online.erase(iter);
                    offline.push_back(which);
                    break; // 因为break的存在，所有我们暂时不考虑迭代器失效的问题
                }
            }
            mtx.unlock();
        }

        void OnlineMachine()
        {
        }

        void ShowMachines()
        {
            mtx.lock();
            std::cout << "当前在线主机列表: ";
            for (auto &id : online)
            {
                std::cout << id << " ";
            }
            std::cout << std::endl;

            std::cout << "当前离线主机列表: ";
            for (auto &id : offline)
            {
                std::cout << id << "";
            }
            std::cout << std::endl;
            mtx.unlock();
        }
    };

    // 这是我们的核心业务逻辑的控制器
    class Controller
    {
    private:
        Model model;             // 提供后台数据
        View view;               // 提供html渲染功能
        LoadBalance load_balace; // 核心负载均衡器

    public:
        Controller(){};
        ~Controller(){};

        // 根据题目数据构建网页
        //  html: 输出型参数
        bool AllQuestions(string *html)
        {
            bool ret = true;
            vector<struct Question> all;
            if (model.GetAllQuestions(&all))
            {
                view.AllExpandHtml(all, html); // 获取题目信息成功，将所有的题目数据构建成网页
            }
            else
            {
                *html = "获取题目失败, 形成题目列表失败";
                ret = false;
            }

            return ret;
        }

        bool Question(const string &number, string *html)
        {
            bool ret = true;
            struct ns_model::Question q;

            if (model.GetOneQuestion(number, &q))
            {
                view.OneExpandHtml(q, html);
            }
            else
            {
                *html = "指定题目: " + number + " 不存在!";
                ret = false;
            }
            return ret;
        }

        bool Judge(const std::string &number, const std::string &in_json, std::string *out_json)
        {
            struct Question q;
            model.GetOneQuestion(number, &q);

            // 1. in_json进行反序列化，得到题目的id，得到用户提交源代码，input
            Json::Reader reader;
            Json::Value in_value;
            reader.parse(in_json, in_value);
            std::string code = in_value["code"].asString();

            // 2. 重新拼接用户代码+测试用例代码，形成新的代码
            Json::Value compile_value;
            compile_value["input"] = in_value["input"];
            compile_value["code"] = code + "\n" + q.tail;
            compile_value["cpu_limit"] = q.cpu_limit;
            compile_value["mem_limit"] = q.mem_limit;

            Json::FastWriter writer;
            std::string compile_string = writer.write(compile_value);

            // 3. 选择负载最低的主机(差错处理)
            // 规则: 一直选择，直到主机可用，否则，就是全部挂掉
            while (true)
            {
                int id = 0;
                Machine *m = nullptr;
                if (!load_balace.SmartChoice(&id, &m))
                {
                    break;
                }

                // 4. 然后发起http请求，得到结果
                httplib::Client cli(m->ip, m->port);
                m->IncLoad();
                LOG(INFO) << " 选择主机成功, 主机id: " << id << " 详情: " << m->ip << ":" << m->port << " 当前主机的负载是: " << m->Load() << "\n";
                if (auto res = cli.Post("/compile_and_run", compile_string, "application/json;charset=utf-8"))
                {
                    if (res->status == 200)
                    {
                        *out_json = res->body;
                        m->DecLoad();
                        LOG(INFO) << "请求编译和运行服务成功..."
                                  << "\n";
                        break;
                    }
                    m->DecLoad();
                }
                else
                {
                    LOG(ERROR) << " 当前请求的主机id: " << id << " 详情: " << m->ip << ":" << m->port << " 可能已经离线"
                               << "\n";
                    load_balace.OfflineMachine(id);
                    load_balace.ShowMachines(); // for test
                }
            }
        }
    };
}