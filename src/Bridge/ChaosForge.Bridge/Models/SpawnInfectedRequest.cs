namespace ChaosForge.Bridge.Models;

public sealed record SpawnInfectedRequest
{
    public required string Infected { get; init; }

    public int Count { get; init; } = 1;

    public string ViewerName { get; init; } = "OpenAI";

    public string? GiftName { get; init; }
}