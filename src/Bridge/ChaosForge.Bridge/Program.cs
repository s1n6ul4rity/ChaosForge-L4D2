using ChaosForge.Bridge.Configuration;
using ChaosForge.Bridge.Endpoints;
using ChaosForge.Core;
using System.Net.WebSockets;
using System.Text;
using ChaosForge.Bridge.Infrastructure.WebSockets;

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

builder.Services.AddSingleton<ChaosForgeWebSocketConnection>();

var app = builder.Build();

app.UseWebSockets();

app.Map("/chaosforge", async context =>
{
    if (!context.WebSockets.IsWebSocketRequest)
    {
        context.Response.StatusCode = StatusCodes.Status400BadRequest;
        await context.Response.WriteAsync(
            "A WebSocket connection is required.");

        return;
    }

    var connection =
        context.RequestServices
            .GetRequiredService<ChaosForgeWebSocketConnection>();

    using WebSocket socket =
        await context.WebSockets.AcceptWebSocketAsync();

    connection.Attach(socket);

    Console.WriteLine("[Bridge] SourceMod connected.");

    byte[] buffer = new byte[4096];

    try
    {
        while (socket.State == WebSocketState.Open)
        {
            WebSocketReceiveResult result =
                await socket.ReceiveAsync(
                    buffer,
                    context.RequestAborted);

            if (result.MessageType == WebSocketMessageType.Close)
            {
                break;
            }

            string message = Encoding.UTF8.GetString(
                buffer,
                0,
                result.Count);

            Console.WriteLine($"[Plugin] {message}");

            if (message.Equals(
                "HELLO CHAOSFORGE_L4D2",
                StringComparison.OrdinalIgnoreCase))
            {
                await connection.SendAsync(
                    "PING",
                    context.RequestAborted);

                Console.WriteLine("[Bridge] Sent PING");
            }

            if (message.Equals(
                "PONG",
                StringComparison.OrdinalIgnoreCase))
            {
                Console.WriteLine(
                    "[Bridge] Plugin replied with PONG");
            }
        }
    }
    catch (OperationCanceledException)
    {
        // Connection ended because the request or application stopped.
    }
    catch (WebSocketException exception)
    {
        Console.WriteLine(
            $"[Bridge] WebSocket error: {exception.Message}");
    }
    finally
    {
        connection.Detach(socket);
        Console.WriteLine("[Bridge] SourceMod disconnected.");
    }
});

app.MapPost(
    "/api/v1/events/spawn-hunter",
    async (
        ChaosForgeWebSocketConnection connection,
        CancellationToken cancellationToken) =>
    {
        if (!connection.IsConnected)
        {
            return Results.Problem(
                title: "SourceMod is not connected",
                detail:
                    "Start Left 4 Dead 2 and ensure the ChaosForge plugin is loaded.",
                statusCode: StatusCodes.Status503ServiceUnavailable);
        }

        await connection.SendAsync(
            "SPAWN_HUNTER",
            cancellationToken);

        Console.WriteLine(
            "[Bridge] HTTP request sent SPAWN_HUNTER");

        return Results.Accepted(
            value: new
            {
                eventType = "SpawnHunter",
                status = "Dispatched"
            });
    });

app.MapGet(
    "/api/v1/game/status",
    (ChaosForgeWebSocketConnection connection) =>
        Results.Ok(new
        {
            connected = connection.IsConnected
        }));

app.Run();