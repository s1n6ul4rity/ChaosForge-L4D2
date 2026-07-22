using ChaosForge.Shared.Contracts;

namespace ChaosForge.Shared.Protocol;

public sealed record ChaosEventPayload
{
    public int Count { get; init; } = 1;

    public SpecialInfectedType? Infected { get; init; }
}