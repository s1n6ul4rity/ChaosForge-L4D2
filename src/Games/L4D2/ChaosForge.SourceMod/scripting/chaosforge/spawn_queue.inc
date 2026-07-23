#if defined _chaosforge_spawn_queue_included
    #endinput
#endif
#define _chaosforge_spawn_queue_included

#define CHAOSFORGE_SPAWN_EVENT_ID_SIZE 64
#define CHAOSFORGE_SPAWN_INFECTED_SIZE 32
#define CHAOSFORGE_SPAWN_VIEWER_NAME_SIZE 128

#define CHAOSFORGE_SPAWN_INTERVAL 0.20
#define CHAOSFORGE_SPAWN_RETRY_DELAY 1.00
#define CHAOSFORGE_SPAWN_REQUEST_TIMEOUT 120.00

/*
 * Initial safety caps based on current local testing.
 * These should move to configuration later.
 */
#define CHAOSFORGE_MAX_ACTIVE_HUNTERS 3
#define CHAOSFORGE_MAX_ACTIVE_SMOKERS 3
#define CHAOSFORGE_MAX_ACTIVE_BOOMERS 3
#define CHAOSFORGE_MAX_ACTIVE_SPITTERS 3
#define CHAOSFORGE_MAX_ACTIVE_JOCKEYS 3
#define CHAOSFORGE_MAX_ACTIVE_CHARGERS 3
#define CHAOSFORGE_MAX_ACTIVE_TANKS 4
#define CHAOSFORGE_MAX_ACTIVE_WITCHES 32

enum struct ChaosSpawnJob
{
    Handle Socket;

    char EventId[CHAOSFORGE_SPAWN_EVENT_ID_SIZE];
    char Infected[CHAOSFORGE_SPAWN_INFECTED_SIZE];
    char ViewerName[CHAOSFORGE_SPAWN_VIEWER_NAME_SIZE];

    int RequestedCount;
    int ExecutedCount;
    int AttemptCount;

    int PendingEntityRef;
    bool WaitingForVerification;

    float CreatedAt;
    float NextAttemptAt;
}

ArrayList g_ChaosSpawnQueue = null;
Handle g_ChaosSpawnTimer = null;

void ChaosSpawnQueue_EnsureStarted()
{
    if (g_ChaosSpawnQueue == null)
    {
        g_ChaosSpawnQueue = new ArrayList(sizeof(ChaosSpawnJob));
    }

    if (g_ChaosSpawnTimer == null)
    {
        g_ChaosSpawnTimer = CreateTimer(
            CHAOSFORGE_SPAWN_INTERVAL,
            ChaosSpawnQueue_ProcessTimer,
            _,
            TIMER_REPEAT | TIMER_FLAG_NO_MAPCHANGE
        );
    }
}

public bool ChaosSpawnQueue_Enqueue(
    WebSocket socket,
    const char[] eventId,
    const char[] infected,
    int requestedCount,
    const char[] viewerName
)
{
    ChaosSpawnQueue_EnsureStarted();

    ChaosSpawnJob job;

    job.Socket = view_as<Handle>(socket);

    strcopy(job.EventId, sizeof(job.EventId), eventId);
    strcopy(job.Infected, sizeof(job.Infected), infected);
    strcopy(job.ViewerName,sizeof(job.ViewerName),viewerName);

    job.RequestedCount = requestedCount > 0 ? requestedCount : 1;
    job.ExecutedCount = 0;
    job.AttemptCount = 0;

    job.PendingEntityRef = INVALID_ENT_REFERENCE;
    job.WaitingForVerification = false;

    job.CreatedAt = GetGameTime();
    job.NextAttemptAt = job.CreatedAt;

    g_ChaosSpawnQueue.PushArray(job, sizeof(job));

    PrintToServer(
        "[ChaosForge] Queued %d %s spawn(s) for event %s. Queue depth: %d.",
        job.RequestedCount,
        job.Infected,
        job.EventId,
        g_ChaosSpawnQueue.Length
    );

    return true;
}

int ChaosSpawnQueue_GetActiveLimit(
    const char[] infected
)
{
    if (StrEqual(infected, "Hunter", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_HUNTERS;
    }

    if (StrEqual(infected, "Smoker", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_SMOKERS;
    }

    if (StrEqual(infected, "Boomer", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_BOOMERS;
    }

    if (StrEqual(infected, "Spitter", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_SPITTERS;
    }

    if (StrEqual(infected, "Jockey", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_JOCKEYS;
    }

    if (StrEqual(infected, "Charger", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_CHARGERS;
    }

    if (StrEqual(infected, "Tank", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_TANKS;
    }

    if (StrEqual(infected, "Witch", false))
    {
        return CHAOSFORGE_MAX_ACTIVE_WITCHES;
    }

    return 1;
}

bool ChaosSpawnQueue_HasCapacity(
    const char[] infected
)
{
    int active =
        ChaosGameState_GetAliveByName(infected);

    int limit =
        ChaosSpawnQueue_GetActiveLimit(infected);

    return active < limit;
}

bool ChaosSpawnQueue_IsEntityConfirmed(int entity)
{
    if (entity == INVALID_ENT_REFERENCE || entity <= 0)
    {
        return false;
    }

    if (entity <= MaxClients)
    {
        return IsClientInGame(entity)
            && IsPlayerAlive(entity)
            && GetClientTeam(entity) == 3;
    }

    return IsValidEntity(entity);
}

void ChaosSpawnQueue_SendFinalResult(
    ChaosSpawnJob job,
    bool success,
    const char[] message
)
{
    ChaosDispatcher_SendAcknowledgement(
        view_as<WebSocket>(job.Socket),
        job.EventId,
        success,
        job.RequestedCount,
        job.ExecutedCount,
        message
    );

    PrintToServer(
        "[ChaosForge] Spawn event %s finished at %d/%d after %d attempt(s). Success: %s.",
        job.EventId,
        job.ExecutedCount,
        job.RequestedCount,
        job.AttemptCount,
        success ? "true" : "false"
    );
}

void ChaosSpawnQueue_Wake()
{
    if (g_ChaosSpawnQueue == null
        || g_ChaosSpawnQueue.Length == 0)
    {
        return;
    }

    ChaosSpawnJob job;

    g_ChaosSpawnQueue.GetArray(
        0,
        job,
        sizeof(job)
    );

    if (job.WaitingForVerification)
    {
        return;
    }

    job.NextAttemptAt = GetGameTime();

    g_ChaosSpawnQueue.SetArray(
        0,
        job,
        sizeof(job)
    );

    PrintToServer(
        "[ChaosForge] Spawn queue awakened for event %s.",
        job.EventId
    );
}

public Action ChaosSpawnQueue_ProcessTimer(
    Handle timer,
    any data
)
{
    if (g_ChaosSpawnQueue == null
        || g_ChaosSpawnQueue.Length == 0)
    {
        return Plugin_Continue;
    }

    ChaosSpawnJob job;
    g_ChaosSpawnQueue.GetArray(0, job, sizeof(job));

    float now = GetGameTime();

    if ((now - job.CreatedAt) >= CHAOSFORGE_SPAWN_REQUEST_TIMEOUT)
    {
        ChaosSpawnQueue_SendFinalResult(
            job,
            false,
            "Spawn request timed out before all requested infected were confirmed."
        );

        g_ChaosSpawnQueue.Erase(0);
        return Plugin_Continue;
    }

    if (job.WaitingForVerification)
    {
        int entity =
            EntRefToEntIndex(job.PendingEntityRef);

        if (ChaosSpawnQueue_IsEntityConfirmed(entity))
        {
            ChaosViewerLabel_Assign(
                entity,
                job.ViewerName,
                job.Infected
            );

            job.ExecutedCount++;

            PrintToServer(
                "[ChaosForge] Confirmed %s entity %d for viewer %s in event %s (%d/%d).",
                job.Infected,
                entity,
                job.ViewerName,
                job.EventId,
                job.ExecutedCount,
                job.RequestedCount
            );

            job.NextAttemptAt = now;
        }
        else
        {
            PrintToServer(
                "[ChaosForge] Verification failed for %s in event %s. Waiting for an available game slot.",
                job.Infected,
                job.EventId
            );

            job.NextAttemptAt =
                now + CHAOSFORGE_SPAWN_RETRY_DELAY;
        }

        job.PendingEntityRef = INVALID_ENT_REFERENCE;
        job.WaitingForVerification = false;

        if (job.ExecutedCount >= job.RequestedCount)
        {
            ChaosSpawnQueue_SendFinalResult(
                job,
                true,
                "All requested special infected were confirmed in-game."
            );

            g_ChaosSpawnQueue.Erase(0);
            return Plugin_Continue;
        }

        g_ChaosSpawnQueue.SetArray(0, job, sizeof(job));
        return Plugin_Continue;
    }

    if (now < job.NextAttemptAt)
    {
        return Plugin_Continue;
    }

    /*
     * Consult the cached game state before making a native call.
     * This avoids repeatedly asking L4D2 for a spawn while the observed
     * class capacity is already full.
     */
    if (!ChaosSpawnQueue_HasCapacity(job.Infected))
    {
        job.NextAttemptAt =
            now + CHAOSFORGE_SPAWN_RETRY_DELAY;

        g_ChaosSpawnQueue.SetArray(0, job, sizeof(job));

        return Plugin_Continue;
    }

    job.AttemptCount++;

    int spawnedEntity =
        ChaosDispatcher_SpawnInfectedEntity(
            job.Infected
        );

    if (spawnedEntity > 0)
    {
        job.PendingEntityRef =
            EntIndexToEntRef(spawnedEntity);

        job.WaitingForVerification = true;

        PrintToServer(
            "[ChaosForge] Waiting to verify %s entity %d for event %s.",
            job.Infected,
            spawnedEntity,
            job.EventId
        );
    }
    else
    {
        PrintToServer(
            "[ChaosForge] %s spawn attempt %d failed for event %s. Waiting for an available game slot.",
            job.Infected,
            job.AttemptCount,
            job.EventId
        );

        job.NextAttemptAt =
            now + CHAOSFORGE_SPAWN_RETRY_DELAY;
    }

    g_ChaosSpawnQueue.SetArray(0, job, sizeof(job));
    return Plugin_Continue;
}
