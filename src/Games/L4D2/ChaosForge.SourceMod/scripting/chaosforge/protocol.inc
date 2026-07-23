#if defined _chaosforge_protocol_included
    #endinput
#endif
#define _chaosforge_protocol_included

void ChaosProtocol_HandleMessage(
    WebSocket socket,
    const char[] message
)
{
    if (StrEqual(message, "PING", false))
    {
        PrintToServer(
            "[ChaosForge] Received PING from Bridge."
        );

        socket.WriteString("PONG");
        return;
    }

    if (message[0] == '{')
    {
        ChaosProtocol_HandleJsonEvent(
            socket,
            message
        );

        return;
    }

    PrintToServer(
        "[ChaosForge] Unknown Bridge message: %s",
        message
    );
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

    char eventId[64];
    char eventType[64];
    char messageType[32];
    char infected[32];
    char viewerName[128];

    eventId[0] = '\0';
    eventType[0] = '\0';
    messageType[0] = '\0';
    infected[0] = '\0';
    viewerName[0] = '\0';

    int protocolVersion =
        json.PtrGetInt("/protocolVersion");

    bool hasMessageType = json.PtrGetString(
        "/messageType",
        messageType,
        sizeof(messageType)
    );

    bool hasEventId = json.PtrGetString(
        "/eventId",
        eventId,
        sizeof(eventId)
    );

    bool hasEventType = json.PtrGetString(
        "/eventType",
        eventType,
        sizeof(eventType)
    );

    if (protocolVersion != 1)
    {
        LogError(
            "[ChaosForge] Unsupported protocol version: %d",
            protocolVersion
        );

        delete json;
        return;
    }

    if (!hasMessageType
        || !StrEqual(
            messageType,
            "ChaosEvent",
            false
        ))
    {
        LogError(
            "[ChaosForge] Unsupported message type."
        );

        delete json;
        return;
    }

    if (!hasEventId)
    {
        LogError(
            "[ChaosForge] JSON event is missing 'eventId'."
        );

        delete json;
        return;
    }

    if (!hasEventType)
    {
        ChaosDispatcher_SendAcknowledgement(
            socket,
            eventId,
            false,
            1,
            0,
            "Missing event type."
        );

        delete json;
        return;
    }

    bool hasViewerName = json.PtrGetString(
        "/context/viewerName",
        viewerName,
        sizeof(viewerName)
    );

    int count =
        json.PtrGetInt("/payload/count");

    if (count < 1)
    {
        count = 1;
    }

    bool hasInfected = false;

    if (StrEqual(
        eventType,
        "SpawnSpecialInfected",
        false
    ))
    {
        hasInfected = json.PtrGetString(
            "/payload/infected",
            infected,
            sizeof(infected)
        );
    }

    PrintToServer(
        "[ChaosForge] Parsed protocol v%d event:",
        protocolVersion
    );

    PrintToServer(
        "[ChaosForge]   ID: %s",
        eventId
    );

    PrintToServer(
        "[ChaosForge]   Type: %s",
        eventType
    );

    if (hasInfected)
    {
        PrintToServer(
            "[ChaosForge]   Infected: %s",
            infected
        );
    }
    else
    {
        PrintToServer(
            "[ChaosForge]   Infected: (not required)"
        );
    }

    PrintToServer(
        "[ChaosForge]   Count: %d",
        count
    );

    if (hasViewerName)
    {
        PrintToServer(
            "[ChaosForge]   Viewer: %s",
            viewerName
        );
    }
    else
    {
        PrintToServer(
            "[ChaosForge]   Viewer: (not provided)"
        );
    }

    ChaosDispatcher_Dispatch(
        socket,
        eventId,
        eventType,
        infected,
        count,
        hasInfected,
        viewerName,
        hasViewerName
    );

    delete json;
}