#include "gtest/gtest.h"
#include <xboard_interface.h>
#include <sstream>

class XBoardInterfaceTests : public ::testing::Test
{
protected:
    std::stringstream in;
    std::stringstream out;
    std::unique_ptr<XBoardInterface> iface;

    void SetUp() override
    {
        iface = std::make_unique<XBoardInterface>(in, out, "TestEngine");
    }

    XBoardInterface::CommandReceived send(const std::string& line)
    {
        in << line << "\n";
        return iface->wait_for_command();
    }
};

// --- Command parsing ---

TEST_F(XBoardInterfaceTests, ParsesXboard)
{
    auto cmd = send("xboard");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::INIT);
    EXPECT_FALSE(cmd.is_invalid());
}

TEST_F(XBoardInterfaceTests, ParsesProtover)
{
    auto cmd = send("protover 2");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::INFO_REQ);
    EXPECT_EQ(cmd.get_params().front(), "2");
}

TEST_F(XBoardInterfaceTests, ParsesPing)
{
    auto cmd = send("ping 7");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::PING);
    EXPECT_EQ(cmd.get_params().front(), "7");
}

TEST_F(XBoardInterfaceTests, ParsesNew)      { EXPECT_EQ(send("new").get_type(),    XBoardInterface::CommandReceived::NEW); }
TEST_F(XBoardInterfaceTests, ParsesQuit)     { EXPECT_EQ(send("quit").get_type(),   XBoardInterface::CommandReceived::QUIT); }
TEST_F(XBoardInterfaceTests, ParsesForce)    { EXPECT_EQ(send("force").get_type(),  XBoardInterface::CommandReceived::FORCE); }
TEST_F(XBoardInterfaceTests, ParsesGo)       { EXPECT_EQ(send("go").get_type(),     XBoardInterface::CommandReceived::GO); }
TEST_F(XBoardInterfaceTests, ParsesPost)     { EXPECT_EQ(send("post").get_type(),   XBoardInterface::CommandReceived::POST); }
TEST_F(XBoardInterfaceTests, ParsesHard)     { EXPECT_EQ(send("hard").get_type(),   XBoardInterface::CommandReceived::HARD); }
TEST_F(XBoardInterfaceTests, ParsesRandom)   { EXPECT_EQ(send("random").get_type(), XBoardInterface::CommandReceived::RANDOM); }
TEST_F(XBoardInterfaceTests, ParsesUndo)     { EXPECT_EQ(send("undo").get_type(),   XBoardInterface::CommandReceived::UNDO); }
TEST_F(XBoardInterfaceTests, ParsesRemove)   { EXPECT_EQ(send("remove").get_type(), XBoardInterface::CommandReceived::REMOVE); }
TEST_F(XBoardInterfaceTests, ParsesEdit)     { EXPECT_EQ(send("edit").get_type(),   XBoardInterface::CommandReceived::EDIT); }

TEST_F(XBoardInterfaceTests, ParsesTime)
{
    auto cmd = send("time 300");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::TIME);
    EXPECT_EQ(cmd.get_intparams().front(), 300);
}

TEST_F(XBoardInterfaceTests, ParsesOtim)
{
    auto cmd = send("otim 120");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::OTIM);
    EXPECT_EQ(cmd.get_intparams().front(), 120);
}

TEST_F(XBoardInterfaceTests, ParsesMemory)
{
    auto cmd = send("memory 64");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::MEMORY);
    EXPECT_EQ(cmd.get_intparams().front(), 64);
}

TEST_F(XBoardInterfaceTests, ParsesLevel)
{
    auto cmd = send("level 40 5 0");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::LEVEL);
    EXPECT_EQ(cmd.get_params().size(), 3u);
}

TEST_F(XBoardInterfaceTests, ParsesAcceptedAsNone)
{
    EXPECT_EQ(send("accepted ping").get_type(), XBoardInterface::CommandReceived::NONE);
}

TEST_F(XBoardInterfaceTests, ParsesRejectedAsNone)
{
    EXPECT_EQ(send("rejected ping").get_type(), XBoardInterface::CommandReceived::NONE);
}

TEST_F(XBoardInterfaceTests, ParsesPlayotherAsGo)
{
    auto cmd = send("playother");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::GO);
    EXPECT_EQ(cmd.get_params().front(), "playother");
}

TEST_F(XBoardInterfaceTests, ParsesResultAsGo)
{
    EXPECT_EQ(send("result 1-0 {White wins}").get_type(), XBoardInterface::CommandReceived::GO);
}

TEST_F(XBoardInterfaceTests, UnknownCommandIsInvalid)
{
    EXPECT_TRUE(send("foobar").is_invalid());
}

// --- Move parsing ---

TEST_F(XBoardInterfaceTests, ParsesSimpleMove)
{
    auto cmd = send("e2e4");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::MOVE);
    EXPECT_EQ(cmd.get_move_string(), "e2e4");
}

TEST_F(XBoardInterfaceTests, ParsesPromotionMove)
{
    auto cmd = send("e7e8q");
    EXPECT_EQ(cmd.get_type(), XBoardInterface::CommandReceived::MOVE);
    EXPECT_EQ(cmd.get_move_string(), "e7e8q");
}

TEST_F(XBoardInterfaceTests, RejectsInvalidPromotionChar)
{
    EXPECT_TRUE(send("e7e8x").is_invalid());
}

TEST_F(XBoardInterfaceTests, RejectsTooShortMove)
{
    EXPECT_TRUE(send("e2e").is_invalid());
}

TEST_F(XBoardInterfaceTests, RejectsMoveWithBadColumn)
{
    EXPECT_TRUE(send("e2z4").is_invalid());
}

// --- Output functions ---

TEST_F(XBoardInterfaceTests, SendThinkingFormat)
{
    iface->send_thinking(5, 42, 100, 99999, "e2e4 e7e5");
    EXPECT_EQ(out.str(), "5 42 100 99999 e2e4 e7e5\n");
}

TEST_F(XBoardInterfaceTests, SendMove)
{
    iface->send_move("e2e4");
    EXPECT_EQ(out.str(), "move e2e4\n");
}

TEST_F(XBoardInterfaceTests, ReplyPing)
{
    iface->reply_ping(send("ping 3"));
    EXPECT_EQ(out.str(), "pong 3\n");
}

TEST_F(XBoardInterfaceTests, ReplyInvalid)
{
    iface->reply_invalid(send("garbage"));
    EXPECT_NE(out.str().find("Error"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReplyIllegalMove)
{
    iface->reply_illegal_move(send("e2e4"));
    EXPECT_NE(out.str().find("Illegal move"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReplyResultWhiteWin)
{
    iface->reply_result(XBoardInterface::Result::WHITE_WIN);
    EXPECT_NE(out.str().find("1-0"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReplyResultBlackWin)
{
    iface->reply_result(XBoardInterface::Result::BLACK_WIN);
    EXPECT_NE(out.str().find("0-1"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReplyResultDraw)
{
    iface->reply_result(XBoardInterface::Result::DRAW);
    EXPECT_NE(out.str().find("1/2-1/2"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReplyFeaturesContainsName)
{
    iface->add_feature("ping=1");
    iface->add_variant("normal");
    iface->reply_features();
    auto s = out.str();
    EXPECT_NE(s.find("TestEngine"), std::string::npos);
    EXPECT_NE(s.find("ping=1"), std::string::npos);
    EXPECT_NE(s.find("normal"), std::string::npos);
    EXPECT_NE(s.find("done=1"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, TellInfo)
{
    iface->tell_info("hello");
    EXPECT_NE(out.str().find("hello"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReplyNewline)
{
    iface->reply_newline();
    EXPECT_FALSE(out.str().empty());
}

TEST_F(XBoardInterfaceTests, ReplyFeaturesIncludesOptions)
{
    iface->add_option("Threads spin 1 1 16");
    iface->reply_features();
    EXPECT_NE(out.str().find("Threads"), std::string::npos);
}

TEST_F(XBoardInterfaceTests, ReadEditMode)
{
    in << "Pa4\npc5\n.\n";
    auto lines = iface->read_edit_mode();
    EXPECT_EQ(lines[0], "Pa4");
    EXPECT_EQ(lines[1], "pc5");
    EXPECT_EQ(lines[2], ".");
}
