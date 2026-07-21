#if defined _chaosforge_protocol_included
    #endinput
#endif
#define _chaosforge_protocol_included

#define CHAOSFORGE_PROTOCOL_MESSAGE_SIZE 512
#define CHAOSFORGE_EVENT_TYPE_SIZE 64
#define CHAOSFORGE_INFECTED_SIZE 32
#define CHAOSFORGE_VIEWER_NAME_SIZE 128

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

    // Keep the existing connection-control protocol.
    if (StrEqual(message, "PING", false))
    {
        socket.WriteString("PONG");
        return;
    }

    // Keep the old command temporarily for compatibility.
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

    ChaosProtocol_HandleJsonEvent(socket, message);
}

void ChaosProtocol_HandleJsonEvent(
    WebSocket socket,
    const char[] message
)
{
    YYJSON json = YYJSON.Parse(message);

    if (json == null)
    {
        LogError(
            "[ChaosForge] Failed to parse Bridge message as JSON."
        );

        socket.WriteString("ERROR INVALID_JSON");
        return;
    }

    char eventType[CHAOSFORGE_EVENT_TYPE_SIZE];
    char infected[CHAOSFORGE_INFECTED_SIZE];
    char viewerName[CHAOSFORGE_VIEWER_NAME_SIZE];

    bool hasEventType = json.PtrGetString(
        "/type",
        eventType,
        sizeof(eventType)
    );

    bool hasInfected = json.PtrGetString(
        "/infected",
        infected,
        sizeof(infected)
    );

    bool hasViewerName = json.PtrGetString(
        "/viewerName",
        viewerName,
        sizeof(viewerName)
    );

    int count = json.PtrGetInt("/count");

    if (!hasEventType)
    {
        LogError(
            "[ChaosForge] JSON event is missing the required 'type' property."
        );

        socket.WriteString("ERROR MISSING_EVENT_TYPE");
        delete json;
        return;
    }

    PrintToServer(
        "[ChaosForge] Parsed event:"
    );

    PrintToServer(
        "[ChaosForge]   Type: %s",
        eventType
    );

    PrintToServer(
        "[ChaosForge]   Infected: %s",
        hasInfected ? infected : "(not provided)"
    );

    PrintToServer(
        "[ChaosForge]   Count: %d",
        count
    );

    PrintToServer(
        "[ChaosForge]   Viewer: %s",
        hasViewerName ? viewerName : "(not provided)"
    );

    ChaosDispatcher_Dispatch(
    socket,
    eventType,
    infected,
    count,
    hasInfected
);

delete json;
}