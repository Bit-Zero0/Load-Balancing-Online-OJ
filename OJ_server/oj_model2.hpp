#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

#include "include/mysql.h"
#include "../comm/log.hpp"

namespace ns_model
{
    using namespace std;
    using namespace ns_log;

    struct Question
    {
        std::string number; // 题目编号，唯一
        std::string title;  // 题目的标题
        std::string star;   // 难度: 简单 中等 困难
        std::string desc;   // 题目的描述
        std::string header; // 题目预设给用户在线编辑器的代码
        std::string tail;   // 题目的测试用例，需要和header拼接，形成完整代码
        int cpu_limit;      // 题目的时间要求(S)
        int mem_limit;      // 题目的空间要去(KB)
    };

    const string questions = "data";
    const string host = "127.0.0.1";
    const string user = "oj_client";
    const string passwd = "123456";
    const string db = "oj";
    const int port = 3306;

    class Model
    {
    public:
        Model(){};
        ~Model() {}

        bool QueryMysql(const string &sql, vector<Question> *out)
        {
            // 创建mysql句柄

            MYSQL *my = mysql_init(nullptr);

            // 连接数据库
            if (nullptr == mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0))
            {
                LOG(FATAL) << "连接数据库失败!"
                           << "\n";
                return false;
            }

            // 一定要设置该链接的编码格式, 要不然会出现乱码问题
            mysql_set_character_set(my, "utf8");

            LOG(INFO) << "连接数据库成功!"
                      << "\n";

            // 执行sql语句
            if (0 != mysql_query(my, sql.c_str()))
            {
                LOG(WARNNING) << sql << " execute error!"
                              << "\n";
                return false;
            }

            // 提取结果
            MYSQL_RES *res = mysql_store_result(my);

            // 分析结果
            int rows = mysql_num_rows(res);   // 获得行数量
            int cols = mysql_num_fields(res); // 获得列数量

            Question q;

            for (int i = 0; i < rows; i++)
            {
                MYSQL_ROW row = mysql_fetch_row(res);
                q.number = row[0];
                q.title = row[1];
                q.star = row[2];
                q.desc = row[3];
                q.header = row[4];
                q.tail = row[5];
                q.cpu_limit = atoi(row[6]);
                q.mem_limit = atoi(row[7]);

                out->push_back(q);
            }
            // 释放结果空间
            free(res);
            // 关闭mysql连接
            mysql_close(my);

            return true;
        }

        bool GetAllQuestions(vector<Question> *out)
        {
            string sql = "select * from ";
            sql += questions;
            return QueryMysql(sql, out);
        }

        bool GetOneQuestion(const string &number, Question *q)
        {
            bool res = false;
            string sql = "select * from ";
            sql += questions;
            sql += " where id=";
            sql += number;
            vector<Question> result;
            if (QueryMysql(sql, &result))
            {
                if (result.size() == 1)
                {
                    *q = result[0];
                    res = true;
                }
            }
            return res;
        }
    };
}