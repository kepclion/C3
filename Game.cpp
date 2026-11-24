#include "allheader.h"
#include "Animation.h"
#include "ConsoleUtils.h"
#include "Story.h"
#include "Game.h"
#include "SaveData.h"
#include "Secret.h"
#include "Lines/Line.h"

//note 展示界面
void SurfaceShow(const GameSurface surface, SaveData& saves)
{
    switch (surface)
    {
        case MainMenu:
            GameMenu();
            break;

        case Start:
            if (saves.preludeProgress_.prelude_ == Agreed) SURFACE = SignIn;
            else if (saves.preludeProgress_.prelude_ == LoggedIn) SURFACE = SiriusMenu;
            else GameMenuStart(saves);
            break;

        case About:
            GameMenuAbout();
            SURFACE = MainMenu;
            break;

        case Help:
            GameMenuHelp();
            SURFACE = MainMenu;
            break;

        case SignIn:
            GameSignIn(saves);
            break;

        case SiriusMenu:
            GameSirius(saves);
            break;

        case SiriusChatRoom:
            KChatRoom_SiriusTi(saves);
            break;

        case SiriusContract:
            Contract(saves);
            break;

        case SiriusMail:
            Mailbox(saves);
            break;

        case EXIT:
            exit(0);
            break;

        case GameCheatTaskSurface:
            GameCheatTaskDetails();
            break;

        case DecodeZipTaskSurface:
            GameDecodeZipTaskDetails();
            break;

        case FindQQClueTaskSurface:
            FindQQClueTaskDetails();
            break;

        case DocumentFaultTaskSurface:
            GameDocumentFaultTaskDetails();
            break;

        case HeartKillerTaskSurface:
            GameHeartKillerDetails(save);
            break;

        case MixMenu:
            GameMix(save);
            break;

        case MixChatRoom:
            KChatRoom_Mix(save);
            break;

        case MixContract:
            ContractMix(save);
            break;

        case MixMail:
            MailboxMix(save);
            break;

        case HeartKillerTaskSurfaceMix:
            GameHeartKillerDetailsMix(save);
            break;

        case MidEnd:
            GameMidEnd(save);
            break;
    }
}

//Sirius界面
void GameSirius(SaveData& saves)
{
    ClearConsole();
    if (saves.preludeProgress_.prelude_ != LoggedIn)
    {
        saves.preludeProgress_.setPrelude(LoggedIn);
        saves.Save();
    }

    //打印 --- 欢迎 SiriusTi --- 头头
    {
        PrintLn s;
        s.content = "<lgreen>--- 欢迎 SiriusTi ---</lgreen>\n";
        s.delay   = 1;
        s.CPrint();
    }
    printf("\n");

    //打印等级
    {
        PrintLn s;
        switch (saves.levelProgress_.level_)
        {
            case 0:
                s.content = "<lwhite>当前等级:</lwhite> <grey>"
                            + std::to_string(saves.levelProgress_.level_) + "</grey>\n";
                break;
            case 1:
                s.content = "<lwhite>当前等级:</lwhite> "
                            + std::to_string(saves.levelProgress_.level_) + "\n";
                break;
            case 2:
                s.content = "<lwhite>当前等级:</lwhite> <blue>"
                            + std::to_string(saves.levelProgress_.level_) + "</blue>\n";
                break;
            case 3:
                s.content = "<lwhite>当前等级:</lwhite> <yellow>"
                            + std::to_string(saves.levelProgress_.level_) + "</yellow>\n";
                break;
            case 4:

                s.content = "<lwhite>当前等级:</lwhite> <red>"
                            + std::to_string(saves.levelProgress_.level_) + "</red>\n";
                break;
            default: break;
        }
        s.CPrint();
    }
    printf("\n");

    ull resu = 0;
    CChoice({"聊天室", "合同", "邮箱", "退出登录"}, resu, ChoiceStyle::Menu, Secrets::kSiriusLoginUser);

    switch (resu)
    {
        case 0: SURFACE = SiriusChatRoom;
            break;
        case 1:
            if (saves.levelProgress_.level_ == 4 and !saves.midEndProgress_.isMidEnd)
                SURFACE  = MidEnd;
            else SURFACE = SiriusContract;
            break;
        case 2: SURFACE = SiriusMail;
            break;
        case 3:
        default: SURFACE = SignIn;
            break;
    }
}

//M1xTri界面
void GameMix(SaveData& saves)
{
    ClearConsole();

    //打印 --- 欢迎 M1xTri --- 头头
    {
        PrintLn s;
        s.content = "<lpurple>--- 欢迎 M1xTri ---</lpurple>\n";
        s.delay   = 1;
        s.CPrint();
    }
    printf("\n");

    //打印等级
    {
        PrintLn s;
        s.content = "<lwhite>当前等级:</lwhite> <purple>8+</lpurple>\n";
        s.CPrint();
    }
    printf("\n");

    ull resu = 0;
    CChoice({"聊天室", "合同", "邮箱", "退出登录"}, resu, ChoiceStyle::Menu, Secrets::kSiriusLoginUser);

    switch (resu)
    {
        case 0: SURFACE = MixChatRoom;
            break;
        case 1: SURFACE = MixContract;
            break;
        case 2: SURFACE = MixMail;
            break;
        case 3:
        default: SURFACE = SignIn;
            break;
    }
}


void KChatRoom_SiriusTi(SaveData& saves)
{
    ClearConsole();

    {
        PrintLn s;
        s.content = "<lgreen>--</lgreen> <lyellow>Kryptos聊天室</lyellow> "
                "<lgreen>当前身份:</lgreen> <lwhite>SiriusTi</lwhite> "
                "<lgreen>--</lgreen>\n";
        s.CPrint();
        printf("\n");
    }

    //展示剧情
    KRPhistory_0(saves);
    if (saves.taskProgress_.isTaskDone(GameCheat))
        KRPhistory_1(saves);
    if (saves.taskProgress_.isTaskDone(DecodeZip))
        KRPhistory_2(saves);
    if (saves.taskProgress_.isTaskDone(FindQQClue))
        KRPhistory_3(saves);
    if (saves.taskProgress_.isTaskDone(DocumentFault))
        KRPhistory_4(saves);

    printf("\n");
    ull resu;
    CChoice({"退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    SURFACE = SiriusMenu;
}

void Contract(const SaveData& saves)
{
    ClearConsole();

    {
        PrintLn s;
        s.content = "<lgreen>---</lgreen> <lyellow>Kryptos 合同列表</lyellow> "
                "当前身份: <lwhite>SiriusTi</lwhite><lgreen>---</lgreen>\n";
        s.CPrint();
        printf("\n");
    }

    std::vector<std::string> lis;
    std::map<Tasks, std::string> task_name =
    {
        {GameCheat, "这游戏打半天打不过去"},
        {DecodeZip, "压缩包密码破解"},
        {FindQQClue, "我需要这个人的一些信息"},
        {DocumentFault, "求求了帮我看看"},
        {HeartKiller, "能不能不要搏起了"}
    };

    auto task_style = [&](const Tasks task)
    {
        if (saves.taskProgress_.isTaskDone(task))
            return "<lgreen>" + task_name[task] + "</lgreen><grey> -> 已完成，委托人已反馈</grey>";

        if (task == HeartKiller)
            return "<lred>" + task_name[task] + "</lred>";

        if (task == DocumentFault)
            return "<lyellow>" + task_name[task] + "</lyellow>";

        return "<lwhite>" + task_name[task] + "</lwhite>";
    };

    for (const auto& key: task_name | std::views::keys)
        lis.emplace_back(task_style(key));


    lis.emplace_back("<invalid><grey>无</grey>");
    lis.emplace_back("<invalid><grey>无</grey>");
    lis.emplace_back("<none>");
    lis.emplace_back("<lwhite>返回</lwhite>");

    ull resu = 0;

    CChoice(lis, resu, ChoiceStyle::Contract, "ENTER");
    switch (resu)
    {
        case 0: SURFACE = GameCheatTaskSurface;
            break;
        case 1: SURFACE = DecodeZipTaskSurface;
            break;
        case 2: SURFACE = FindQQClueTaskSurface;
            break;
        case 3: SURFACE = DocumentFaultTaskSurface;
            break;
        case 4: SURFACE = HeartKillerTaskSurface;
            break;
        case 5:
        case 6:
        default:
        case 7:
            SURFACE = SiriusMenu;
            break;
    }
}

void Mailbox(const SaveData& saves)
{
    std::string mailbox, message;
    bool sta = false;
    while (true)
    {
        ClearConsole();
        {
            PrintLn s;
            s.content = "<lgreen>--</lgreen> <lyellow>Kryptos 邮箱</lyellow> "
                    "<lgreen>当前身份:</lgreen> <lwhite>SiriusTi</lwhite> "
                    "<lgreen>--</lgreen>\n"
                    "\n"
                    "<grey> [两个输入框都空置即返回] </grey>\n";
            s.CPrint();
            printf("\n");
        }

        CInputTo("<lyellow>Enter Mailbox:<lyellow><lwhite></lwhite>", 0, 0, mailbox);
        printf("\n");
        CInputTo("<lyellow>Enter Message:<lyellow><lwhite></lwhite>", 0, 0, message);
        printf("\n");

        if (message.empty() and mailbox.empty()) break;
        for (const auto& task: MailboxAndTask)
        {
            if (task.message == message and task.mail == mailbox)
            {
                if (save.taskProgress_.isTaskDone(task.taskname))
                {
                    PrintLn s;
                    s.content = "\n<lred> - 委托不能重复提交 - </lred>\n";
                    s.CPrint();
                } else
                {
                    task.taskDone();
                    save.levelProgress_.level_++;
                    save.Save();

                    PrintLn s;
                    s.content = "\n<lgreen> - 委托完成!- </lgreen>\n";
                    s.CPrint();

                    sta = true;
                }
            }
        }

        if (sta == false)
        {
            PrintLn s;
            s.content = "\n<lred> - 委托未完成!- </lred>\n";
            s.CPrint();
        }

        _getch();
    }

    SURFACE = SiriusMenu;
}

void KChatRoom_Mix(SaveData& saves)
{
    ClearConsole();
    std::string resu;
    CInputTo("Enter ChatRoom Password:", 1, 0, resu);
    PrintLn t;
    t.content = "\n<red>密码错误!</red>\n";
    t.CPrint();
    _getch();
    SURFACE = MixMenu;
}

void ContractMix(const SaveData& saves)
{
    ClearConsole();

    {
        PrintLn s;
        s.content = "<lgreen>---</lgreen> <lyellow>Kryptos 合同列表</lyellow> "
                "当前身份: <lpurple>M1xTri</lpurple><lgreen>---</lgreen>\n";
        s.CPrint();
        printf("\n");
    }

    std::vector<std::string> lis;

    constexpr auto cden  = 100;
    constexpr auto Kyota = 200;

    std::map<Tasks, std::string> task_name =
    {
        {GameCheat, "这游戏打半天打不过去"},
        {DecodeZip, "压缩包密码破解"},
        {FindQQClue, "我需要这个人的一些信息"},
        {DocumentFault, "个人学术档案修改"},
        {HeartKiller, "能不能不要搏起了"},
        {static_cast<Tasks>(cden), "OTSN 安全预案申报"},
        {static_cast<Tasks>(cden + 1), "供应链安全排查"},
        {static_cast<Tasks>(Kyota), "数据外发行为核查"}

    };

    auto task_style = [&](const Tasks task)
    {
        if (saves.taskProgress_.isTaskDone(task))
        {
            if (task == DocumentFault)
                return "<invalid><lyellow>" + task_name[task] + "</lyellow> -> 该项目已被占用";

            return "<invalid><lgreen>" + task_name[task] + "</lgreen><grey> -> 已被 Sirius 完成</grey>";
        }

        if (task == static_cast<Tasks>(cden) or task == static_cast<Tasks>(cden + 1))
            return "<invalid><lgreen>" + task_name[task] + "</lgreen><grey> -> 已被 cden 完成</grey>";

        if (task == static_cast<Tasks>(Kyota))
            return "<invalid><lgreen>" + task_name[task] + "</lgreen><grey> -> 已被 Kyota 完成</grey>";

        if (task == DocumentFault)
            return "<lyellow>" + task_name[task] + "</lyellow>";

        if (task == HeartKiller)
            return "<lred>" + task_name[task] + "</lred>";

        return "<lwhite>" + task_name[task] + "</lwhite>";
    };

    for (const auto& key: task_name | std::views::keys)
        lis.emplace_back(task_style(key));


    lis.emplace_back("<invalid><grey>N-3020 [已失效]</grey>");
    lis.emplace_back("<invalid><grey>N-0556 [已失效]</grey>");
    lis.emplace_back("<invalid><grey>N-7900 [已失效]</grey>");
    lis.emplace_back("<none>");
    lis.emplace_back("<lwhite>返回</lwhite>");

    ull resu = 0;

    CChoice(lis, resu, ChoiceStyle::Contract, "ENTER");
    switch (resu)
    {
        case 0: SURFACE = GameCheatTaskSurface;
            break;
        case 1: SURFACE = DecodeZipTaskSurface;
            break;
        case 2: SURFACE = FindQQClueTaskSurface;
            break;
        case 3: SURFACE = DocumentFaultTaskSurface;
            break;
        case 4: SURFACE = HeartKillerTaskSurfaceMix;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        default:
        case 11:
            SURFACE = MixMenu;
            break;
    }
}

void MailboxMix(const SaveData& saves)
{
    ClearConsole();
    std::string resu;
    CInputTo("Enter Mailbox Password:", 1, 0, resu);
    PrintLn t;
    t.content = "\n<red>密码错误!</red>\n";
    t.CPrint();
    _getch();
    SURFACE = MixMenu;
}

void GameCheatTaskDetails()
{
    ClearConsole();
    PrintLn t;
    if (!save.taskProgress_.isTaskDone(GameCheat))
    {
        t.content =
                "<lyellow>[委托名称]</lyellow>\n"
                "这游戏打半天打不过去\n"
                "\n"
                "<lyellow>[委托人邮箱]</lyellow>\n"
                + MailboxAndTask[0].mail +
                "\n\n"
                "<lyellow>[委托详情]</lyellow>\n"
                "我去了，这放置游戏怎么这么难打\n"
                "我打 1 个月了，游戏进度才 14.5%\n"
                "游戏进度曲线简直十四了玛\n"
                "游戏作者说通关了奖励 19198 块钱\n"
                "帮我打通关，事成了分你 10 %\n"
                "通过过后会显示一串兑换码，直接发到我邮箱里就好了\n"
                "\n"
                "<lyellow>[委托附件]</lyellow>\n"
                "<lwhite>下载链接: ada.nz/r/latex</lwhite>\n"
                "<grey>密码: latex</grey>\n";


        // char path[256];
        // GetDesktopPath(path, 256);
        // std::string npth = path;
        //
        // std::ifstream src("Data\\Lat3xIdle.exe", std::ios::binary);
        // std::ofstream dst(npth + "Lat3xIdle.exe", std::ios::binary);
        //
        // dst << src.rdbuf();
    } else
    {
        t.content =
                "<lyellow>[委托人反馈]</lyellow>\n"
                "卧槽了，我去兑奖了，他妈怎么是越南盾\n"
                "合计人民币tmd才不到5块钱\n"
                "话说兄弟你怎么这都能通关\n"
                "速度还这么快，神了\n"
                "\n";
    }
    t.CPrint();
    ull resu;
    CChoice({"退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    SURFACE = SiriusContract;
}


void GameDecodeZipTaskDetails()
{
    ClearConsole();
    PrintLn t;
    if (!save.taskProgress_.isTaskDone(DecodeZip))
    {
        t.content =
                "<lyellow>[委托名称]</lyellow>\n"
                "压缩包密码破解\n"
                "\n"
                "<lyellow>[委托人邮箱]</lyellow>\n"
                + MailboxAndTask[1].mail +
                "\n\n"
                "<lyellow>[委托详情]</lyellow>\n"
                "请帮我破解这个压缩包的密码。\n"
                "顺带一提，压缩包来源处有这么一串信息\n"
                "0x53, 0x32, 0x56, 0x77, 0x62, 0x47, 0x56, 0x79\n"
                "\n"
                "<lyellow>[委托附件]</lyellow>\n"
                "<lwhite>下载链接: keplerz.lanzouu.com/i4DzW4ae4hpa</lwhite>\n"
                "<grey>密码:0000</grey>\n";
    } else
    {
        t.content =
                "<lyellow>[委托人反馈]</lyellow>\n"
                "谢谢。\n"
                "\n";
    }
    t.CPrint();
    ull resu;
    CChoice({"退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    SURFACE = SiriusContract;
}


void FindQQClueTaskDetails()
{
    ClearConsole();
    PrintLn t;
    if (!save.taskProgress_.isTaskDone(FindQQClue))
    {
        t.content =
                "<lyellow>[委托名称]</lyellow>\n"
                "我需要这个人的一些信息\n"
                "\n"
                "<lyellow>[委托人邮箱]</lyellow>\n"
                + MailboxAndTask[2].mail +
                "\n\n"
                "<lyellow>[委托详情]</lyellow>\n"
                "目前已知线索：该个体长期活跃在QQ平台\n"
                "请告诉我 TA 的 Bilibili 平台账号的 UID\n"
                "附两条线索文件，请在附件中给出的页面里下载。\n"
                "\n"
                "<lyellow>[委托附件]</lyellow>\n"
                "<lwhite>docu.txt : keplerz.lanzouu.com/ixWYc4akaaxi</lwhite>\n"
                "<lwhite>voice.mp3 : keplerz.lanzouu.com/iZQMr4akaavg</lwhite>\n"
                "<grey>密码:0000</grey>\n";
    } else
    {
        t.content =
                "<lyellow>[委托人反馈]</lyellow>\n"
                "收到，经过核验，是正确的，感谢您的帮助。\n"
                "\n";
    }
    t.CPrint();
    ull resu;
    CChoice({"退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    SURFACE = SiriusContract;
}

void GameDocumentFaultTaskDetails()
{
    ClearConsole();
    PrintLn t;
    if (!save.taskProgress_.isTaskDone(DocumentFault))
    {
        t.content =
                "<lyellow>[委托名称]</lyellow>\n"
                "求求了帮我看看\n"
                "\n"
                "<lyellow>[委托人邮箱]</lyellow>\n"
                + MailboxAndTask[3].mail +
                "\n\n"
                "<lyellow>[委托详情]</lyellow>\n"
                "我导说我看报告不认真，于是在这文档里藏了个英文单词\n"
                "要我找出来，妈蛋我拿放大镜找半天都没发现哪里藏了\n"
                "再不尽快找到我导要找我麻烦了，求求了，帮个忙吧\n"
                "<grey>提交答案时，该单词用英文字母输入，全大写</grey>\n"
                "\n"
                "<lyellow>[委托附件]</lyellow>\n"
                "<lwhite>keplerz.lanzouu.com/irdoI4aiqn9g\n"
                "<grey>密码:0000</grey>\n";
    } else
    {
        t.content =
                "<lyellow>[委托人反馈]</lyellow>\n"
                "天哪谢谢你！呜呜呜\n"
                "\n";
    }
    t.CPrint();
    ull resu;
    CChoice({"退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    SURFACE = SiriusContract;
}

void GameHeartKillerDetails(SaveData& saves)
{
    ClearConsole();
    PrintLn t;
    t.content =
            "<lyellow>[委托名称]</lyellow>\n"
            "<grey>[数据删除]\n</grey>"
            "\n"
            "<lyellow>[委托人邮箱]</lyellow>\n"
            + MailboxAndTask[4].mail +
            "\n\n"
            "<lyellow>[委托详情]</lyellow>\n"
            "<grey>[数据删除]</grey>\n"
            "\n"
            "<lyellow>[委托附件]</lyellow>\n"
            "<grey>[数据删除]\n</grey>\n";

    t.CPrint();
    ull resu;
    CChoice({"退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    SURFACE = SiriusContract;
}

void GameHeartKillerDetailsMix(SaveData& saves)
{
    ClearConsole();
    PrintLn t;
    t.content =
            "<lyellow>[委托名称]</lyellow>\n"
            "<lred>6LW35pCP5Zmo5q2i5q2l6K6h5YiS\n</lred>"
            "\n"
            "<lyellow>[委托人邮箱]</lyellow>\n"
            + MailboxAndTask[4].mail +
            "\n\n"
            "<lyellow>[委托详情]</lyellow>\n"
            "\n"
            "cvq' qf:vk cmuqn cgdg g ugvggafhkwv qgg,"
            "fgv anovftuweucferdnvg,k c khen q gv tcg"
            "sgvqutnvfv gyt vcmcffhpg jtht,kffpvkewgv"
            "k gg.dvawjx geg jurkv je uvwakrgux!wjtuy"
            "tu juvu jwfjx gpuvc zton khen.jyxt w qo k"
            "kg gqtg p cckkku vyufhkwvhto qegv wukp g"
            "cg qpvqmcve p ggu.vggqg  k q pnf junx"
            "n w q cgtcjfvk qp,ykjk tn otukg\n"
            "\n"
            "<lyellow>[委托附件]</lyellow>\n"
            "<lwhite>U2FsdGVkX1/U+SPY0ww7FgmbV31A9vjc80jX+Slf2z4=</lwhite>\n";

    t.CPrint();
    ull resu;
    CChoice({"删除", "退出"}, resu, ChoiceStyle::HBracket, "ENTER");
    if (resu == 0)
    {
        save.mixProgress_.HeartDenied = 1;
        save.Save();
        t.content = "<red>删除成功！</red>";
        t.CPrint();
        _getch();
    }
    SURFACE = MixContract;
}

void GameMidEnd(SaveData& saves)
{
    ClearConsole();

    auto s = [&]
    {
        if (saves.mixProgress_.HeartDenied != 1)
            return "其实还有个支线你有没有通关";
        return "很惊诧，那个支线你竟然也打通过了";
    };
    RollCPrint({
                   "你好啊，很高兴你能玩到这里",
                   "这个游戏主线部分差不多就做到这里了",
                   s(),
                   "很感谢你的游玩，感谢你的认真参与",
                   "这个项目基本上是我一个人完成的，期间也有少量 Deepseek 的帮助",
                   "同时我也感激 Monny 和 Haki 在我创作过程中提供的精神支持",
                   "这部作品最初的灵感来源于我的一位学网安的朋友",
                   "他让我首次接触到CTF，其中的推理和挑战很吸引我",
                   "因此我萌生了创作这样的作品的想法，作为对CTF冰山一角的拙劣模仿",
                   "这个致谢画面只显示一次，最后，感谢你的耐心游玩！"
               }, 8, 7, 800, 0);
    saves.midEndProgress_.isMidEnd = 1;
    SURFACE                        = SiriusContract;
}

void GameMenu()
{
    ClearConsole();

    PrintLn s;
    s.content = "     --> <lyellow>Cipher3</lyellow> <--\n\n";
    s.CPrint();

    ull resu = 0;
    CChoice({"开始", "帮助", "关于", "退出"}, resu, ChoiceStyle::Menu, "ENTER");
    switch (resu)
    {
        case 0: SURFACE = Start;
            break;
        case 1: SURFACE = Help;
            break;
        case 2: SURFACE = About;
            break;
        case 3:
        default: SURFACE = EXIT;
            break;
    }
}

void GameMenuStart(SaveData& saves)
{
    InitializeStory();
    prelude_intro();

    ull resu = 0;
    CChoice({"记住了", "666发的什么东西"}, resu, ChoiceStyle::Menu, "ENTER");
    switch (resu)
    {
        case 0:
            prelude_intro_agree(saves);
            SURFACE = SignIn;
            break;
        case 1:
            prelude_intro_refuse();
            break;
        default: break;
    }
}


void GameMenuHelp()
{
    ClearConsole();

    constexpr int D1 = 5, D2 = 10, D3 = 5, W = 300;

    auto Play = [&](const std::string& forward, const std::string& final)
    {
        PrintLn t;
        t.content = forward;

        CPrintThenBack({
            .forward      = t,
            .finalText    = final,
            .delayForward = D1,
            .delayBack    = D2,
            .delayFinal   = D3,
            .waitForward  = W,
            .waitBack     = W,
        });
    };

    Play("我不知道要写什么",
         "这是一款较为硬核的解密游戏，\n");
    Play("这么硬核的游戏真的有人玩得下去吗",
         "参考了部分CTF的题型，会涉及一些基础密码学方面内容\n");
    Play("如果有人玩的话我就发个攻略",
         "你可以询问 AI 来帮忙解决\n");
    Play("发个B站专栏给你们详细讲讲",
         "但独立思考肯定比坐享其成更有趣味！\n");

    Sleep(1000);
    for (auto Y = GetConsoleCursorXY().Y; Y > 0; Y--)
    {
        SetConsoleCursorXY(0, Y);
        std::cout << std::string(60, ' ') << std::flush;
        Sleep(200);
    }
}

void GameMenuAbout()
{
    ClearConsole();

    RollCPrint({
                   "游戏名: <lyellow>KepCipher3</lyellow>",
                   "游戏制作: <lyellow>Z. Kepler</lyellow>",
                   "AI参与度: <lgreen>极低</lgreen>",
                   "特别鸣谢: Moony, Haki",
                   "<black>彩蛋: 把 'RkxBRw==' 这条消息通过 QQ 发送给 Z. Kepler，他可能会给你什么东西</black>",
               }, 8, 7, 200, 0);

    RollCPrint({
                   "Game Name: <lyellow>KepCipher3</lyellow>",
                   "Game Maker: <lyellow>Z. Kepler</lyellow>",
                   "AI Participation Degree: <lgreen>Very Low</lgreen>",
                   "Special Thanks: Moony, Haki",
                   "<black>Clue: Send the message 'RkxBRw==' via QQ to Z. Kepler. He might give you something.</black>",
               }, 8, 7, 200, 0);
}

void GameRestart()
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(nullptr, szPath, MAX_PATH);

    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi{};

    if (!CreateProcess(szPath, nullptr, nullptr, nullptr, FALSE, 0,
                       nullptr, nullptr, &si, &pi))
    {
        const PrintLn s;
        s.CPrint("<lred>重启失败，请手动重新打开程序</lred>\n");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    exit(0);
}
