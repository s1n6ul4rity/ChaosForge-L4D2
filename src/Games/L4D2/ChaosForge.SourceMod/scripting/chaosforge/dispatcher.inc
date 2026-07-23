#if defined _chaosforge_dispatcher_included
    #endinput
#endif
#define _chaosforge_dispatcher_included

#define CHAOSFORGE_ACK_MESSAGE_SIZE 512

forward bool ChaosSpawnQueue_Enqueue(
    WebSocket socket,
    const char[] eventId,
    const char[] infected,
    int requestedCount
);

bool ChaosDispatcher_Dispatch(
    WebSocket socket,
    const char[] eventId,
    const char[] eventType,
    const char[] infected,
    int count,
    bool hasInfected
)
{
    if (count < 1)
    {
        count = 1;
    }

    if (StrEqual(eventType, "SpawnSpecialInfected", false))
    {
        if (!hasInfected)
        {
            ChaosDispatcher_SendAcknowledgement(
                socket,
                eventId,
                false,
                count,
                0,
                "Missing infected type."
            );

            return false;
        }

        if (!ChaosSpawnQueue_Enqueue(
            socket,
            eventId,
            infected,
            count
        ))
        {
            ChaosDispatcher_SendAcknowledgement(
                socket,
                eventId,
                false,
                count,
                0,
                "Failed to queue special infected request."
            );

            return false;
        }

        return true;
    }

    if (StrEqual(eventType, "SpawnCommonInfected", false))
    {
        if (!ChaosSpawn_Common(count))
        {
            ChaosDispatcher_SendAcknowledgement(
                socket,
                eventId,
                false,
                count,
                0,
                "Failed to spawn common infected."
            );

            return false;
        }

        ChaosDispatcher_SendAcknowledgement(
            socket,
            eventId,
            true,
            count,
            count,
            "Common infected spawned successfully."
        );

        return true;
    }

    ChaosDispatcher_SendAcknowledgement(
        socket,
        eventId,
        false,
        count,
        0,
        "Unknown event type."
    );

    return false;
}

void ChaosDispatcher_SendAcknowledgement(
    WebSocket socket,
    const char[] eventId,
    bool success,
    int requestedCount,
    int executedCount,
    const char[] resultMessage
)
{
    char acknowledgement[CHAOSFORGE_ACK_MESSAGE_SIZE];
    char successValue[6];

    strcopy(
        successValue,
        sizeof(successValue),
        success ? "true" : "false"
    );

    Format(
        acknowledgement,
        sizeof(acknowledgement),
        "{\"protocolVersion\":1,\"messageType\":\"EventResult\",\"eventId\":\"%s\",\"success\":%s,\"requestedCount\":%d,\"executedCount\":%d,\"message\":\"%s\"}",
        eventId,
        successValue,
        requestedCount,
        executedCount,
        resultMessage
    );

    PrintToServer(
        "[ChaosForge] Sending acknowledgement: %s",
        acknowledgement
    );

    socket.WriteString(acknowledgement);
}

int ChaosDispatcher_SpawnInfectedEntity(
    const char[] infected
)
{
    return ChaosSpawn_SpecialInfectedEntity(infected);
}

bool ChaosDispatcher_SpawnInfected(
    const char[] infected
)
{
    return ChaosDispatcher_SpawnInfectedEntity(infected) > 0;
}
