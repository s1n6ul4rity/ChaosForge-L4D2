namespace ChaosForge.Shared.Protocol;

public sealed record ChaosEventContext
{
    public required string Platform { get; init; }

    public string? ViewerId { get; init; }

    public required string ViewerName { get; init; }

    public string? GiftName { get; init; }

    public string? InteractionId { get; init; }

    public string? StreamId { get; init; }
}