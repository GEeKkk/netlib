#include "sudoku.h"

// #include <muduo/base/Atomic.h>
// #include <muduo/base/Logging.h>
// #include <muduo/base/Thread.h>
// #include <muduo/net/EventLoop.h>
// #include <muduo/net/InetAddress.h>
// #include <muduo/net/TcpServer.h>

#include <foxtail/reactor/TcpServer.h>
#include <foxtail/base/CurrentThread.h>
#include <foxtail/reactor/EventLoop.h>
#include <foxtail/base/Timestamp.h>

#include <utility>

#include <stdio.h>
#include <unistd.h>

#include <functional>

using namespace std;
using namespace muduo;
using namespace std::placeholders;


class SudokuServer
{
public:
    SudokuServer(EventLoop *loop, const InetAddress &listenAddr)
        : server_(loop, listenAddr),
          startTime_(Timestamp::now())
    {
        server_.SetConnCallback(bind(&SudokuServer::onConnection, this, _1));
        server_.SetMsgCallback(bind(&SudokuServer::onMessage, this, _1, _2, _3));
    }

    void start()
    {
        server_.Start();
    }

private:
    void onConnection(const TcpConnPtr &conn)
    {
        LOG_TRACE << conn->peerAddress().toHostPort() << " -> "
                  << conn->localAddress().toHostPort() << " is "
                  << (conn->Connected() ? "UP" : "DOWN");
    }

    void onMessage(const TcpConnPtr &conn, Buffer *buf, Timestamp)
    {
        LOG_DEBUG << conn->name();
        size_t len = buf->readableBytes();
        LOG_DEBUG << "Receive " << len << " bytes";
        while (len >= kCells + 2)
        {
            const char *crlf = buf->findCRLF();
            if (crlf)
            {
                string request(buf->peek(), crlf);
                buf->retrieveUntil(crlf + 2);
                len = buf->readableBytes();
                if (!processRequest(conn, request))
                {
                    conn->Send("Bad Request!\r\n");
                    conn->Shutdown();
                    break;
                }
            }
            else if (len > 100) // id + ":" + kCells + "\r\n"
            {
                conn->Send("Id too long!\r\n");
                conn->Shutdown();
                break;
            }
            else
            {
                break;
            }
        }
    }

    bool processRequest(const TcpConnPtr &conn, const string &request)
    {
        string id;
        string puzzle;
        bool goodRequest = true;

        string::const_iterator colon = find(request.begin(), request.end(), ':');
        if (colon != request.end())
        {
            id.assign(request.begin(), colon);
            puzzle.assign(colon + 1, request.end());
        }
        else
        {
            puzzle = request;
        }

        if (puzzle.size() == implicit_cast<size_t>(kCells))
        {
            LOG_DEBUG << conn->name();
            string result = solveSudoku(puzzle);
            if (id.empty())
            {
                conn->Send(result + "\r\n");
            }
            else
            {
                conn->Send(id + ":" + result + "\r\n");
            }
        }
        else
        {
            goodRequest = false;
        }
        return goodRequest;
    }

    TcpServer server_;
    Timestamp startTime_;
};

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    EventLoop loop;
    InetAddress listenAddr(9981);
    SudokuServer server(&loop, listenAddr);

    server.start();

    loop.Loop();
}
