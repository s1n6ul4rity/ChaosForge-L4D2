namespace ChaosForge.Shared.Contracts;

public sealed record ChaosExecutionResult
{
    public required Guid EventId { get; init; }

    public required bool Success { get; init; }

    public int RequestedCount { get; init; }

    public int ExecutedCount { get; init; }

    public string? Message { get; init; }

    public static ChaosExecutionResult Succeeded(
        ChaosEvent chaosEvent,
        int executedCount,
        string? message = null)
    {
        return new ChaosExecutionResult
        {
            EventId = chaosEvent.Id,
            Success = true,
            RequestedCount = chaosEvent.Count,
            ExecutedCount = executedCount,
            Message = message
        };
    }

    public static ChaosExecutionResult Failed(
        ChaosEvent chaosEvent,
        string message)
    {
        return new ChaosExecutionResult
        {
            EventId = chaosEvent.Id,
            Success = false,
            RequestedCount = chaosEvent.Count,
            ExecutedCount = 0,
            Message = message
        };
    }
}