using ChaosForge.Shared.Protocol;

namespace ChaosForge.Shared.Contracts;

public sealed record ChaosEventAcknowledgement
{
    public required int ProtocolVersion { get; init; }

    public required string MessageType { get; init; }

    public required Guid EventId { get; init; }

    public required bool Success { get; init; }

    public int RequestedCount { get; init; }

    public int ExecutedCount { get; init; }

    public string? Message { get; init; }

    public bool IsSupported =>
        ProtocolVersion == ChaosProtocol.CurrentVersion
        && string.Equals(
            MessageType,
            ChaosProtocol.MessageTypes.EventResult,
            StringComparison.Ordinal);
}