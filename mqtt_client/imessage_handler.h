#pragma once

#include <string_view>

class IMessageHandler
{
public:
    virtual void onMessage(std::string_view topic,
                           std::string_view payload) = 0;
};
