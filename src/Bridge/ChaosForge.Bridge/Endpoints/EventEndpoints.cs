using ChaosForge.Bridge.Models;
using ChaosForge.Core.Dispatching;
using ChaosForge.Shared.Contracts;

namespace ChaosForge.Bridge.Endpoints;

public static class EventEndpoints
{
    public static IEndpointRouteBuilder MapEventEndpoints(
        this IEndpointRouteBuilder endpoints)
    {
        var group = endpoints.MapGroup("/api/v1/events")
            .WithTags("Events");

        group.MapPost("/test", HandleTestEventAsync);

        return endpoints;
    }

    private static async Task<IResult> HandleTestEventAsync(
        TestEventRequest request,
        IChaosDispatcher dispatcher,
        ILoggerFactory loggerFactory,
        CancellationToken cancellationToken)
    {
        var logger = loggerFactory.CreateLogger(
            "ChaosForge.Bridge.EventEndpoints");

        if (string.IsNullOrWhiteSpace(request.ViewerName))
        {
            return Results.BadRequest(new
            {
                error = "viewerName is required."
            });
        }

        if (request.Count < 1)
        {
            return Results.BadRequest(new
            {
                error = "count must be at least 1."
            });
        }

        var chaosEvent = new ChaosEvent
        {
            Id = request.RequestId ?? Guid.NewGuid(),
            Type = ChaosEventType.TestEvent,
            ViewerName = request.ViewerName.Trim(),
            GiftName = string.IsNullOrWhiteSpace(request.GiftName)
                ? null
                : request.GiftName.Trim(),
            Count = request.Count
        };

        logger.LogInformation(
            "Received chaos event {EventId}. Type: {EventType}; Viewer: {ViewerName}; Gift: {GiftName}; Count: {Count}",
            chaosEvent.Id,
            chaosEvent.Type,
            chaosEvent.ViewerName,
            chaosEvent.GiftName,
            chaosEvent.Count);

        var result = await dispatcher.DispatchAsync(
            chaosEvent,
            cancellationToken);

        logger.LogInformation(
            "Completed chaos event {EventId}. Success: {Success}; Executed: {ExecutedCount}/{RequestedCount}",
            result.EventId,
            result.Success,
            result.ExecutedCount,
            result.RequestedCount);

        return result.Success
            ? Results.Ok(result)
            : Results.BadRequest(result);
    }
}