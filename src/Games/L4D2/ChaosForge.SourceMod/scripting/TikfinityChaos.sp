#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <sdktools>

#define PLUGIN_VERSION "0.2.1"
#define MAX_GIFTER_NAME 64
#define MAX_INFECTED_NAME 24
#define MAX_TRACKED_ENTITIES 2049

public Plugin myinfo =
{
    name = "TikFinity Chaos",
    author = "Open-source community starter pack",
    description = "Spawns L4D2 infected and attaches TikFinity gifter labels.",
    version = PLUGIN_VERSION,
    url = ""
};

ConVar g_CvarEnabled;
ConVar g_CvarLabels;
ConVar g_CvarLabelDuration;
ConVar g_CvarMaxCount;
ConVar g_CvarAnnounce;
ConVar g_CvarCrosshairSpawn;
ConVar g_CvarSpawnHeight;
ConVar g_CvarLabelHeight;

int g_HintForEntity[MAX_TRACKED_ENTITIES];
int g_HintTargetForEntity[MAX_TRACKED_ENTITIES];
char g_GifterForEntity[MAX_TRACKED_ENTITIES][MAX_GIFTER_NAME];
char g_InfectedForEntity[MAX_TRACKED_ENTITIES][MAX_INFECTED_NAME];

bool g_Pending;
char g_PendingGifter[MAX_GIFTER_NAME];
char g_PendingType[MAX_INFECTED_NAME];
int g_PendingCount;
float g_PendingExpires;
float g_PendingPosition[3];
bool g_PendingHasPosition;
int g_PendingSpawnSerial;

public void OnPluginStart()
{
    g_CvarEnabled = CreateConVar("sm_tfc_enabled", "1", "Enable TikFinity Chaos.", FCVAR_NOTIFY, true, 0.0, true, 1.0);
    g_CvarLabels = CreateConVar("sm_tfc_labels", "1", "Show gifter labels above spawned infected.", FCVAR_NOTIFY, true, 0.0, true, 1.0);
    g_CvarLabelDuration = CreateConVar("sm_tfc_label_duration", "30.0", "Label lifetime in seconds. 0 keeps it until death.", FCVAR_NOTIFY, true, 0.0, true, 300.0);
    g_CvarMaxCount = CreateConVar("sm_tfc_max_spawn_count", "5", "Maximum infected spawned by one command.", FCVAR_NOTIFY, true, 1.0, true, 20.0);
    g_CvarAnnounce = CreateConVar("sm_tfc_announce", "1", "Announce gift-triggered spawns in chat.", FCVAR_NOTIFY, true, 0.0, true, 1.0);
    g_CvarCrosshairSpawn = CreateConVar("sm_tfc_crosshair_spawn", "1", "Move spawned infected to the command player's crosshair.", FCVAR_NOTIFY, true, 0.0, true, 1.0);
    g_CvarSpawnHeight = CreateConVar("sm_tfc_spawn_height", "12.0", "Vertical offset above the crosshair hit point.", FCVAR_NOTIFY, true, -64.0, true, 128.0);
    g_CvarLabelHeight = CreateConVar("sm_tfc_label_height", "105.0", "Height of the gifter label above infected.", FCVAR_NOTIFY, true, 40.0, true, 250.0);

    AutoExecConfig(true, "tikfinity_chaos");

    RegAdminCmd("sm_chaos_spawn", Command_ChaosSpawn, ADMFLAG_ROOT,
        "sm_chaos_spawn <tank|hunter|smoker|charger|jockey|spitter|boomer|witch> <gifter name> [count]");
    RegAdminCmd("sm_spawn_tank", Command_SpawnTank, ADMFLAG_ROOT, "sm_spawn_tank <gifter name> [count]");
    RegAdminCmd("sm_spawn_hunter", Command_SpawnHunter, ADMFLAG_ROOT, "sm_spawn_hunter <gifter name> [count]");
    RegAdminCmd("sm_spawn_smoker", Command_SpawnSmoker, ADMFLAG_ROOT, "sm_spawn_smoker <gifter name> [count]");
    RegAdminCmd("sm_spawn_charger", Command_SpawnCharger, ADMFLAG_ROOT, "sm_spawn_charger <gifter name> [count]");
    RegAdminCmd("sm_spawn_jockey", Command_SpawnJockey, ADMFLAG_ROOT, "sm_spawn_jockey <gifter name> [count]");
    RegAdminCmd("sm_spawn_spitter", Command_SpawnSpitter, ADMFLAG_ROOT, "sm_spawn_spitter <gifter name> [count]");
    RegAdminCmd("sm_spawn_boomer", Command_SpawnBoomer, ADMFLAG_ROOT, "sm_spawn_boomer <gifter name> [count]");
    RegAdminCmd("sm_spawn_witch", Command_SpawnWitch, ADMFLAG_ROOT, "sm_spawn_witch <gifter name> [count]");

    HookEvent("player_spawn", Event_PlayerSpawn, EventHookMode_Post);
    HookEvent("player_death", Event_PlayerDeath, EventHookMode_Post);
    HookEvent("round_end", Event_RoundEnd, EventHookMode_PostNoCopy);

    for (int i = 0; i < MAX_TRACKED_ENTITIES; i++)
    {
        g_HintForEntity[i] = -1;
        g_HintTargetForEntity[i] = -1;
    }
}

public void OnMapStart()
{
    ResetTracking();
}

public void OnMapEnd()
{
    ResetTracking();
}

public void OnEntityCreated(int entity, const char[] classname)
{
    if (!g_Pending || GetGameTime() > g_PendingExpires)
    {
        return;
    }

    if (StrEqual(g_PendingType, "witch", false) && StrEqual(classname, "witch", false))
    {
        RequestFrame(Frame_AttachWitchLabel, EntIndexToEntRef(entity));
    }
}

public void OnEntityDestroyed(int entity)
{
    if (entity > 0 && entity < MAX_TRACKED_ENTITIES)
    {
        RemoveGifterLabel(entity);
    }
}

public Action Command_ChaosSpawn(int client, int args)
{
    if (args < 2)
    {
        ReplyToCommand(client, "[TikFinity] Usage: sm_chaos_spawn <type> <gifter name> [count]");
        return Plugin_Handled;
    }

    char infectedType[MAX_INFECTED_NAME];
    char gifter[MAX_GIFTER_NAME];
    GetCmdArg(1, infectedType, sizeof(infectedType));
    GetCmdArg(2, gifter, sizeof(gifter));

    int count = 1;
    if (args >= 3)
    {
        char countText[12];
        GetCmdArg(3, countText, sizeof(countText));
        count = StringToInt(countText);
    }

    return StartSpawn(client, infectedType, gifter, count);
}

public Action Command_SpawnTank(int client, int args)    { return HandleShortcut(client, args, "tank"); }
public Action Command_SpawnHunter(int client, int args)  { return HandleShortcut(client, args, "hunter"); }
public Action Command_SpawnSmoker(int client, int args)  { return HandleShortcut(client, args, "smoker"); }
public Action Command_SpawnCharger(int client, int args) { return HandleShortcut(client, args, "charger"); }
public Action Command_SpawnJockey(int client, int args)  { return HandleShortcut(client, args, "jockey"); }
public Action Command_SpawnSpitter(int client, int args) { return HandleShortcut(client, args, "spitter"); }
public Action Command_SpawnBoomer(int client, int args)  { return HandleShortcut(client, args, "boomer"); }
public Action Command_SpawnWitch(int client, int args)   { return HandleShortcut(client, args, "witch"); }

Action HandleShortcut(int client, int args, const char[] infectedType)
{
    if (args < 1)
    {
        ReplyToCommand(client, "[TikFinity] Usage: command <gifter name> [count]");
        return Plugin_Handled;
    }

    char gifter[MAX_GIFTER_NAME];
    GetCmdArg(1, gifter, sizeof(gifter));

    int count = 1;
    if (args >= 2)
    {
        char countText[12];
        GetCmdArg(2, countText, sizeof(countText));
        count = StringToInt(countText);
    }

    return StartSpawn(client, infectedType, gifter, count);
}

Action StartSpawn(int client, const char[] infectedType, const char[] rawGifter, int count)
{
    if (!g_CvarEnabled.BoolValue)
    {
        ReplyToCommand(client, "[TikFinity] Plugin is disabled.");
        return Plugin_Handled;
    }

    if (!IsSupportedType(infectedType))
    {
        ReplyToCommand(client, "[TikFinity] Unsupported infected type: %s", infectedType);
        return Plugin_Handled;
    }

    count = ClampInt(count, 1, g_CvarMaxCount.IntValue);

    char gifter[MAX_GIFTER_NAME];
    SanitizeGifterName(rawGifter, gifter, sizeof(gifter));
    if (gifter[0] == '\0')
    {
        strcopy(gifter, sizeof(gifter), "Viewer");
    }

    strcopy(g_PendingGifter, sizeof(g_PendingGifter), gifter);
    strcopy(g_PendingType, sizeof(g_PendingType), infectedType);
    g_PendingCount = count;
    g_PendingExpires = GetGameTime() + 4.0;
    g_Pending = true;

    int executor = FindSpawnExecutor(client);
    if (executor <= 0)
    {
        ReplyToCommand(client, "[TikFinity] No connected player is available to execute the spawn command.");
        ClearPending();
        return Plugin_Handled;
    }

    g_PendingHasPosition = false;
    if (g_CvarCrosshairSpawn.BoolValue && IsClientInGame(executor) && IsPlayerAlive(executor))
    {
        g_PendingHasPosition = GetCrosshairPosition(executor, g_PendingPosition);
        if (!g_PendingHasPosition)
        {
            ReplyToCommand(client, "[TikFinity] Crosshair did not hit the world; using Director spawn placement.");
        }
    }
    g_PendingSpawnSerial = 0;

    int commandFlags = GetCommandFlags("z_spawn_old");
    if (commandFlags == INVALID_FCVAR_FLAGS)
    {
        ReplyToCommand(client, "[TikFinity] L4D2 command z_spawn_old was not found.");
        ClearPending();
        return Plugin_Handled;
    }

    // z_spawn_old is a client command in L4D2. Running it with ServerCommand
    // can silently do nothing, so execute it through a connected player.
    SetCommandFlags("z_spawn_old", commandFlags & ~FCVAR_CHEAT);
    for (int i = 0; i < count; i++)
    {
        FakeClientCommand(executor, "z_spawn_old %s auto", infectedType);
    }
    SetCommandFlags("z_spawn_old", commandFlags);

    if (g_CvarAnnounce.BoolValue)
    {
        PrintToChatAll("\x04[TikFinity]\x01 %s spawned %d %s%s!", gifter, count, infectedType, count == 1 ? "" : "s");
    }

    ReplyToCommand(client, "[TikFinity] Requested %d %s spawn(s) for %s.", count, infectedType, gifter);
    CreateTimer(4.1, Timer_ClearExpiredPending, _, TIMER_FLAG_NO_MAPCHANGE);
    return Plugin_Handled;
}

public void Event_PlayerSpawn(Event event, const char[] name, bool dontBroadcast)
{
    if (!g_Pending || GetGameTime() > g_PendingExpires || StrEqual(g_PendingType, "witch", false))
    {
        return;
    }

    int client = GetClientOfUserId(event.GetInt("userid"));
    if (client <= 0 || !IsClientInGame(client) || GetClientTeam(client) != 3)
    {
        return;
    }

    int zombieClass = GetEntProp(client, Prop_Send, "m_zombieClass");
    if (!ZombieClassMatches(g_PendingType, zombieClass))
    {
        return;
    }

    MovePendingSpawnToCrosshair(client);
    AttachGifterLabel(client, g_PendingGifter, g_PendingType);
    ConsumePendingSpawn();
}

public void Event_PlayerDeath(Event event, const char[] name, bool dontBroadcast)
{
    int client = GetClientOfUserId(event.GetInt("userid"));
    if (client > 0)
    {
        RemoveGifterLabel(client);
    }
}

public void Event_RoundEnd(Event event, const char[] name, bool dontBroadcast)
{
    ResetTracking();
}

public void Frame_AttachWitchLabel(any entRef)
{
    int entity = EntRefToEntIndex(entRef);
    if (entity == INVALID_ENT_REFERENCE || !IsValidEntity(entity) || !g_Pending)
    {
        return;
    }

    MovePendingSpawnToCrosshair(entity);
    AttachGifterLabel(entity, g_PendingGifter, "witch");
    ConsumePendingSpawn();
}

void MovePendingSpawnToCrosshair(int entity)
{
    if (!g_PendingHasPosition || entity <= 0 || !IsValidEntity(entity))
    {
        return;
    }

    float position[3];
    position[0] = g_PendingPosition[0];
    position[1] = g_PendingPosition[1];
    position[2] = g_PendingPosition[2] + g_CvarSpawnHeight.FloatValue;

    // Separate multiple spawns slightly so they do not occupy exactly the same space.
    if (g_PendingSpawnSerial > 0)
    {
        float angle = float(g_PendingSpawnSerial) * 2.3999632;
        float radius = 36.0 * SquareRoot(float(g_PendingSpawnSerial));
        position[0] += Cosine(angle) * radius;
        position[1] += Sine(angle) * radius;
    }

    float zeroVelocity[3] = {0.0, 0.0, 0.0};
    TeleportEntity(entity, position, NULL_VECTOR, zeroVelocity);
    g_PendingSpawnSerial++;
}

bool GetCrosshairPosition(int client, float result[3])
{
    float eyePosition[3];
    float eyeAngles[3];
    GetClientEyePosition(client, eyePosition);
    GetClientEyeAngles(client, eyeAngles);

    Handle trace = TR_TraceRayFilterEx(
        eyePosition,
        eyeAngles,
        MASK_PLAYERSOLID,
        RayType_Infinite,
        TraceFilter_IgnorePlayers,
        client
    );

    bool hit = TR_DidHit(trace);
    if (hit)
    {
        TR_GetEndPosition(result, trace);
    }
    delete trace;
    return hit;
}

public bool TraceFilter_IgnorePlayers(int entity, int contentsMask, any data)
{
    if (entity == data)
    {
        return false;
    }
    if (entity >= 1 && entity <= MaxClients)
    {
        return false;
    }
    return true;
}

void AttachGifterLabel(int entity, const char[] gifter, const char[] infectedType)
{
    if (!g_CvarLabels.BoolValue || entity <= 0 || entity >= MAX_TRACKED_ENTITIES || !IsValidEntity(entity))
    {
        return;
    }

    RemoveGifterLabel(entity);

    // A dedicated replicated hint target is more reliable than pointing the
    // instructor hint directly at an infected player/bot entity.
    int target = CreateEntityByName("info_target_instructor_hint");
    if (target == -1 || !IsValidEntity(target))
    {
        LogError("Could not create info_target_instructor_hint for entity %d.", entity);
        return;
    }

    char targetName[64];
    Format(targetName, sizeof(targetName), "tfc_label_target_%d_%d", entity, GetTime());
    DispatchKeyValue(target, "targetname", targetName);
    DispatchSpawn(target);
    ActivateEntity(target);

    float entityOrigin[3];
    GetEntPropVector(entity, Prop_Send, "m_vecOrigin", entityOrigin);
    entityOrigin[2] += g_CvarLabelHeight.FloatValue;
    TeleportEntity(target, entityOrigin, NULL_VECTOR, NULL_VECTOR);

    SetVariantString("!activator");
    AcceptEntityInput(target, "SetParent", entity, target);

    int hint = CreateEntityByName("env_instructor_hint");
    if (hint == -1 || !IsValidEntity(hint))
    {
        AcceptEntityInput(target, "Kill");
        LogError("Could not create env_instructor_hint for entity %d.", entity);
        return;
    }

    char caption[160];
    Format(caption, sizeof(caption), "%s - %s", gifter, infectedType);

    char timeout[16];
    FloatToString(g_CvarLabelDuration.FloatValue, timeout, sizeof(timeout));

    DispatchKeyValue(hint, "hint_target", targetName);
    DispatchKeyValue(hint, "hint_caption", caption);
    DispatchKeyValue(hint, "hint_timeout", timeout);
    DispatchKeyValue(hint, "hint_range", "0");
    DispatchKeyValue(hint, "hint_icon_onscreen", "icon_alert");
    DispatchKeyValue(hint, "hint_icon_offscreen", "icon_alert");
    DispatchKeyValue(hint, "hint_color", "255 215 0");
    DispatchKeyValue(hint, "hint_static", "0");
    DispatchKeyValue(hint, "hint_forcecaption", "1");
    DispatchKeyValue(hint, "hint_nooffscreen", "0");
    DispatchKeyValue(hint, "hint_allow_nodraw_target", "1");

    DispatchSpawn(hint);
    ActivateEntity(hint);
    AcceptEntityInput(hint, "ShowHint");

    g_HintForEntity[entity] = EntIndexToEntRef(hint);
    g_HintTargetForEntity[entity] = EntIndexToEntRef(target);
    strcopy(g_GifterForEntity[entity], sizeof(g_GifterForEntity[]), gifter);
    strcopy(g_InfectedForEntity[entity], sizeof(g_InfectedForEntity[]), infectedType);

    float duration = g_CvarLabelDuration.FloatValue;
    if (duration > 0.0)
    {
        DataPack pack = new DataPack();
        pack.WriteCell(EntIndexToEntRef(entity));
        pack.WriteCell(EntIndexToEntRef(hint));
        pack.WriteCell(EntIndexToEntRef(target));
        CreateTimer(duration, Timer_RemoveLabel, pack, TIMER_FLAG_NO_MAPCHANGE | TIMER_DATA_HNDL_CLOSE);
    }
}

public Action Timer_RemoveLabel(Handle timer, DataPack pack)
{
    pack.Reset();
    int entity = EntRefToEntIndex(pack.ReadCell());
    int hint = EntRefToEntIndex(pack.ReadCell());
    int target = EntRefToEntIndex(pack.ReadCell());

    if (hint != INVALID_ENT_REFERENCE && IsValidEntity(hint))
    {
        AcceptEntityInput(hint, "EndHint");
        AcceptEntityInput(hint, "Kill");
    }
    if (target != INVALID_ENT_REFERENCE && IsValidEntity(target))
    {
        AcceptEntityInput(target, "Kill");
    }

    if (entity != INVALID_ENT_REFERENCE && entity > 0 && entity < MAX_TRACKED_ENTITIES)
    {
        g_HintForEntity[entity] = -1;
        g_HintTargetForEntity[entity] = -1;
        g_GifterForEntity[entity][0] = '\0';
        g_InfectedForEntity[entity][0] = '\0';
    }
    return Plugin_Stop;
}

public Action Timer_ClearExpiredPending(Handle timer)
{
    if (g_Pending && GetGameTime() > g_PendingExpires)
    {
        ClearPending();
    }
    return Plugin_Stop;
}

void RemoveGifterLabel(int entity)
{
    if (entity <= 0 || entity >= MAX_TRACKED_ENTITIES)
    {
        return;
    }

    int hint = EntRefToEntIndex(g_HintForEntity[entity]);
    if (hint != INVALID_ENT_REFERENCE && IsValidEntity(hint))
    {
        AcceptEntityInput(hint, "EndHint");
        AcceptEntityInput(hint, "Kill");
    }

    int target = EntRefToEntIndex(g_HintTargetForEntity[entity]);
    if (target != INVALID_ENT_REFERENCE && IsValidEntity(target))
    {
        AcceptEntityInput(target, "Kill");
    }

    g_HintForEntity[entity] = -1;
    g_HintTargetForEntity[entity] = -1;
    g_GifterForEntity[entity][0] = '\0';
    g_InfectedForEntity[entity][0] = '\0';
}

void ResetTracking()
{
    for (int i = 1; i < MAX_TRACKED_ENTITIES; i++)
    {
        RemoveGifterLabel(i);
    }
    ClearPending();
}

void ConsumePendingSpawn()
{
    g_PendingCount--;
    if (g_PendingCount <= 0)
    {
        ClearPending();
    }
}

void ClearPending()
{
    g_Pending = false;
    g_PendingCount = 0;
    g_PendingExpires = 0.0;
    g_PendingGifter[0] = '\0';
    g_PendingType[0] = '\0';
}


int FindSpawnExecutor(int preferredClient)
{
    if (preferredClient > 0 && preferredClient <= MaxClients && IsClientInGame(preferredClient))
    {
        return preferredClient;
    }

    // Prefer a living survivor because the auto spawn position is chosen
    // relative to the command's client.
    for (int i = 1; i <= MaxClients; i++)
    {
        if (IsClientInGame(i) && GetClientTeam(i) == 2 && IsPlayerAlive(i))
        {
            return i;
        }
    }

    for (int i = 1; i <= MaxClients; i++)
    {
        if (IsClientInGame(i) && !IsFakeClient(i))
        {
            return i;
        }
    }

    for (int i = 1; i <= MaxClients; i++)
    {
        if (IsClientInGame(i))
        {
            return i;
        }
    }

    return 0;
}

bool IsSupportedType(const char[] infectedType)
{
    return StrEqual(infectedType, "tank", false)
        || StrEqual(infectedType, "hunter", false)
        || StrEqual(infectedType, "smoker", false)
        || StrEqual(infectedType, "charger", false)
        || StrEqual(infectedType, "jockey", false)
        || StrEqual(infectedType, "spitter", false)
        || StrEqual(infectedType, "boomer", false)
        || StrEqual(infectedType, "witch", false);
}

bool ZombieClassMatches(const char[] infectedType, int zombieClass)
{
    if (StrEqual(infectedType, "smoker", false))  return zombieClass == 1;
    if (StrEqual(infectedType, "boomer", false))  return zombieClass == 2;
    if (StrEqual(infectedType, "hunter", false))  return zombieClass == 3;
    if (StrEqual(infectedType, "spitter", false)) return zombieClass == 4;
    if (StrEqual(infectedType, "jockey", false))  return zombieClass == 5;
    if (StrEqual(infectedType, "charger", false)) return zombieClass == 6;
    if (StrEqual(infectedType, "tank", false))    return zombieClass == 8;
    return false;
}

void SanitizeGifterName(const char[] input, char[] output, int maxLength)
{
    int outPos = 0;
    int inputLength = strlen(input);

    for (int i = 0; i < inputLength && outPos < maxLength - 1; i++)
    {
        int c = input[i];
        if (c == '"' || c == '\\' || c == ';' || c == '\n' || c == '\r')
        {
            continue;
        }
        output[outPos++] = c;
    }
    output[outPos] = '\0';
    TrimString(output);
}

int ClampInt(int value, int minimum, int maximum)
{
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}
