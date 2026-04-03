// 视图（view）


#include<iostream>
#include<string>
#include<vector>
#include<ctemplate/template.h>
#include "model.hpp"
namespace oj_view
{
    using namespace oj_model;

    class View
    {
    public:
        View() = default;
        ~View() = default;

        void AllExpandHtml(const std::vector<Question>& questions, std::string& html)
        {
            ctemplate::TemplateDictionary root("all_questions");
            for(const auto& q : questions)
            {
                ctemplate::TemplateDictionary* temp = root.AddSectionDictionary("question_list");
                temp->SetValue("num", q.num);
                temp->SetValue("name", q.name);
                temp->SetValue("star", q.star);
            }
            RenderTemplate("QuestionsList.html", root, html);
        }

        void OneExpandHtml(const Question& q, std::string& html)
        {
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("num", q.num);
            root.SetValue("name", q.name);
            root.SetValue("star", q.star);
            root.SetValue("desc", q.desc);
            root.SetValue("code", q.header);
            RenderTemplate("Question.html", root, html);
        }

    private:
        //渲染逻辑
        static inline const std::string template_path = "./Interface/";
        //template_file：模板文件名、dict：字典对象、html：输出参数
        bool RenderTemplate(const std::string& template_file, const ctemplate::TemplateDictionary& dict, std::string& html)
        {
            html.clear();
            std::string temp = template_path + template_file;
            // GetTemplate：从磁盘加载并解析html模板文件
            // ctemplate::DO_NOT_STRIP：指示渲染时保留 HTML 中的空白符和换行
            ctemplate::Template* tpl = ctemplate::Template::GetTemplate(temp, ctemplate::DO_NOT_STRIP);
            if(tpl == nullptr)
            {
                std::cerr << "加载模板失败" << "\n";
                return false;
            }
            //xpand：将之前准备好的字典dict中的数据（如题目编号、标题等），按照模板文件中的标记（{{number}}, {{title}} 等），填充到 HTML 字符串中。
            tpl->Expand(&html, &dict);
            return true;
        }

    };
}