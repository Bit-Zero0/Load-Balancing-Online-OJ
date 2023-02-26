#include <iostream>
#include "../comm/httplib.h"
#include "oj_controller.hpp"

using namespace httplib;
using namespace ns_controller;

int main()
{
    Server svr; // 用户请求的服务路由功能

    Controller ctrl;

    // 获取所有的题目列表
    svr.Get("/all_questions", [&ctrl](const Request &req, Response &resp)
            {
        std::string html;
        ctrl.AllQuestions(&html);
        resp.set_content(html , "text/html;charset=utf-8"); });

    // 用户要根据题目编号，获取题目的内容
    // /question/100 -> 正则匹配
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
            {
        std::string number = req.matches[1];
        std::string html;
        ctrl.Question(number, &html);
        resp.set_content(html , "text/html;charset=utf-8"); });

    svr.Post(R"(/judge/(\d+))", [](const Request &req, Response &resp)
             { std::string number = req.matches[1]; });

    svr.set_base_dir("./wwwroot");
    svr.listen("0.0.0.0", 8080);
    return 0;
}