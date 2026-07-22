using ChaosForge.Shared.Contracts;

namespace ChaosForge.Shared.Protocol;

public sealed record ChaosEventMessage
{
    public int ProtocolVersion { get; init; } =
        ChaosProtocol.CurrentVersion;

    public string MessageType { get; init; } =
        ChaosProtocol.MessageTypes.ChaosEvent;

    public required Guid EventId { get; init; }

    public required ChaosEventType EventType { get; init; }

    public required ChaosEventContext Context { get; init; }

    public required ChaosEventPayload Payload { get; init; }

    public required DateTimeOffset CreatedAt { get; init; }

    public static ChaosEventMessage FromChaosEvent(
        ChaosEvent chaosEvent)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        return new ChaosEventMessage
        {
            EventId = chaosEvent.Id,
            EventType = chaosEvent.Type,
            CreatedAt = chaosEvent.CreatedAt,

            Context = new ChaosEventContext
            {
                Platform = ChaosProtocol.Platforms.Development,
                ViewerName = chaosEvent.ViewerName,
                GiftName = chaosEvent.GiftName
            },

            Payload = new ChaosEventPayload
            {
                Count = chaosEvent.Count,
                Infected = chaosEvent.Infected
            }
        };
    }
}