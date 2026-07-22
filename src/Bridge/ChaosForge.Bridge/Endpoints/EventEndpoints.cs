using ChaosForge.Core.Interactions;
using ChaosForge.Shared.Contracts;
using ChaosForge.Shared.Interactions;

namespace ChaosForge.Bridge.Endpoints;

public static class EventEndpoints
{
    public static IEndpointRouteBuilder MapEventEndpoints(
        this IEndpointRouteBuilder endpoints)
    {
        ArgumentNullException.ThrowIfNull(endpoints);

        endpoints.MapPost(
            "/api/v1/events",
            ProcessInteractionAsync);

        return endpoints;
    }

    private static async Task<IResult> ProcessInteractionAsync(
        ChaosEventRequest request,
        IInteractionPipeline pipeline,
        CancellationToken cancellationToken)
    {
        if (string.IsNullOrWhiteSpace(request.Type))
        {
            return Results.BadRequest(new
            {
                error = "Event type is required."
            });
        }

        Guid interactionId = Guid.NewGuid();

        var parameters =
            new Dictionary<string, string>(
                StringComparer.OrdinalIgnoreCase);

        if (!string.IsNullOrWhiteSpace(request.Infected))
        {
            parameters["infected"] =
                request.Infected.Trim();
        }

        if (!string.IsNullOrWhiteSpace(request.GiftName))
        {
            parameters["giftName"] =
                request.GiftName.Trim();
        }

        var interaction = new ViewerInteraction
        {
            Id = interactionId,
            ExternalId = interactionId.ToString(),
            Source = "Development",
            Type = InteractionType.Command,
            TriggerName = request.Type.Trim(),

            ViewerName = string.IsNullOrWhiteSpace(
                request.ViewerName)
                    ? "Anonymous"
                    : request.ViewerName.Trim(),

            Quantity = Math.Clamp(
                request.Count,
                1,
                100),

            Parameters = parameters
        };

        ChaosExecutionResult result =
            await pipeline.ExecuteAsync(
                interaction,
                cancellationToken);

        if (!result.Success)
        {
            return Results.BadRequest(result);
        }

        return Results.Accepted(
            value: result);
    }
}

public sealed record ChaosEventRequest
{
    public required string Type { get; init; }

    public string? ViewerName { get; init; }

    public string? GiftName { get; init; }

    public int Count { get; init; } = 1;

    public string? Infected { get; init; }
}