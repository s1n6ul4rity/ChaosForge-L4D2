using ChaosForge.Core.Dispatching;
using ChaosForge.Core.Handlers;
using ChaosForge.Core.Interactions;
using Microsoft.Extensions.DependencyInjection;

namespace ChaosForge.Core;

public static class DependencyInjection
{
    public static IServiceCollection AddChaosForgeCore(
        this IServiceCollection services)
    {
        ArgumentNullException.ThrowIfNull(services);

        services.AddSingleton<IChaosDispatcher,ChaosDispatcher>();

        services.AddSingleton<IChaosEventHandler,TestEventHandler>();

        services.AddSingleton<IChaosEventHandler,SpawnSpecialInfectedHandler>();

        services.AddSingleton<IChaosEventHandler,SpawnCommonInfectedHandler>();

        services.AddSingleton<IInteractionMapper,CatalogInteractionMapper>();

        services.AddSingleton<IInteractionPipeline,InteractionPipeline>();

        return services;
    }
}