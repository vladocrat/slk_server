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
        std::apply([&stream](Args&... unpackedArgs) {
            ([&stream](auto& arg) {
                using ArgType = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<ArgType, QUuid>) {
                    QByteArray uuidBytes;
                    *stream >> uuidBytes;
                    arg = QUuid::fromRfc4122(uuidBytes);
                    return;
                }

                *stream >> arg;
            } (unpackedArgs), ...);
        }, args);
    }

private:
    MessageParser();
};

} //! slk
