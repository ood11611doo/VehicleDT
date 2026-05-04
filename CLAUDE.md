# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

VehicleDT is an Unreal Engine 5.5 vehicle simulation project built on Epic's Vehicle Template. It uses the Chaos physics system for realistic wheeled vehicle behavior.

## Build & Development

This is a UE5 C++ project — there is no `npm`, `make`, or script-based build.

**Open the project:**
- Double-click `VehicleDT.uproject` in Explorer, or launch via Epic Games Launcher
- Or right-click → "Generate Visual Studio project files", then open `VehicleDT.sln`

**Compile C++ changes:**
- Inside Unreal Editor: click the compile button (or `Ctrl+Alt+F11`)
- From Visual Studio / Rider: Build → Build Solution
- Hot reload is available for most changes; full recompile needed for new UProperties/UFunctions

**Default play map:** `/Game/VehicleTemplate/Maps/VehicleAdvExampleMap` (set in `Config/DefaultEngine.ini`)

## Architecture

### Module Dependencies (`Source/VehicleDT/VehicleDT.Build.cs`)
Core UE modules + `EnhancedInput`, `ChaosVehicles`, `PhysicsCore`, `UMG`.

### Class Hierarchy

```
AVehicleDTGameMode          – sets PlayerController class; minimal logic
AVehicleDTPlayerController  – polls vehicle state each frame, owns UVehicleDTUI widget
AVehicleDTPawn              – abstract base: input binding, camera, physics helpers
  ├── AVehicleDTSportsCar   – Ackermann steering, 5-speed, 750 Nm torque, 39 cm wheels
  └── AVehicleDTOffroadCar  – angle-ratio steering, differential, 600 Nm torque, 50 cm wheels
UVehicleDTUI                – C++ base for speed/gear HUD widget; Blueprint subclass renders it

Wheels/
  UVehicleDTWheelFront / Rear        – base wheel configs (steering / handbrake flags)
  UVehicleDTSportsWheelFront / Rear  – high friction, smaller radius
  UVehicleDTOffroadWheelFront / Rear – lower friction, larger radius, higher suspension
```

### Data Flow
1. Raw input → `AVehicleDTPlayerController` (EnhancedInput action mappings)
2. Input callbacks call methods on `AVehicleDTPawn` (steering, throttle, brake, handbrake, camera toggle)
3. Pawn drives `UChaosWheeledVehicleMovementComponent`
4. Each tick, `AVehicleDTPlayerController` reads vehicle speed/gear → updates `UVehicleDTUI`

### Key Design Patterns
- **C++ defines systems; Blueprints configure assets.** Wheel meshes, physics curves, and material assignments live in Blueprint subclasses, not C++.
- **Constructor-driven physics tuning.** `AVehicleDTSportsCar` and `AVehicleDTOffroadCar` configure all physics properties (engine, transmission, wheels, suspension) in their constructors.
- **Two-camera setup.** `AVehicleDTPawn` holds a front (first-person) and rear (third-person) `USpringArmComponent` + `UCameraComponent` pair; `bFrontCameraActive` flag controls which is active.
- **Midair damping.** `AVehicleDTPawn::Tick` applies extra angular damping when the vehicle leaves the ground (`IsInAir()`).

## Plugins (`.uproject`)
- `ChaosVehiclesPlugin` – vehicle physics backbone
- `RawInput` – steering wheel / peripheral support (configured in `Config/DefaultInput.ini`)
- `ModelingToolsEditorMode` – editor mesh editing

## Configuration
| File | Purpose |
|------|---------|
| `Config/DefaultEngine.ini` | Map defaults, physics substep settings |
| `Config/DefaultInput.ini` | Input axis/action bindings, steering wheel RawInput setup |
| `Config/DefaultGame.ini` | Project name / metadata |
