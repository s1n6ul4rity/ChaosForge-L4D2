#if defined _chaosforge_dispatcher_included
    #endinput
#endif
#define _chaosforge_dispatcher_included

#define CHAOSFORGE_ACK_MESSAGE_SIZE 512

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

    if (StrEqual(
        eventType,
        "SpawnSpecialInfected",
        false
    ))
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

        int executedCount = 0;

        for (int i = 0; i < count; i++)
        {
            if (!ChaosDispatcher_SpawnInfected(infected))
            {
                ChaosDispatcher_SendAcknowledgement(
                    socket,
                    eventId,
                    false,
                    count,
                    executedCount,
                    "Failed to spawn all requested special infected."
                );

                return false;
            }

            executedCount++;
        }

        ChaosDispatcher_SendAcknowledgement(
            socket,
            eventId,
            true,
            count,
            executedCount,
            "Special infected spawned successfully."
        );

        return true;
    }

    if (StrEqual(
        eventType,
        "SpawnCommonInfected",
        false
    ))
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
    char acknowledgement[
        CHAOSFORGE_ACK_MESSAGE_SIZE
    ];

    char successValue[6];

    if (success)
    {
        strcopy(
            successValue,
            sizeof(successValue),
            "true"
        );
    }
    else
    {
        strcopy(
            successValue,
            sizeof(successValue),
            "false"
        );
    }

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

bool ChaosDispatcher_SpawnInfected(
    const char[] infected
)
{
    if (StrEqual(infected, "Hunter", false))
    {
        return ChaosSpawn_Hunter();
    }

    if (StrEqual(infected, "Smoker", false))
    {
        return ChaosSpawn_Smoker();
    }

    if (StrEqual(infected, "Boomer", false))
    {
        return ChaosSpawn_Boomer();
    }

    if (StrEqual(infected, "Charger", false))
    {
        return ChaosSpawn_Charger();
    }

    if (StrEqual(infected, "Jockey", false))
    {
        return ChaosSpawn_Jockey();
    }

    if (StrEqual(infected, "Spitter", false))
    {
        return ChaosSpawn_Spitter();
    }

    if (StrEqual(infected, "Tank", false))
    {
        return ChaosSpawn_Tank();
    }

    if (StrEqual(infected, "Witch", false))
    {
        return ChaosSpawn_Witch();
    }

    PrintToServer(
        "[ChaosForge] Unsupported infected type: %s",
        infected
    );

    return false;
}