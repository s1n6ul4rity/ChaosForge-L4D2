using ChaosForge.Shared.Contracts;

namespace ChaosForge.Core.Interactions;

public sealed record InteractionMappingResult
{
    public required bool Success { get; init; }

    public ChaosEvent? ChaosEvent { get; init; }

    public string? Error { get; init; }

    public static InteractionMappingResult Mapped(
        ChaosEvent chaosEvent)
    {
        ArgumentNullException.ThrowIfNull(chaosEvent);

        return new InteractionMappingResult
        {
            Success = true,
            ChaosEvent = chaosEvent
        };
    }

    public static InteractionMappingResult Failed(
        string error)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(error);

        return new InteractionMappingResult
        {
            Success = false,
            Error = error
        };
    }
}