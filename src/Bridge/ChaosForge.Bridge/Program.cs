using ChaosForge.Bridge.Configuration;
using ChaosForge.Bridge.Endpoints;
using ChaosForge.Core;

var builder = WebApplication.CreateBuilder(args);

builder.Services
    .AddOptions<BridgeSettings>()
    .Bind(
        builder.Configuration.GetSection(
            BridgeSettings.SectionName))
    .Validate(
        settings => settings.Port is > 0 and <= 65535,
        "Bridge port must be between 1 and 65535.")
    .ValidateOnStart();

builder.Services.AddChaosForgeCore();

var bridgeSettings = builder.Configuration
    .GetSection(BridgeSettings.SectionName)
    .Get<BridgeSettings>()
    ?? throw new InvalidOperationException(
        "The Bridge configuration section is missing.");

builder.WebHost.UseUrls(
    $"http://{bridgeSettings.Host}:{bridgeSettings.Port}");

var app = builder.Build();

app.MapHealthEndpoints();
app.MapEventEndpoints();

app.Run();