# STweaks

Server-side LeviLamina tweaks for bug fixes and configurable gameplay rules.

## Build

```powershell
xmake f -y -p windows -a x64 -m release
xmake
```

The packaged plugin is written to `bin/STweaks`.

## Configuration

On first plugin load, STweaks creates `config.json` in its LeviLamina configuration directory. Restart the server or
reload the plugin after editing it.

The configuration supports:

- Feature switches for chunk, bundle, silverfish, portal, wither, explosion, and loot behavior.
- Portal-mob `allowlist` and `denylist` policies using entity type IDs.
- Multiple prioritized 3D AABB rules for wither summoning and explosions.
- Per-explosion-rule block breaking, fire, and entity damage controls.
- Configurable bonus loot rules with exact or ECMAScript regular-expression block matchers.
- Loot conditions for player mining and Silk Touch, plus independent or single-selection permille drop chances.

The first matching AABB rule wins. Loot regular expressions are compiled while loading the configuration; invalid rules
are ignored and reported in the server log.

## License

CC0-1.0, based on the LeviLamina mod template.
