using ChaosForge.Core.Dispatching;
using ChaosForge.Core.Features.TestEvent;
using ChaosForge.Core.Handlers;
using Microsoft.Extensions.DependencyInjection;

namespace ChaosForge.Core;

public static class DependencyInjection
{
    public static IServiceCollection AddChaosForgeCore(
        this IServiceCollection services)
    {
        ArgumentNullException.ThrowIfNull(services);

        services.AddSingleton<IChaosDispatcher, ChaosDispatcher>();

        services.AddSingleton<IChaosEventHandler, TestEventHandler>();

        return services;
    }
}