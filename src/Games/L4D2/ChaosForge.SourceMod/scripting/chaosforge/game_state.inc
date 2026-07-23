#if defined _chaosforge_game_state_included
    #endinput
#endif
#define _chaosforge_game_state_included

#define CHAOSFORGE_TEAM_SURVIVOR 2
#define CHAOSFORGE_TEAM_INFECTED 3

#define CHAOSFORGE_ZOMBIECLASS_SMOKER 1
#define CHAOSFORGE_ZOMBIECLASS_BOOMER 2
#define CHAOSFORGE_ZOMBIECLASS_HUNTER 3
#define CHAOSFORGE_ZOMBIECLASS_SPITTER 4
#define CHAOSFORGE_ZOMBIECLASS_JOCKEY 5
#define CHAOSFORGE_ZOMBIECLASS_CHARGER 6
#define CHAOSFORGE_ZOMBIECLASS_TANK 8

int g_ChaosGameState_LivingSurvivors = 0;

int g_ChaosGameState_AliveSmokers = 0;
int g_ChaosGameState_AliveBoomers = 0;
int g_ChaosGameState_AliveHunters = 0;
int g_ChaosGameState_AliveSpitters = 0;
int g_ChaosGameState_AliveJockeys = 0;
int g_ChaosGameState_AliveChargers = 0;
int g_ChaosGameState_AliveTanks = 0;
int g_ChaosGameState_AliveWitches = 0;

bool g_ChaosGameState_RoundLive = false;
bool g_ChaosGameState_HasSnapshot = false;

void ChaosGameState_Initialize()
{
    HookEvent(
        "round_start",
        ChaosGameState_OnRoundStart,
        EventHookMode_PostNoCopy
    );

    HookEvent(
        "round_end",
        ChaosGameState_OnRoundEnd,
        EventHookMode_PostNoCopy
    );

    HookEvent(
        "player_spawn",
        ChaosGameState_OnPlayerChanged,
        EventHookMode_Post
    );

    HookEvent(
        "player_death",
        ChaosGameState_OnPlayerChanged,
        EventHookMode_Post
    );

    HookEvent(
        "player_team",
        ChaosGameState_OnPlayerChanged,
        EventHookMode_Post
    );

    HookEvent(
        "witch_spawn",
        ChaosGameState_OnWorldChanged,
        EventHookMode_Post
    );

    HookEvent(
        "witch_killed",
        ChaosGameState_OnWorldChanged,
        EventHookMode_Post
    );

    ChaosGameState_Refresh();
}

public void ChaosGameState_OnRoundStart(
    Event event,
    const char[] name,
    bool dontBroadcast
)
{
    g_ChaosGameState_RoundLive = true;
    ChaosGameState_Refresh();
}

public void ChaosGameState_OnRoundEnd(
    Event event,
    const char[] name,
    bool dontBroadcast
)
{
    g_ChaosGameState_RoundLive = false;
    ChaosGameState_Refresh();
}

public void ChaosGameState_OnPlayerChanged(
    Event event,
    const char[] name,
    bool dontBroadcast
)
{
    RequestFrame(ChaosGameState_RefreshNextFrame);
}

public void ChaosGameState_OnWorldChanged(
    Event event,
    const char[] name,
    bool dontBroadcast
)
{
    RequestFrame(ChaosGameState_RefreshNextFrame);
}

public void ChaosGameState_RefreshNextFrame(any data)
{
    ChaosGameState_Refresh();
}

void ChaosGameState_Refresh()
{
    int livingSurvivors = 0;

    int aliveSmokers = 0;
    int aliveBoomers = 0;
    int aliveHunters = 0;
    int aliveSpitters = 0;
    int aliveJockeys = 0;
    int aliveChargers = 0;
    int aliveTanks = 0;
    int aliveWitches = 0;

    for (int client = 1; client <= MaxClients; client++)
    {
        if (!IsClientInGame(client) || !IsPlayerAlive(client))
        {
            continue;
        }

        int team = GetClientTeam(client);

        if (team == CHAOSFORGE_TEAM_SURVIVOR)
        {
            livingSurvivors++;
            continue;
        }

        if (team != CHAOSFORGE_TEAM_INFECTED)
        {
            continue;
        }

        int zombieClass = GetEntProp(
            client,
            Prop_Send,
            "m_zombieClass"
        );

        switch (zombieClass)
        {
            case CHAOSFORGE_ZOMBIECLASS_SMOKER:
            {
                aliveSmokers++;
            }

            case CHAOSFORGE_ZOMBIECLASS_BOOMER:
            {
                aliveBoomers++;
            }

            case CHAOSFORGE_ZOMBIECLASS_HUNTER:
            {
                aliveHunters++;
            }

            case CHAOSFORGE_ZOMBIECLASS_SPITTER:
            {
                aliveSpitters++;
            }

            case CHAOSFORGE_ZOMBIECLASS_JOCKEY:
            {
                aliveJockeys++;
            }

            case CHAOSFORGE_ZOMBIECLASS_CHARGER:
            {
                aliveChargers++;
            }

            case CHAOSFORGE_ZOMBIECLASS_TANK:
            {
                aliveTanks++;
            }
        }
    }

    int witch = -1;

    while ((witch = FindEntityByClassname(witch, "witch")) != -1)
    {
        if (IsValidEntity(witch))
        {
            aliveWitches++;
        }
    }

    bool changed =
        !g_ChaosGameState_HasSnapshot
        || livingSurvivors != g_ChaosGameState_LivingSurvivors
        || aliveSmokers != g_ChaosGameState_AliveSmokers
        || aliveBoomers != g_ChaosGameState_AliveBoomers
        || aliveHunters != g_ChaosGameState_AliveHunters
        || aliveSpitters != g_ChaosGameState_AliveSpitters
        || aliveJockeys != g_ChaosGameState_AliveJockeys
        || aliveChargers != g_ChaosGameState_AliveChargers
        || aliveTanks != g_ChaosGameState_AliveTanks
        || aliveWitches != g_ChaosGameState_AliveWitches;

    g_ChaosGameState_LivingSurvivors = livingSurvivors;

    g_ChaosGameState_AliveSmokers = aliveSmokers;
    g_ChaosGameState_AliveBoomers = aliveBoomers;
    g_ChaosGameState_AliveHunters = aliveHunters;
    g_ChaosGameState_AliveSpitters = aliveSpitters;
    g_ChaosGameState_AliveJockeys = aliveJockeys;
    g_ChaosGameState_AliveChargers = aliveChargers;
    g_ChaosGameState_AliveTanks = aliveTanks;
    g_ChaosGameState_AliveWitches = aliveWitches;

    g_ChaosGameState_HasSnapshot = true;

    if (!changed)
    {
        return;
    }

    PrintToServer(
        "[ChaosForge] Game state changed. Survivors: %d | SI: %d | Hunters: %d | Tanks: %d | Witches: %d.",
        g_ChaosGameState_LivingSurvivors,
        ChaosGameState_GetAliveSpecialCount(),
        g_ChaosGameState_AliveHunters,
        g_ChaosGameState_AliveTanks,
        g_ChaosGameState_AliveWitches
    );
}

int ChaosGameState_GetLivingSurvivorCount()
{
    return g_ChaosGameState_LivingSurvivors;
}

int ChaosGameState_GetAliveSpecialCount()
{
    return g_ChaosGameState_AliveSmokers
        + g_ChaosGameState_AliveBoomers
        + g_ChaosGameState_AliveHunters
        + g_ChaosGameState_AliveSpitters
        + g_ChaosGameState_AliveJockeys
        + g_ChaosGameState_AliveChargers;
}

int ChaosGameState_GetAliveHunters()
{
    return g_ChaosGameState_AliveHunters;
}

int ChaosGameState_GetAliveSmokers()
{
    return g_ChaosGameState_AliveSmokers;
}

int ChaosGameState_GetAliveBoomers()
{
    return g_ChaosGameState_AliveBoomers;
}

int ChaosGameState_GetAliveSpitters()
{
    return g_ChaosGameState_AliveSpitters;
}

int ChaosGameState_GetAliveJockeys()
{
    return g_ChaosGameState_AliveJockeys;
}

int ChaosGameState_GetAliveChargers()
{
    return g_ChaosGameState_AliveChargers;
}

int ChaosGameState_GetAliveTanks()
{
    return g_ChaosGameState_AliveTanks;
}

int ChaosGameState_GetAliveWitches()
{
    return g_ChaosGameState_AliveWitches;
}

int ChaosGameState_GetAliveByName(
    const char[] infected
)
{
    if (StrEqual(infected, "Hunter", false))
    {
        return ChaosGameState_GetAliveHunters();
    }

    if (StrEqual(infected, "Smoker", false))
    {
        return ChaosGameState_GetAliveSmokers();
    }

    if (StrEqual(infected, "Boomer", false))
    {
        return ChaosGameState_GetAliveBoomers();
    }

    if (StrEqual(infected, "Spitter", false))
    {
        return ChaosGameState_GetAliveSpitters();
    }

    if (StrEqual(infected, "Jockey", false))
    {
        return ChaosGameState_GetAliveJockeys();
    }

    if (StrEqual(infected, "Charger", false))
    {
        return ChaosGameState_GetAliveChargers();
    }

    if (StrEqual(infected, "Tank", false))
    {
        return ChaosGameState_GetAliveTanks();
    }

    if (StrEqual(infected, "Witch", false))
    {
        return ChaosGameState_GetAliveWitches();
    }

    return 0;
}

bool ChaosGameState_IsRoundLive()
{
    return g_ChaosGameState_RoundLive;
}
