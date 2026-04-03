//模型
//负责题库的加载、存储和数据查询
#pragma once
#include <iostream>

#include "../common/log.hpp"
#include "../common/util.hpp"

namespace oj_model
{
    using namespace oj_log;
    using namespace oj_util;
    struct Question
    {
        std::string num; // 题号
        std::string name;   // 题目名
        std::string star;   // 难度
        std::string desc;   // 题目描述
        int cpu_limit = 0;  // cpu限制
        int mem_limit = 0;  // 内存限制
        // 后续需要对写完的代码与测试代码进行拼接
        std::string header; // 预设代码
        std::string tail;   // 测试代码
    };
    //题目路径
    const std::string question = "./questions/";
    //题目列表
    const std::string questionList = "./questions/questions.list";
    class Model
    {
    private:
        //用于接收所有题目的容器
        std::unordered_map<std::string, Question> questions;

    public:
        //初始工作
        Model()
        {
            if(!LoadQuestionList(questionList))
            {
                throw std::runtime_error("获取列表失败");
            }
            LOG(INFO) << "加载题目数量：" << questions.size();
        }
        // 显式声明默认析构
        ~Model() = default;
        //禁用拷贝与赋值
        Model(const Model&) = delete;
        Model& operator = (const Model&) = delete;

    public:
        bool GetAllQuestions(std::vector<Question>* out)
        {
            if(questions.empty())
            {
                LOG(ERROR) << "列表为空";
                return false;
            }
            // 预分配空间
            // 避免 vector 在 push_back 过程中因容量不足而进行多次内存重新分配和拷贝
            out->reserve(questions.size());
            for(const auto& i : questions)
            {
                out->push_back(i.second);
            }
            return true;
        }

        bool GetQuestion(const std::string& num, Question& q) const
        {
            const auto temp = questions.find(num);
            if(temp == questions.end())
            {
                LOG(ERROR) << "题目" << num << "不存在";
                return false;
            }
            q = temp -> second;
            return true;
        }

    private:
        // 加载数据库对应的核心逻辑
        // 获取对应题目的所有相关信息
        // 这就是model核心，先行加载模型，然后提供接口给control进行控制，配合view展现数据
        bool LoadQuestionList(const std::string& question_file)
        {
            // ============ 读取文件 ======================
            std::ifstream in(question_file);
            if(!in.is_open())
            {
                LOG(FATAL) << "打开" << question_file << "文件失败";
                return false;
            }
            std::string line;
            while(getline(in, line))
            {
                std::vector<std::string> infor;
                //例子：1 有效的括号 简单 1 30000
                StringUtil::SplitString(line, &infor, " ");
                if(infor.size() != 5)
                {
                    LOG(WARNING) << "该题目的列表信息不完整";
                    continue;
                }
                Question q;
                q.num = infor[0];
                q.name = infor[1];
                q.star = infor[2];
                //可能出现转换失败的情况
                //通过try进行尝试，失败就执行catch内容
                try{
                    q.cpu_limit = std::stoi(infor[3]);
                    q.mem_limit = std::stoi(infor[4]);
                } catch (...)
                {
                    LOG(WARNING) << "转换失败";
                    continue;
                }

                // 判断文件是否存在的同时，获取文件内的内容
                std::string path = question;
                path = path + q.num + "/";
                bool temp = true;
                temp &= FileUtil::ReadFile(path + "desc.txt", &q.desc, true);
                temp &= FileUtil::ReadFile(path + "header.cpp", &q.header, true);
                temp &= FileUtil::ReadFile(path + "tail.cpp", &q.tail, true);
                if(temp == false)
                {
                    LOG(WARNING) << "读取文件" << q.num << "失败";  
                    continue;
                }
                // 用move函数，可以避免拷贝问题
                questions[q.num] = std::move(q);
            }
            LOG(INFO) << "列表获取成功";
            return true;
        }
    };
}







