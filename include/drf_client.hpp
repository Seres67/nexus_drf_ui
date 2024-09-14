#ifndef WEBSOCKET_HPP
#define WEBSOCKET_HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

typedef std::shared_ptr<boost::asio::ssl::context> context_ptr;
typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

class DrfClient
{
  public:
    DrfClient();

    void run();

    void on_open();

    void on_message(const message_ptr &msg);

    void on_close();

    void on_fail();

    void close();

    [[nodiscard]] bool is_running() const { return m_running; }

  private:
    client m_client;
    context_ptr m_context;
    websocketpp::connection_hdl m_handle;
    bool m_running = false;
};

#endif // WEBSOCKET_HPP