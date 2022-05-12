#include "test.h"
#include <iostream>
#include <thread>

class TestMessageHandler: public IMessageHandler
{
public:
    explicit TestMessageHandler(MqttClient* client):
        client_{client} {}
    void onMessage(std::string_view topic, std::string_view payload) override
    {
        if (payload == "stop" && client_)
        {
            client_->Stop();
        }
        std::cout << "TestMessageHandler: " << topic << " " << payload << std::endl;
    }
private:
    MqttClient* client_;
};

std::string_view client_id {"imgvideo1"};
MqttClient::str_view_vec topics {
    "/er/imgvideo1/cmd",
    "/er/cmd",
    //"#",
};
std::string_view host{"192.168.1.32"};
std::string_view port{"1883"};

void mqtt_client_test()
{
    MqttClient client{client_id, host, port, topics};
    TestMessageHandler handler{&client};
    client.SetMessageHandler(&handler);
    std::this_thread::sleep_for(std::chrono::hours(1));
}
