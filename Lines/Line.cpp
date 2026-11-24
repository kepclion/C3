#include "../allheader.h"
#include "../Animation.h"
#include "../Story.h"
#include "../Game.h"
#include "../SaveData.h"
#include "../ConsoleUtils.h"
#include "Line.h"
#include "../Secret.h"

#define DELAY 27


// ------------------------------------------------------------------
// 序章主流程
// ------------------------------------------------------------------
void prelude_intro()
{
    CInputWhenCorrect("请输入密码: ", 1, 6, Secrets::kInitPassword,
                      "<lred>验证不通过</lred>\n", "<lgreen>验证通过</lgreen>\n");
    LineLoading("接入线路中 :", 100, "*", "·", 24, 50, 15);

    {
        PrintLn s;
        s.content = "已接入到线路 [K203] 端口: 8083\n";
        s.delay   = 5;
        s.CPrint();
    }

    // M1xTri 的开场白
    {
        PrintLn t;
        t.content = "~~Hello, we are the Kryptos tea";
        t.speaker = "M1xTri";
        t.hid     = 1;

        CPrintThenBack({
            .forward   = t,
            .finalText =
            "你好~~，我们是 Kryptos 团队。\n"
            "~~你正在用的是我们的内部成员专用客户端\n"
            "~~虽然不知道你是通过什么渠道获取到的\n"
            "~~但你能来到这里~~，多少有点能力\n"
            "~~我们可以考虑让你加入我们\n"
            "~~不过我们还需要对你进行一次能力测验。",
            .delayForward = 20,
            .delayBack    = 5,
            .delayFinal   = DELAY,
            .waitForward  = 100,
            .waitBack     = 100,
        });
    }
    printf("\n");

    Sleep(1000);
    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "~~欢迎新小孩";
        t.delay   = DELAY;
        t.speaker = "abc_allac";
        t.CPrint();
        printf("\n");
    }

    Sleep(1000);
    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "~~嘿~~，<lyellow>@DEFAULT</lyellow>~~，\n~~记住下面这串编码。~~~我只发一遍。";
        t.speaker = "M1xTri";
        t.delay   = DELAY;
        t.CPrint();
        printf("\n");
    }

    // 六段闪现（原地擦除，不推进光标，无需翻页检查）
    {
        const std::vector<std::string> flashes = {
            "J2tlc ----------",
            "Gxlcn ----------",
            "onLmx ----------",
            "hbnpv ----------",
            "dXUuY29tL2 ----------",
            "IwMWV1cG1wcGU= ----------"
        };
        const std::vector delays = {500, 400, 300, 250, 200, 175};

        PrintLn t;
        t.speaker = "";
        t.hid     = 1;
        for (size_t i = 0; i < flashes.size(); ++i)
        {
            t.content = flashes[i];
            t.delay   = delays[i];
            t.ReverseCPrint();
        }
    }

    Sleep(1000);
    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "~~记住了么？";
        t.delay   = DELAY;
        t.speaker = "M1xTri";
        t.CPrint();
        printf("\n");
    }

    Sleep(1200);
}

// ------------------------------------------------------------------
// 分支：拒绝加入
// ------------------------------------------------------------------
void prelude_intro_refuse()
{
    PrintLn s;
    s.content = "666发的什么东西";
    s.speaker = "DEFAULT";
    s.delay   = 0;
    s.CPrint();
    printf("\n");

    Sleep(1000);
    EnsureRoomForNewPage();

    s.content = "~~看来你并不适合加入我们~~~，请退吧。~~\n";
    s.delay   = DELAY;
    s.speaker = "M1xTri";
    s.CPrint();
    printf("\n");

    Sleep(1500);
    exit(0);
}

// ------------------------------------------------------------------
// 分支：同意加入
// ------------------------------------------------------------------
void prelude_intro_agree(SaveData& saves)
{
    PrintLn s;
    s.content = "记住了";
    s.speaker = "DEFAULT";
    s.delay   = 0;
    s.CPrint();
    printf("\n");

    Sleep(1000);
    EnsureRoomForNewPage();

    s.content = "~~抓小孩吃";
    s.speaker = "abc_allac";
    s.delay   = DELAY;
    s.CPrint();
    printf("\n");

    Sleep(1000);
    EnsureRoomForNewPage();

    s.content = "~~<lyellow>@DEFAULT</lyellow>, 我现在断开你的连接，\n你用你的账号密码，再重新登录一下。";
    s.speaker = "M1xTri";
    s.delay   = DELAY;
    s.CPrint();
    printf("\n");
    EnsureRoomForNewPage();


    s.content = "<lred>--- 临时连接已断开！请重新连接 ---</lred>\n";
    s.speaker = "";
    s.delay   = 0;
    s.CPrint();
    EnsureRoomForNewPage();

    ull res = 0;
    CChoice({"重新连接"}, res, ChoiceStyle::VNone);

    s.content = "<lred>--- 正在重新连接到远程服务器 ---</lred>\n";
    s.speaker = "";
    s.delay   = 5;
    s.CPrint();
    Sleep(900);

    saves.preludeProgress_.setPrelude(Agreed);
    saves.Save();
}

// ------------------------------------------------------------------
// 登录流程
// ------------------------------------------------------------------
void GameSignIn(const SaveData& saves)
{
    (void) saves;
    ClearConsole();

    CirLoading(13, "连接到 156.81.26.143 ...");

    {
        PrintLn s;
        s.content = "<lgreen>--- 登录系统已激活 ---</lgreen>\n";
        s.delay   = 10;
        s.CPrint();
    }

    std::string acc, pswd;
    const auto [X, Y] = GetConsoleCursorXY();

    PrintLn err;
    err.content = "\n<lred>用户名或密码错误！</lred> (按任意键重试)\n";
    err.delay   = 0;

    while (true)
    {
        SetConsoleCursorXY(X, Y);
        std::cout << std::string(60, ' ') << '\n'
                << std::string(60, ' ') << '\n'
                << std::string(60, ' ') << '\n';
        SetConsoleCursorXY(X, Y);

        CInputTo("请输入用户名：", 0, 0, acc);
        printf("\n");
        CInputTo("请输入密码：", 1, 0, pswd);

        if (acc == Secrets::kSiriusLoginUser and pswd == Secrets::kSiriusLoginPassword)
        {
            SURFACE = SiriusMenu;
            return;
        }

        if (acc == Secrets::kMixLoginUser and pswd == Secrets::kMixLoginPassword)
        {
            SURFACE = MixMenu;
            return;
        }

        err.CPrint();
        _getch();
    }
}

// ------------------------------------------------------------------
// KRP 聊天室
// ------------------------------------------------------------------
void KRPhistory_0(SaveData& saves)
{
    int delay = 50;
    if (saves.storyProgress_.isStoryRead(Story_0)) delay = 0;

    {
        PrintLn s;
        s.content = "<grey>[ 聊天记录1 ]</grey>\n";
        s.delay   = 0;
        s.CPrint();
    }

    const std::pair<const std::string, const std::string> lines[] = {
        {"~~此手段是否有些卑劣？", "cden"},
        {"~~这是任务，~执行即可，~不用多管", "M1xTri"},
        {"~~不不~~，我想说~~，难道非做不可吗", "cden"},
        {"~~若做不到~~，把机会留给别人吧", "M1xTri"},
        {"~~行~~，我放弃", "cden"},
    };
    for (const auto& [text, speaker]: lines)
    {
        EnsureRoomForNewPage();

        PrintLn t;
        t.content = text;
        t.speaker = speaker;
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }

    if (!saves.storyProgress_.isStoryRead(Story_0))
    {
        saves.storyProgress_.markStoryRead(Story_0);
        saves.Save();
    }
}

void KRPhistory_1(SaveData& saves)
{
    int delay = 50;
    if (saves.storyProgress_.isStoryRead(Story_1)) delay = 0;

    printf("\n");
    {
        PrintLn s;
        s.content = "<grey>[ 聊天记录2 ]</grey>\n";
        s.delay   = delay;
        s.CPrint();
    }

    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "嗯哼？~~那个新来的好像还有点实力。\n"
                "~~这合同可不简单\n"
                "~~<lyellow>@SiriusTi</lyellow> 怎能这么快就解决了？";
        t.speaker = "Kyota";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }

    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "嗯~~，可以的";
        t.speaker = "R30N";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }

    if (!saves.storyProgress_.isStoryRead(Story_1))
    {
        saves.storyProgress_.markStoryRead(Story_1);
        saves.Save();
    }
}

void KRPhistory_2(SaveData& saves)
{
    int delay = 50;
    if (saves.storyProgress_.isStoryRead(Story_2)) delay = 0;

    printf("\n");
    {
        PrintLn s;
        s.content = "<grey>[ 聊天记录3 ]</grey>\n";
        s.delay   = delay;
        s.CPrint();
    }

    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "~~为什么破个压缩包密码这件事都能登上委托\n"
                "~~谁把家庭作业发这来了？\n"
                "这压缩包密码还没我账号密码复杂";
        t.speaker = "Kyota";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }
    EnsureRoomForNewPage();
    {
        PrintLn t;
        t.content = "~~我密码也挺复杂的，我都记不住";
        t.speaker = "R30N";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }
    EnsureRoomForNewPage();

    {
        PrintLn t;
        t.content = "~~话说 mix 曾经跟我说过他的密码一直是个谜~~~~";
        t.speaker = "R30N";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }
    EnsureRoomForNewPage();
    {
        PrintLn t;
        t.content = "我去~，怎么撤回不了";
        t.speaker = "R30N";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }
    {
        PrintLn t;
        t.content = "~~勤撤";
        t.speaker = "Lra";
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }


    if (!saves.storyProgress_.isStoryRead(Story_2))
    {
        saves.storyProgress_.markStoryRead(Story_2);
        saves.Save();
    }
}

void KRPhistory_3(SaveData& saves)
{
    int delay = 50;
    if (saves.storyProgress_.isStoryRead(Story_4)) delay = 0;

    {
        PrintLn s;
        s.content = "<grey>[ 聊天记录4 ]</grey>\n";
        s.delay   = 0;
        s.CPrint();
    }

    const std::pair<const std::string, const std::string> lines[] = {
        {"~~怎么还有盒武器", "Kyota"},
        {"~~<lyellow>@SiriusTi</lyellow> 牛逼，这都会", "cden"},
        {"~~可以可以", "ahah"},
    };
    for (const auto& [text, speaker]: lines)
    {
        EnsureRoomForNewPage();

        PrintLn t;
        t.content = text;
        t.speaker = speaker;
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }

    if (!saves.storyProgress_.isStoryRead(Story_3))
    {
        saves.storyProgress_.markStoryRead(Story_3);
        saves.Save();
    }
}


void KRPhistory_4(SaveData& saves)
{
    int delay = 50;
    if (saves.storyProgress_.isStoryRead(Story_4)) delay = 0;

    {
        PrintLn s;
        s.content = "<grey>[ 聊天记录5 ]</grey>\n";
        s.delay   = 0;
        s.CPrint();
    }

    const std::pair<const std::string, const std::string> lines[] = {
        {"~~哈哈哈笑死我了谁家苦逼大学生", "Kyota"},
        {"~~XD", "cden"},
        {"~~细节XD", "Kyota"},
    };
    for (const auto& [text, speaker]: lines)
    {
        EnsureRoomForNewPage();

        PrintLn t;
        t.content = text;
        t.speaker = speaker;
        t.delay   = delay;
        t.CPrint();
        printf("\n");
    }

    if (!saves.storyProgress_.isStoryRead(Story_4))
    {
        saves.storyProgress_.markStoryRead(Story_4);
        saves.Save();
    }
}
