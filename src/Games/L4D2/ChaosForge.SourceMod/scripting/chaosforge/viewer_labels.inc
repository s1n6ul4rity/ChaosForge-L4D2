#if defined _chaosforge_viewer_labels_included
    #endinput
#endif
#define _chaosforge_viewer_labels_included

#define CHAOSFORGE_VIEWER_NAME_SIZE 128
#define CHAOSFORGE_ENTITY_LABEL_SIZE 64

void ChaosViewerLabel_Assign(
    int entity,
    const char[] viewerName,
    const char[] infectedName
)
{
    if (entity <= 0 || !IsValidEntity(entity))
    {
        return;
    }

    char resolvedViewerName[CHAOSFORGE_VIEWER_NAME_SIZE];

    if (viewerName[0] == '\0')
    {
        strcopy(
            resolvedViewerName,
            sizeof(resolvedViewerName),
            "Anonymous"
        );
    }
    else
    {
        strcopy(
            resolvedViewerName,
            sizeof(resolvedViewerName),
            viewerName
        );
    }

    char label[CHAOSFORGE_ENTITY_LABEL_SIZE];

    Format(
        label,
        sizeof(label),
        "%s's %s",
        resolvedViewerName,
        infectedName
    );

    /*
     * Special infected and Tanks are client entities.
     * Renaming them makes viewer ownership available in the game UI,
     * kill feed, console output, and future tracking systems.
     */
    if (entity <= MaxClients)
    {
        if (!IsClientInGame(entity))
        {
            return;
        }

        SetClientName(entity, label);

        PrintToServer(
            "[ChaosForge] Assigned viewer label '%s' to infected client %d.",
            label,
            entity
        );

        return;
    }

    /*
     * Witches are world entities, not clients. A targetname gives us
     * persistent ownership metadata, although it is not yet rendered
     * as floating text.
     */
    DispatchKeyValue(
        entity,
        "targetname",
        label
    );

    PrintToServer(
        "[ChaosForge] Assigned viewer ownership '%s' to world entity %d.",
        label,
        entity
    );
}