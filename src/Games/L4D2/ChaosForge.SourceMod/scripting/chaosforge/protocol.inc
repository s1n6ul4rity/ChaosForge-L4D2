#if defined _chaosforge_protocol_included
    #endinput
#endif
#define _chaosforge_protocol_included

#define CHAOSFORGE_PROTOCOL_MESSAGE_SIZE 512

void ChaosProtocol_HandleMessage(
    WebSocket socket,
    const char[] incomingMessage
)
{
    char message[CHAOSFORGE_PROTOCOL_MESSAGE_SIZE];

    strcopy(
        message,
        sizeof(message),
        incomingMessage
    );

    TrimString(message);

    PrintToServer(
        "[ChaosForge] Bridge message received: %s",
        message
    );

    if (StrEqual(message, "PING", false))
    {
        socket.WriteString("PONG");
        return;
    }

    if (StrEqual(message, "SPAWN_HUNTER", false))
    {
        if (ChaosSpawn_Hunter())
        {
            socket.WriteString("OK SPAWN_HUNTER");
        }
        else
        {
            socket.WriteString("ERROR SPAWN_HUNTER");
        }

        return;
    }

    socket.WriteString("ERROR UNKNOWN_COMMAND");
}
