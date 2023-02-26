#pragma once
#include <iostream>
#include <vector>
#include <ctemplate/template.h>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "oj_model.hpp"

namespace ns_view
{
    using namespace ns_model;

    const std::string template_path = "./template_html/";

    class View
    {
    public:
        void AllExpandHtml(const vector<struct Question> &questions, std::string *html)
        {

            std::string src_path = temp_path + "all-questions.html";

            // 形成数字典
            ctemplate::TemplateDictionary root("all_questions");
            for (auto &q : questions)
            {
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("questions_list");
                // 题目的编号 题目的标题 题目的难度
                sub->SetValue("number", q.number);
                sub->SetValue("title", q.title);
                sub->SetValue("star", q.star);
            }

            // 获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_path, ctemplate::DO_NOT_STRIP);

            // 开始渲染功能
            tpl->Expand(html, &root);
        }

        void OneExpandHtml(const struct Question &q, string *html)
        {
            std::string src_path = temp_path + "one_question.html";

            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("numver", q.number);
            root.SetValue("title", q.title);
            root.SetValue("star", q.star);
            root.SetValue("desc", q.desc);
            root.SetValue("pre_code", q.header);

            // 获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_path, ctemplate::DO_NOT_STRIP);

            // 开始渲染功能
            tpl->Expand(html, &root);
        }
    };
}