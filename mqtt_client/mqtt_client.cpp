#include "mqtt_client.h"

#include <iostream>
#include <thread>

const MQTT_NS::qos MqttClient::QoS = MQTT_NS::qos::at_least_once;

MqttClient::MqttClient(std::string_view client_id,
                         std::string_view host,
                         std::string_view port,
                         const str_view_vec& topics):
    out_{std::cout}, message_handler_{nullptr}
{
    client_ = MQTT_NS::make_sync_client(ioc_, host.data(), port.data());
    client_->set_client_id(client_id.data());
    client_->set_clean_session(true);

    auto on_connect = [this, &topics]
    (bool, MQTT_NS::connect_return_code connack_return_code) -> bool
    {
        this->connackHandler(topics, connack_return_code);
        return true;
    };
    client_->set_connack_handler(on_connect);

    client_->set_close_handler([this](){
        this->out_ << "connection closed" << std::endl;
    });

    auto on_publish = [this](
            MQTT_NS::optional<packet_id_t>,
            MQTT_NS::publish_options,
            MQTT_NS::buffer topic,
            MQTT_NS::buffer payload) -> bool
    {
        const std::string str_topic{topic.data(), topic.length()};
        const std::string str_payload{payload.data(), payload.length()};
        this->publishHandler(str_topic, str_payload);
        return true;
    };
    client_->set_publish_handler(on_publish);

    client_->connect();
    std::thread([this](){ioc_.run();}).detach();
}

void MqttClient::SetMessageHandler(IMessageHandler* handler)
{
    message_handler_ = handler;
}

void MqttClient::Stop()
{
    out_ << "stopping..." << std::endl;
    if (client_ && client_->connected())
    {
        client_->disconnect();
    }
    ioc_.stop();
    out_ << "stopped" << std::endl;
}

MqttClient::~MqttClient()
{
    Stop();
}

void MqttClient::connackHandler(const str_view_vec& topics,
                                  MQTT_NS::connect_return_code rc)
{
    std::string str_rc{MQTT_NS::connect_return_code_to_str(rc)};
    out_ << "Connack handler called" << std::endl;
    out_ << "Connack Return Code: " << str_rc << std::endl;
    if (rc != MQTT_NS::connect_return_code::accepted) {
        throw std::runtime_error(
            std::string{"broker connection failure: "} + str_rc
        );
    }
    for (const auto& t: topics) {
        client_->subscribe(t.data(), MqttClient::QoS);
        out_ << "subscribed to" << t.data() << std::endl;
    }
}

void MqttClient::publishHandler(std::string_view topic,
                                  std::string_view payload) noexcept
{
    out_ << "topic: " << topic.data()
         << "; payload: " << payload.data() << std::endl;
    if (message_handler_)
    {
        message_handler_->onMessage(topic, payload);
    }
}

