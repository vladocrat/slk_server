#pragma once

#include <QByteArray>
#include <QUuid>
#include <QDataStream>
#include <QDebug>

#include "protocol.h"

namespace slk
{
    
class MessageParser
{
public:
    using CommandResult = std::pair<Messages::MessageType, std::shared_ptr<QDataStream>>;
    
    static CommandResult parseCommand(const QByteArray& data) noexcept;

    template<class... Args>
    static void parseData(const std::shared_ptr<QDataStream>& stream, const std::tuple<Args&...>& args)
    {
        parseValue(stream, args, std::make_index_sequence<sizeof... (Args)>{});
    }

private:
    template<class... Args,  std::size_t... Is>
    static void parseValue(const std::shared_ptr<QDataStream>& stream, const std::tuple<Args&...>& args, const std::index_sequence<Is...>)
    {
        (writeValue(stream, std::get<Is>(args)), ...);
    }

    template<class T>
    static void writeValue(const std::shared_ptr<QDataStream>& stream, T& value)
    {
        using ArgType = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<ArgType, QUuid>) {
            QByteArray uuidBytes;
            *stream >> uuidBytes;
            value = QUuid::fromRfc4122(uuidBytes);
            return;
        }

        *stream >> value;
    }
};

} //! slk
