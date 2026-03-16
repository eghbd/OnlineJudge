//加载文件内的题目
#pragma once

#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>

#include <fstream>
#include <cstdlib>
#include <cassert>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

// 根据题目list文件，把题目列表加载到内存中，提供接口供外部访问
namespace ns_model
{
    using namespace std;
    using namespace ns_log;
    using namespace ns_util;

    struct Question
    {
        std::string number; //编号
        std::string title;  //标题
        std::string star;   //难度
        int cpu_limit;      //时间要求
        int mem_limit;      //空间要求
        std::string desc;   //描述
        std::string header; //预设代码
        std::string tail;   //测试用例，后续需要进行文件拼接，形成完整代码
    };
    //我们存放所有题目大致信息的文件
    const std::string questins_list = "./questions/questions.list";
    //获取题目的路径
    const std::string questins_path = "./questions/";

    class Model
    {
    private:
        //题号 : 题目细节
        unordered_map<string, Question> questions;
    public:
        Model()
        {
            assert(LoadQuestionList(questins_list));
        }
        bool LoadQuestionList(const string &question_list)
        {
            //加载配置文件: questions/questions.list + 题目编号文件
            ifstream in(question_list);
            if(!in.is_open())
            {
                LOG(FATAL) << "题库加载失败" << "\n";
                return false;
            }

            string line;
            while(getline(in, line))
            {
                vector<string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                if(tokens.size() != 5)
                {
                    LOG(WARNING) << "加载部分题目失败, 请检查文件格式" << "\n";
                    continue;
                }
                Question q;
                q.number = tokens[0];
                q.title = tokens[1];
                q.star = tokens[2];
                q.cpu_limit = atoi(tokens[3].c_str());
                q.mem_limit = atoi(tokens[4].c_str());

                string path = questins_path;
                path += q.number;
                path += "/";

                FileUtil::ReadFile(path+"desc.txt", &(q.desc), true);
                FileUtil::ReadFile(path+"header.cpp", &(q.header), true);
                FileUtil::ReadFile(path+"tail.cpp", &(q.tail), true);

                questions.insert({q.number, q});
            }
            LOG(INFO) << "加载题库...成功!" << "\n";
            in.close();

            return true;
        }
        bool GetAllQuestions(vector<Question> *out)
        {
            if(questions.size() == 0)
            {
                LOG(ERROR) << "用户获取题库失败" << "\n";
                return false;
            }
            for(const auto &q : questions){
                out->push_back(q.second); //first: key, second: value
            }

            return true;
        }
        bool GetOneQuestion(const std::string &number, Question *q)
        {
            const auto& iter = questions.find(number);
            if(iter == questions.end()){
                LOG(ERROR) << "用户获取题目失败, 题目编号: " << number << "\n";
                return false;
            }
            (*q) = iter->second;
            return true;
        }
        ~Model(){}
    };
}
