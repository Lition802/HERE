// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "framework.h"
#pragma warning(disable:4996)
using namespace std;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
void entry();

extern "C" {
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
        try {
            entry();
        }
        catch (EXCEPTION_DEBUG_INFO c) {
            std::cout <<c.ExceptionRecord.ExceptionInformation << "\n";
        }

    }
}


Vec3 getplayerpos(string n) {
    auto list = liteloader::getAllPlayers();
    auto p = Vec3();
    p.x = 0;
    p.y = 0;
    p.z = 0;
    for (size_t i = 0; i < list.size(); i++)
    {
        if (list[i]->getNameTag() == n)
            return list[i]->getPos();
    }
    return p;
}

string getdimname(int i) {
    switch (i) {
    case 0:
        return u8"主世界";
    case 1:
        return u8"下界";
    case 2:
        return u8"末地";
    default:
        return u8"未知";
    }
}

std::vector<std::string> split(std::string str, std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str += pattern;//扩展字符串以方便操作
    size_t size = str.size();
    for (size_t i = 0; i < size; i++)
    {
        pos = str.find(pattern, i);
        if (pos < size)
        {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

bool oncmd_wow(CommandOrigin const& ori, CommandOutput& outp) {
    auto p = getplayerpos(ori.getName());
    string pos = "("+ split(std::to_string(p.x),".")[0] + "," + split(std::to_string(p.y), ".")[0] + "," + split(std::to_string(p.z), ".")[0] + ")";
    int d = ori.getEntity()->getDimensionId();
    liteloader::runcmdEx("tellraw @a {\"rawtext\":[{\"text\":\"" + ori.getName() + u8" 位于 " +getdimname(d) +u8" 的 "+ pos + "\"}]}");
    return false;
}

void LOG(string m) {
    std::time_t secSinceEpoch = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());    //Seconds since the Epoch
    struct tm* calendarTime = localtime(&secSinceEpoch);    //转换成本地日历时间
    char usrdefFormat[50] = { 0 };    //自定义格式存储位置
    strftime(usrdefFormat, 50, "%Y-%m-%d %H:%M:%S", calendarTime);    //格式：2019-11-01 13:55:25
    cout <<"["<< string(usrdefFormat) <<" INFO][HERE] "<< m << std::endl;;
}
void PostInit(PostInitEV ev) {
    LOG("init!");
    LOG("supports for 1.17.11.01 or higher");
    LOG("v1.2.1 By Lition");
}

void entry() {
    Event::addEventListener([](RegCmdEV ev) {
        CMDREG::SetCommandRegistry(ev.CMDRg);
        MakeCommand("here", "send your pos",0);//注册指令
        CmdOverload(here, oncmd_wow);//重载指令
   });
   Event::addEventListener(PostInit);
    //Event::addEventListener(cmd);
}
