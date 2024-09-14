#include <drf_client.hpp>

#include "settings.hpp"
#include <globals.hpp>
#include <nlohmann/json.hpp>
static context_ptr on_tls_init()
{
    // establishes a SSL connection
    context_ptr ctx = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 | boost::asio::ssl::context::single_dh_use);
    } catch (std::exception &e) {
        std::string message("Error in context pointer: " + std::string(e.what()));
        api->Log(ELogLevel_CRITICAL, addon_name, message.c_str());
    }
    return ctx;
}

DrfClient::DrfClient()
{
    m_client.init_asio();
    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    m_client.set_open_handler(bind(&DrfClient::on_open, this));
    //    m_client.set_close_handler(bind(&DrfClient::on_close, this, _1));
    //    m_client.set_fail_handler(bind(&DrfClient::on_fail, this, _1));
    m_client.set_message_handler(bind(&DrfClient::on_message, this, _2));
    m_client.set_tls_init_handler(bind(&on_tls_init));
}

void DrfClient::run()
{
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_client.get_connection("wss://drf.rs/ws", ec);
    if (ec) {
        std::string message("Get Connection Error: " + std::string(ec.message()));
        api->Log(ELogLevel_CRITICAL, addon_name, message.c_str());
        return;
    }

    // Grab a handle for this connection so we can talk to it in a thread
    // safe manor after the event loop starts.
    m_handle = con->get_handle();

    // Queue the connection. No DNS queries or network connections will be
    // made until the io_service event loop is run.
    m_client.connect(con);

    // Create a thread to run the ASIO io_service event loop
    websocketpp::lib::thread asio_thread(&client::run, &m_client);
    asio_thread.detach();
    m_running = true;
}

void DrfClient::on_open() { m_client.send(m_handle, "Bearer " + Settings::drf_token, websocketpp::frame::opcode::text); }

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void DrfClient::on_message(const message_ptr& msg)
{
    std::string message = msg->get_payload();
    nlohmann::json json = nlohmann::json::parse(message);
    {
        std::lock_guard<std::mutex> lock(to_process_mutex);
        to_process.push(json);
    }
    websocketpp::lib::error_code ec;
}

void DrfClient::close()
{
    m_client.close(m_handle, websocketpp::close::status::normal, "closed");
    m_client.stop();
    m_running = false;
}