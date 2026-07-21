namespace ChaosForge.Bridge.Models;

public sealed record TestEventRequest
{
    public Guid? RequestId { get; init; }

    public string ViewerName { get; init; } = string.Empty;

    public string? GiftName { get; init; }

    public int Count { get; init; } = 1;
}