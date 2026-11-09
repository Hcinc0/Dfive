# D-Five fork configuration

This source tree is configured for the D-Five platform operated under `diola.ir`.

## Product contract

- Display brand: `D-Five`
- Executable/product identifier: `DFive`
- Installation root: `%LocalAppData%\DFive`
- Application directory: `DFive.app`
- Configuration file: `DFive.ini`
- Link protocol: `dfive://`
- Client updater: `https://cdn.diola.ir/updates`
- Platform API: `https://api.diola.ir`
- Server list ingress: `https://api.diola.ir/v1/server-list/ingress`
- Game cache mirrors: `https://cdn.diola.ir/mirrors/*`

## Server configuration

DFiveServer performs an additional entitlement check using:

```cfg
dfive_licenseKey "dfive_..."
```

This check does not replace the upstream platform entitlement or GTA ownership checks.

## Build prerequisite warning

The source directory was supplied without `.git` metadata and without all Git submodules. Before a production build, restore the submodules listed in `.gitmodules` (including the UI and binary dependencies), or create a clean Git clone and port these changes onto it.

## Deployment

The API/CDN deployment bundle, update publisher, mirror tool and Persian deployment guide are delivered separately as `dfive-deployment`.

