namespace ChaosForge.Shared.Contracts;

public sealed record ChaosEvent
{
    public required Guid Id { get; init; }

    public required ChaosEventType Type { get; init; }

    public required string ViewerName { get; init; }

    public int Count { get; init; } = 1;

    public string? GiftName { get; init; }

    public DateTimeOffset CreatedAt { get; init; } =
        DateTimeOffset.UtcNow;
}