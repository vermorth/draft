#include <iostream>

#include "client.hh"

int main(int argc, char *argv[]) {
    if (argc != 5) {
        std::cout << "Usage: coro_client <method> <host> <port> <path>\n";
        std::cout << "Example:\n";
        std::cout << "  coro_client GET think-async.com 80 /Asio/\n";
        return 1;
    }

    try {
        asio::io_context io_ctx;
        auto client_ctx =
            std::make_shared<draft::http::coro_client::Context>(io_ctx);

        std::string path;
        bool bret;
        std::tie(path, bret) = draft::http::Request::UrlEncode(argv[4]);
        if (!bret) {
            std::cout << "error occurred while encode path"
                      << "\n";
            return 1;
        }
        draft::http::Request request = {
            argv[1],  // _method
            argv[2],  // _host
            argv[3],  // _port
            "",       // _url
            path,     // _path
            "",       // _query
            {},       // _query_params
            {},       // _headers
            ""        // _body
        };
        request._headers.push_back(draft::http::Header{"Host", argv[2]});

        client_ctx->SetRequest(request);
        draft::http::coro_client::Do(client_ctx);

        io_ctx.run();
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}