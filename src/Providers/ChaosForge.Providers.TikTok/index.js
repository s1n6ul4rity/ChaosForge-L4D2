const {
    TikTokLiveConnection
} = require("tiktok-live-connector");

const creatorUsername = process.env.TIKTOK_USERNAME;

const chaosForgeUrl =
    process.env.CHAOSFORGE_URL ??
    "http://127.0.0.1:6721/api/v1/events";

const recentlyProcessedGifts = new Map();

const giftDeduplicationWindowMs = 1500;

if (!creatorUsername) {
    console.error(
        "TIKTOK_USERNAME environment variable is required."
    );

    process.exit(1);
}

const connection = new TikTokLiveConnection(
    creatorUsername,
    {}
);

connection.on("gift", async (event) => {
    const giftId =
        event.giftId ??
        event.giftDetails?.giftId ??
        "unknown";

    const viewerId =
        event.user?.uniqueId ??
        event.user?.userId ??
        event.user?.nickname ??
        "anonymous";

    const repeatCount = Number(event.repeatCount);

    const count =
        Number.isFinite(repeatCount)
            ? Math.max(repeatCount, 1)
            : 1;

    const giftType =
        event.giftType ??
        event.giftDetails?.giftType;

    // For streakable gifts, process only the final callback.
    if (giftType === 1 && !event.repeatEnd) {
        return;
    }

    const deduplicationKey = [
        viewerId,
        giftId,
        count,
        event.repeatEnd ?? false
    ].join(":");

    const now = Date.now();
    const lastProcessedAt =
        recentlyProcessedGifts.get(deduplicationKey);

    if (
        lastProcessedAt !== undefined &&
        now - lastProcessedAt < giftDeduplicationWindowMs
    ) {
        console.warn(
            `Ignored duplicate gift callback: ${deduplicationKey}`
        );

        return;
    }

    recentlyProcessedGifts.set(
        deduplicationKey,
        now
    );

    // Remove old entries so the map cannot grow forever.
    for (const [key, processedAt] of recentlyProcessedGifts) {
        if (now - processedAt > giftDeduplicationWindowMs) {
            recentlyProcessedGifts.delete(key);
        }
    }

    const payload = {
        type: "SpawnSpecialInfected",

        viewerName:
            event.user?.uniqueId ??
            event.user?.nickname ??
            "Anonymous",

        giftName:
            event.giftDetails?.giftName ??
            event.extendedGiftInfo?.name ??
            `Gift-${giftId}`,

        count,

        infected: "hunter"
    };

    console.log("Gift received:", payload);

    try {
        const response = await fetch(chaosForgeUrl, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
        });

        const body = await response.text();

        if (!response.ok) {
            console.error(
                `ChaosForge rejected the gift: ` +
                `${response.status} ${body}`
            );

            return;
        }

        console.log(
            `${payload.viewerName} sent ` +
            `${payload.giftName} x${payload.count}.`
        );

        console.log(
            `ChaosForge response: ${body}`
        );
    } catch (error) {
        console.error(
            "Failed to send the gift to ChaosForge:",
            error
        );
    }
});

connection.on("connected", (state) => {
    console.log(
        `Connected to TikTok room ${state.roomId}.`
    );
});

connection.on("disconnected", () => {
    console.warn(
        "Disconnected from TikTok LIVE."
    );
});

connection.on("error", (error) => {
    console.error(
        "TikTok connection error:",
        error
    );
});

connection.connect()
    .catch((error) => {
        console.error(
            "Could not connect to TikTok LIVE:",
            error
        );

        process.exitCode = 1;
    });