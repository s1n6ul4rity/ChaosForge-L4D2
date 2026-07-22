namespace ChaosForge.Shared.Interactions;

public sealed record ViewerInteraction
{
    public required Guid Id { get; init; }

    public required string Source { get; init; }

    public required string ExternalId { get; init; }

    public required InteractionType Type { get; init; }

    public required string TriggerName { get; init; }

    public string? ViewerId { get; init; }

    public required string ViewerName { get; init; }

    public int Quantity { get; init; } = 1;

    public IReadOnlyDictionary<string, string> Parameters { get; init; } =
        new Dictionary<string, string>();

    public DateTimeOffset ReceivedAt { get; init; } =
        DateTimeOffset.UtcNow;
}