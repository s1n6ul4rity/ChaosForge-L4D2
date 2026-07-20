# Tikfinity Chaos Engine (TCE)

> A powerful, customizable TikTok LIVE integration for Left 4 Dead 2.

Transform your TikTok LIVE gifts, likes, follows, and subscriptions into interactive gameplay events for Left 4 Dead 2.

---

## Features

### 🎁 Gift Events
- Spawn Special Infected
- Spawn Tanks & Witches
- Spawn Common Hordes
- Trigger Panic Events
- Give Weapons
- Heal Survivors
- Apply Buffs & Debuffs
- Play Custom Sounds
- Display On-Screen Alerts

### ❤️ Like Events
- Trigger actions every X likes
- Configurable thresholds
- Multiple actions per threshold

### 👥 Follow Events
- Spawn enemies
- Give rewards
- Random events

### ⭐ Subscription Events
- Special Chaos Events
- Boss Waves
- Unique Buffs

---

## Planned Features

- Visual Gift Editor
- Drag & Drop Rule Builder
- Queue System
- Cooldown System
- OBS Overlay
- Stream Statistics
- Profile System
- Random Events
- Action Chains
- Import / Export Configurations
- Plugin System
- Multi-language Support

---

## Example Gift Configuration

```json
{
  "Rose": {
    "cooldown": 30,
    "actions": [
      {
        "type": "spawn",
        "infected": "tank",
        "count": 1
      },
      {
        "type": "alert",
        "text": "{user} has spawned a Tank!"
      }
    ]
  }
}
```

---

## Architecture

```
TikTok LIVE
        │
        ▼
   TikFinity
        │
        ▼
 Tikfinity Chaos Bridge
        │
        ▼
 SourceMod Plugin
        │
        ▼
 Left 4 Dead 2
```

---

## Project Structure

```
TikfinityChaosEngine/

Bridge/
    Windows Application

GamePlugin/
    SourceMod Plugin

Shared/
    Shared Models

Config/
    Configuration Files

Docs/
    Documentation

Installer/
    Installer

Assets/
    Images & Icons
```

---

## Requirements

- Left 4 Dead 2 (Steam)
- MetaMod:Source
- SourceMod
- TikFinity v1.70+
- Windows 10/11

---

## Roadmap

### v0.1
- [ ] SourceMod communication
- [ ] Spawn Special Infected
- [ ] Basic Gift Mapping

### v0.2
- [ ] TikFinity Integration
- [ ] Queue System
- [ ] Cooldowns

### v0.3
- [ ] Buffs
- [ ] Debuffs
- [ ] Weapon Rewards

### v0.4
- [ ] GUI Editor
- [ ] Profiles
- [ ] Import / Export

### v1.0
- [ ] OBS Overlay
- [ ] Statistics
- [ ] Plugin Support
- [ ] Public Release

---

## License

MIT License

---

## Contributing

Contributions, suggestions, and bug reports are welcome!

If you'd like to help improve Tikfinity Chaos Engine, feel free to submit issues or pull requests.

---

## Disclaimer

Tikfinity Chaos Engine is an independent open-source project and is not affiliated with Valve, TikTok, or TikFinity.
