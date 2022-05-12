#pragma once

#include "imessage_handler.h"

#include <mqtt_client_cpp.hpp>

#include <mutex>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class MqttClient
{
public:
    using str_view_vec = std::vector<std::string_view>;
    MqttClient(std::string_view client_id,
                std::string_view host,
                std::string_view port,
                const str_view_vec& topics);
    void Stop();
    void SetMessageHandler(IMessageHandler* handler);
    ~MqttClient();
private:
    static const MQTT_NS::qos QoS;
    using sync_client_overlay = MQTT_NS::callable_overlay<
        MQTT_NS::sync_client<
            MQTT_NS::tcp_endpoint<
                boost::asio::basic_stream_socket<boost::asio::ip::tcp>,
                boost::asio::io_context::strand
            >
        >
    >;
    using sync_client_share_ptr_t = std::shared_ptr<sync_client_overlay>;
    using packet_id_t = typename std::remove_reference_t<sync_client_overlay>::packet_id_t;

    void connackHandler(const str_view_vec& topics,
                         MQTT_NS::connect_return_code rc);
    void publishHandler(std::string_view topic,
                         std::string_view payload) noexcept;

    sync_client_share_ptr_t client_;
    boost::asio::io_context ioc_;
    std::ostream& out_;
    IMessageHandler *message_handler_;
};
