#include <iostream>
#include <set>
#include <sstream>


#include <cqcppsdk/cqcppsdk.h>

#include <cstdlib>
#include <string>

using namespace cq;
using namespace std;
using Message = cq::message::Message;
using MessageSegment = cq::message::MessageSegment;

int group_amount = 3;

string helpInfo =
    "Instruction format: /debug (or /db [parameter])\n"
    "-v or --Version\tCheck the current version\n"
    "-i or --Info\tCheck login user's information\n"
    "-p or --Picture\tCheck if pictures can be sent\n"
    "-t or --Tailor [group_id] [message]\tSend tailored message to specific group (only in text)";
string errorInfo = 
    "Parameter is incorrect or illegal，Enter ./debug or ./db for help";

string JudgeMessage(string message, int64_t target);
bool AllisNum(string str);
int str2num(string str);
bool JudgeGroupID(int64_t group_id);

string JudgeMessage(string message, int64_t target) //判断消息是否为调用debug
{
    if (message.substr(0, 6) == "/debug" || message.substr(0, 3) == "/db") //判断是否有前缀
    {
        string blank = " ";
        int blankCount = 0;
        int searchPosition = -1;
        int blankPosition[10];
        while ((searchPosition = message.find(blank, searchPosition + 1)) != string::npos) {
            blankCount++;
            blankPosition[blankCount] = searchPosition;
            searchPosition = searchPosition + blank.length();
        }
        if (blankCount == 1) 
        {
            return "EXECUTE";
        }else if(blankCount == 3){
            string flag = message.substr(blankPosition[1] + 1, blankPosition[2] - blankPosition[1] - 1);
            string temp = message.substr(blankPosition[2] + 1, blankPosition[3] - blankPosition[2] - 1);
            int64_t group_id;
            string text = message.substr(blankPosition[3] + 1);
            if(flag == "-t" || flag == "--Tailor"){
                if(AllisNum(temp)){
                    group_id = str2num(temp);
                    if (JudgeGroupID(group_id)){
                        send_group_message(group_id, text);
                        send_private_message(target, "Sent successfully");
                    }else
                        send_private_message(target, "Error: NijiBot is not a member of the group!");
                }else{
                    send_private_message(target, "Error: Not a group_id!");
                }
            }else{
                return "ERROR";
            }
            
            return "EXECUTED";
        }else if(blankCount == 0){
            return "HELP";
        }
        else {
            return "ERROR";
        }

    } else //如果不具有前缀则直接不处理
    {
        return "MISS";
    }
}


bool AllisNum(string str) {  
    for (int i = 0; i < str.size(); i++)
    {
        int tmp = (int)str[i];
        if (tmp >= 48 && tmp <= 57)
        {
            continue;
        }
        else
        {
            return false;
        }
    } 
    return true;
}

int str2num(string str)
{
	int num;
	stringstream ss(str);
	ss >> num;
	return num;
}

bool JudgeGroupID(int64_t group_id){
    const vector group_list = get_group_list();
    int i;
    for(i = 0; i < group_amount; i++)
        if(group_id == group_list[i].group_id){
            return true; 
        }
    if(i == group_amount)
        return false;
}

CQ_INIT {
    on_enable([] { logging::info("启用", "NijiBot已启用"); });

    on_message([](const MessageEvent &event) {
        try{
            stringstream applist;
            string msg1, msg2, app1, app2, app3, app4, app5, app6;
            app1 = "\t·计算器：\"SolveInPic 表达式\"\n";
            app2 = "\t·点歌机：\"我想听/来首/点歌 歌名\"\n";
            app3 = "\t·翻译：\"翻译 翻译文本\"\n";
            app4 = "\t·搜题：\"搜题 题目\"\n";
            app5 = "\t·Pixiv搜图：\"搜图 ID/关键词 | 以图搜图\"\n";
            app6 = "\t·Debugger: ./debug (only in private chat)\n";

            if (event.message == "应用列表"){
                applist << "NijiBot搭载的应用及其指令：\n" << app1 << app2 << app3 << app4 << app5 << app6 << "回复\"详情 <应用名>\"以获取应用详情";
                send_message(event.target, applist.str()); 
            }

            if (event.message == "详情 计算器"){
                msg1 = "接口网站：https://www.wolframalpha.com/ \n(需梯子，大部分高数涉及的计算都能解决，还有很多其他功能)";
                msg2 = Message("支持的计算及输入规范：\n[CQ:image,file=SuperCalculator.jpg]");
                send_message(event.target, msg1); 
                send_message(event.target, msg2);
            }

            if (event.message == "详情 点歌机"){
                msg1 = "接口：QQ音乐";
                send_message(event.target, msg1); 
            }

            if (event.message == "详情 翻译"){
                msg1 = "接口：有道翻译\n支持中译英、多语言译中";
                send_message(event.target, msg1); 
            }

            if (event.message == "详情 搜题"){
                msg1 = "接口：别人家的接口\n搜不到的话自己百度去";
                send_message(event.target, msg1); 
            }
            
            if (event.message == "详情 Pixiv搜图"){
                msg1 = "接口：Pixivic(https://pixivic.com/)\n指令输入是：搜图 <ID/关键词> 或 以图搜图";
                send_message(event.target, msg1);
            } 

        }catch (ApiError &err) { 
            logging::warning("所有", "消息回复失败, 错误码: " + to_string(err.code));
        }
        event.block();
    });

    on_private_message([](const PrivateMessageEvent &event) {
        /*
        static const set<int64_t> PERMITTED_USERS = {1104416052};
        if (PERMITTED_USERS.count(event.user_id) == 0) {
            send_private_message(event.user_id, "PERMISSION DENIED");
            return;
        }
        */
        try{
            string returnInfo = "";
            returnInfo = JudgeMessage(event.message, event.user_id);
            if (returnInfo == "EXECUTE") {
                try{
                    if(event.message == "/debug -v" || event.message == "/debug --Version"){
                        send_message(event.target, "Version: Beta1.0.1\nUpdate Log:https://github.com/KinNiji/NijiBot/blob/UpdateLog/Beta1.0.1.txt");
                        event.block();
                    }else if(event.message == "/debug -i" || event.message == "/debug --Info"){
                        stringstream info;
                        string info1, info2, info3;
                        info1 = "Login user id: " + to_string(get_login_user_id());
                        info2 = "\nLogin user nickname: " + to_string(get_login_nickname());
                        info3 = "";
                        info << info1 << info2 << info3;
                        send_message(event.target, info.str());
                        event.block();
                    }else if(event.message == "/debug -p" || event.message == "/debug --Picture"){
                        bool flag = false;
                        if(can_send_image()) flag = true;
                        send_message(event.target, to_string(flag));
                        event.block();
                    }else if(event.message == "/debug -t" || event.message == "/debug --Tailor"){
                        send_message(event.target, "parameter:\n""-t or --Tailor [group_id] [message]\tSend tailored message to specific group (only in text & private mod)");
                        event.block();
                    }else {
                        send_private_message(event.user_id, errorInfo);
                    }
                }catch (ApiError &err) { 
                    logging::warning("Debug", "消息回复失败, 错误码: " + to_string(err.code));
                }
                return;
            } else if (returnInfo == "ERROR") {
                send_private_message(event.user_id, errorInfo);
                return;
            }else if (returnInfo == "HELP"){
                send_private_message(event.user_id, helpInfo);
                return;
            }else{
                return;
            }
            event.block();
        }catch (ApiError &err) { 
            logging::warning("Debug", "消息回复失败, 错误码: " + to_string(err.code));
        }
    });
    
    on_group_message([](const GroupMessageEvent &event) {
        static const set<int64_t> BANNED_GROUPS = {114514};
        if (BANNED_GROUPS.count(event.group_id) == 1) return; // 在未启用的群中, 忽略
        
        try{
            const auto msg = Message(event.message); // 从消息事件的消息内容解析 Message 对象
            for (const auto &seg : msg) { // 遍历消息段
                if (seg == MessageSegment::at(get_login_user_id())) { // 发现 at 消息段
                    send_message(event.target, "NijiBot会优先处理群聊中出现的@NijiBot，并留下一些帮助：\n\t·要查看应用就输应用列表，要聊天的话NijiBot一直都在~\n\t·所有群聊指令私聊也有效~");
                    send_message(event.target, "NijiBot会对聊天内容字段进行自动识别，不需要每次都@哦\n(哪个B不长眼的一直@NijiBot那NijiBot只能祝Ta马飞了)");
                    break;
                }
            }
        
            if (event.is_anonymous()) {
                try{
                    try {
                        const auto mi = get_group_member_info(event.group_id, get_login_user_id(), true);
                        if (mi.role == GroupRole::MEMBER) {
                         try {
                            send_group_message(event.group_id, Message("[CQ:image,file=shale.jpg]"));
                            send_group_message(event.group_id, "等NijiBot有权限了，迟早回来把匿名的B都鲨了");
                        } catch (ApiError &) {}
                        return;
                        }
                    } catch (ApiError &) {}
                    try {
                        const auto mi = get_group_member_info(event.group_id, event.user_id);
                        if (mi.role != GroupRole::MEMBER) {
                            try {
                                send_group_message(event.group_id, " 匿名的是是是是，是群主或者管理。。。");
                            } catch (ApiError &) {
                            }
                            return;
                        }
                    } catch (ApiError &) {}
                    set_group_anonymous(event.group_id, false);
                    send_group_message(event.group_id, Message("[CQ:image,file=shale.jpg]"));
                    send_group_message(event.group_id, "匿名通过Base64加密，鲨不了鲨不了ヽ( >д<)ｐ");
                    //set_group_anonymous_ban(event.group_id, event.anonymous, 30 * 60);
                    //send_group_message(event.group_id, "匿名的B已经被禁言了，yes！");
                }catch (ApiError &err){
                    logging::warning("匿名" + event.group_id, "对匿名操作失败, 错误码: " + to_string(err.code));
                }
                logging::info("群聊", "消息是匿名消息, 匿名昵称: " + event.anonymous.name);
            }  
        } catch (ApiError &err) { 
            logging::warning("群聊" + event.group_id, "群聊消息回复失败, 错误码: " + to_string(err.code));
        }
    });

    on_group_upload([](const GroupUploadEvent &event) { 
        stringstream ss;
        string at_sb = MessageSegment::at(event.user_id);
        ss << at_sb << "上传了一个文件, 文件名: " << event.file.name << ", 大小(字节): " << event.file.size << "\n感谢对群文件的贡献~";
        try {
            send_message(event.target, ss.str());
            event.block();
        } catch (ApiError &err) {
            logging::warning("群文件" + event.group_id, "群文件上传反馈失败, 错误码: " + to_string(err.code));
        }
    });

    on_group_admin([](const GroupAdminEvent &event){
        const auto mi = get_group_member_info(event.group_id, get_login_user_id(), true);
        if (event.user_id == get_login_user_id() && mi.role != GroupRole::MEMBER) {
            try {
                send_group_message(event.group_id, "感谢提拔~ NijiBot一定会认真工作的！");
                event.block();
            } catch (ApiError &err) {
                logging::warning("管理员" + event.group_id, "任命管理员回复失败, 错误码: " + to_string(err.code));
            }
        }
        else{
            try {
                send_group_message(event.group_id, "NijiBot做错了什么！！！！！！");
                send_group_message(event.group_id, Message("[CQ:image,file=liulei.jpg]"));
                event.block();
            } catch (ApiError &err) {
                logging::warning("管理员" + event.group_id, "失去管理员回复失败, 错误码: " + to_string(err.code));
            }
        }
    });

    on_group_ban([](const GroupBanEvent &event){
        try {
            send_group_message(event.group_id, "哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈嗝");
            event.block();
        } catch (ApiError &) {
        }
    });

    on_friend_request([](const FriendRequestEvent &event){
        try {
            set_friend_request(event.flag, RequestEvent::Operation::APPROVE, "NijiBot为您服务，当前版本Beta1.0.1");
            event.block();
        } catch (ApiError &) {
        }
    });

    on_group_request([](const GroupRequestEvent &event){
        try {
            set_group_request(event.flag, event.sub_type, RequestEvent::Operation::APPROVE, "欢迎新成员~");
            set_group_card(event.group_id, get_login_user_id(), "NijiBot");
            event.block();
        } catch (ApiError &) {
        }
    });


}

CQ_MENU(menu_demo_1) {
    logging::info("菜单", "测试");
}

CQ_MENU(menu_demo_2) {
    send_private_message(1104416052, "测试");
}

