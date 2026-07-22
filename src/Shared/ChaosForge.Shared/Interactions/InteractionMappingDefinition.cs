using ChaosForge.Shared.Contracts;

namespace ChaosForge.Shared.Interactions;

public sealed class InteractionMappingDefinition
{
    public required string Id { get; init; }

    public bool Enabled { get; init; } = true;

    public required string Source { get; init; }

    public required InteractionType InteractionType { get; init; }

    public required string TriggerName { get; init; }

    public required ChaosEventType EventType { get; init; }

    public int CountMultiplier { get; init; } = 1;

    public Dictionary<string, string> Parameters { get; init; } =
        new(StringComparer.OrdinalIgnoreCase);
}