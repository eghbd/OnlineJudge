#include <iostream>
#include <signal.h>
#include <string>

#include "../common/httplib.h"
#include "control.hpp"

using namespace httplib;
using namespace oj_control;

int main()
{
    Server svr;
    Control ctrl; // 这里定义的 ctrl

    // 路由1：题目列表（前端）
    svr.Get("/questionsList", [&ctrl](const Request &req, Response &resp)
    {
        std::string html;
        ctrl.QuestionsList(&html);
        resp.set_content(html, "text/html; charset=utf-8");
    });

    // 路由2：单个题目（前端）
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
    {
        std::string num = req.matches[1];
        std::string html;
        ctrl.GetOneQuestion(num, &html);
        resp.set_content(html, "text/html; charset=utf-8");
    });

    // 路由3：运行编译（前端调后端）
    svr.Post(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp)
    {
        std::string num = req.matches[1];
        std::string result_json;
        ctrl.Judge(num, req.body, &result_json);
        resp.set_content(result_json, "application/json;charset=utf-8");
    });

    svr.set_base_dir("./Home");
    svr.set_mount_point("/Interface", "./Interface");
    //启动端口8080（先启动编译服务先，编译服务端口就设置成8081）
    svr.listen("0.0.0.0", 8080);

    return 0;
}