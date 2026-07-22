using ChaosForge.Core.Dispatching;
using ChaosForge.Shared.Contracts;
using ChaosForge.Shared.Interactions;
using Microsoft.Extensions.Logging;

namespace ChaosForge.Core.Interactions;

public sealed class InteractionPipeline : IInteractionPipeline
{
    private readonly IInteractionMapper _mapper;
    private readonly IChaosDispatcher _dispatcher;
    private readonly ILogger<InteractionPipeline> _logger;

    public InteractionPipeline(
        IInteractionMapper mapper,
        IChaosDispatcher dispatcher,
        ILogger<InteractionPipeline> logger)
    {
        _mapper = mapper;
        _dispatcher = dispatcher;
        _logger = logger;
    }

    public async Task<ChaosExecutionResult> ExecuteAsync(
        ViewerInteraction interaction,
        CancellationToken cancellationToken = default)
    {
        ArgumentNullException.ThrowIfNull(interaction);

        _logger.LogInformation(
            "Processing interaction {InteractionId}. Source: {Source}, Type: {InteractionType}, Trigger: {TriggerName}, Viewer: {ViewerName}, Quantity: {Quantity}",
            interaction.Id,
            interaction.Source,
            interaction.Type,
            interaction.TriggerName,
            interaction.ViewerName,
            interaction.Quantity);

        InteractionMappingResult mappingResult =
            await _mapper.MapAsync(
                interaction,
                cancellationToken);

        if (!mappingResult.Success ||
            mappingResult.ChaosEvent is null)
        {
            string error =
                mappingResult.Error
                ?? "The interaction could not be mapped.";

            _logger.LogWarning(
                "Interaction {InteractionId} was rejected: {Error}",
                interaction.Id,
                error);

            return new ChaosExecutionResult
            {
                EventId = interaction.Id,
                Success = false,
                RequestedCount = interaction.Quantity,
                ExecutedCount = 0,
                Message = error
            };
        }

        ChaosEvent chaosEvent =
            mappingResult.ChaosEvent;

        _logger.LogInformation(
            "Interaction {InteractionId} mapped to chaos event {EventType}.",
            interaction.Id,
            chaosEvent.Type);

        return await _dispatcher.DispatchAsync(
        chaosEvent,
        cancellationToken);
    }
}