# Qura

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.8-0E1128?logo=unrealengine&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-Gameplay%20Code-00599C?logo=cplusplus&logoColor=white)
![Blueprint](https://img.shields.io/badge/Unreal-Blueprints-0E1128?logo=unrealengine&logoColor=white)
![Gameplay Ability System](https://img.shields.io/badge/Gameplay%20Ability%20System-GAS-7B2CBF)
![Enhanced Input](https://img.shields.io/badge/Enhanced%20Input-Input%20Actions-2D6CDF)
![Niagara](https://img.shields.io/badge/Niagara-VFX-FF6B35)
![CommonUI](https://img.shields.io/badge/CommonUI-UMG%20%2F%20MVVM-4B8BBE)
![AI](https://img.shields.io/badge/AI-Behavior%20Tree%20%2F%20EQS-2E8B57)
![DirectX 12](https://img.shields.io/badge/Windows-DirectX%2012-0078D4?logo=windows&logoColor=white)

Qura is an action-RPG gameplay prototype built with Unreal Engine 5.8. The
project combines C++ gameplay systems with Blueprint-authored characters,
abilities, interfaces, AI, levels, and UI. Its core loop is built around
mouse-directed movement, target-based combat, elemental abilities, character
progression, enemy encounters, checkpoints, and save slots.

The project is currently in active development. The editor startup map and
default game map are configured as `Content/Levels/Lvl_Test_1.umap`.

## Used technologies

### Engine and programming

- **Unreal Engine 5.8** provides the runtime, editor, level system, rendering,
  animation, physics, navigation, networking, and asset pipeline.
- **C++** contains reusable gameplay and framework code in the `Qura` Runtime
  module.
- **Unreal Engine Blueprints** contain most content-side configuration and
  authoring, including character variants, ability assets, gameplay effects,
  animation blueprints, AI graphs, widgets, and level logic.
- **Unreal Build Tool** compiles the `Qura` Game and `QuraEditor` targets.

### Gameplay and combat

- **Gameplay Ability System (GAS)** manages abilities, gameplay effects,
  attributes, cooldowns, costs, tags, replication, and ability input.
- **Gameplay Tags** identify abilities, input slots, cooldowns, attributes,
  resistances, debuffs, montage events, gameplay cues, and input blocking
  states.
- **Gameplay Effect Contexts** carry combat data such as source and target
  actors, hit results, knockback, damage type, resistance type, critical hits,
  blocked hits, and debuff information.
- **Niagara** supplies combat, projectile, fire, shock, stun, level-up, cursor,
  and character effects.
- **Motion Warping** is enabled for animation-driven movement and attack
  alignment.

### Input, camera, and movement

- **Enhanced Input** provides input actions and mapping contexts.
- The custom `UAuraInputComponent` maps input actions to gameplay tags so the
  same ability input flow can be reused across characters.
- Mouse cursor tracing provides enemy highlighting and target selection.
- Navigation paths and a spline component provide click-to-move style
  movement and auto-running.
- A spring arm and camera component provide third-person camera control,
  including adjustable zoom.

### UI and presentation

- **UMG** is used for HUDs, health and mana globes, XP bars, spell menus,
  attribute menus, load slots, main menus, floating damage text, and messages.
- **CommonUI** provides shared UI behavior and focus handling.
- **MVVM/ViewModel** classes connect load-screen data and widgets.
- Widget controllers broadcast attribute, ability, XP, cooldown, and gameplay
  message updates from C++ systems to Blueprint widgets.

### AI and navigation

- **AI Controller** classes own enemy behavior execution.
- **Behavior Trees** coordinate target selection, movement, attack decisions,
  melee attacks, ranged attacks, and elemental enemy behavior.
- **Blackboards** store combat targets and state such as stunned status.
- **EQS** searches for suitable ranged attack positions around the player.
- **Navigation System** supplies traversable paths for enemies and player
  auto-running.

### Project plugins and modules

The project enables or depends on the following Unreal systems:

- GameplayAbilities
- EnhancedInput
- StateTreeModule
- AIModule
- UMG
- Niagara
- ModelViewViewModel
- CommonUI
- MotionWarping
- ModelingToolsEditorMode for editor workflows

## Gameplay overview

### Player character

The player is implemented by `AAuraCharacter`, which derives from the shared
`AAuraCharacterBase` class and is represented in content by
`BP_AuraCharacter`. The character owns or accesses the Ability System
Component, Attribute Set, camera, player state, startup abilities, passive
abilities, and progression data.

The controller uses the mouse cursor as the primary interaction point:

1. The cursor performs a visibility trace every tick.
2. Actors implementing `IHighlightInterface` are highlighted while under the
   cursor.
3. Actors implementing `IEnemyInterface` are treated as combat targets.
4. Ability input is forwarded to the Ability System Component through
   gameplay input tags.
5. When no enemy is targeted, the cursor destination is used for movement and
   auto-running.

Input-blocking gameplay tags prevent movement, cursor tracing, or ability input
while an ability, montage, stun, or other gameplay state requires control to be
locked.

### Combat and abilities

Abilities are split between reusable C++ base classes and Blueprint assets.
The project includes:

- Fire Bolt projectile attacks
- Fire Blast area or impact attacks
- Electrocute lightning attacks
- Arcane Shards projectile attacks
- Melee and ranged attacks
- Passive abilities such as Halo of Protection, Life Siphon, Mana Siphon, and
  event-listening abilities
- Summoning abilities
- Enemy melee, ranged, firebolt, and summon abilities

Each ability can be connected to:

- a gameplay ability class,
- an input tag,
- a gameplay effect,
- a mana or resource cost,
- a cooldown tag and cooldown effect,
- montage and combat socket data,
- Niagara and sound effects,
- ability information used by the spell menu.

The custom Ability System Component provides input pressed, held, and released
handling; ability lookup by gameplay tag; startup ability initialization;
ability equipping; spell point spending; and client/server ability updates.

### Damage pipeline

Damage is represented as gameplay effect data and processed by the custom
attribute system. The pipeline can include:

1. The source ability creates damage parameters.
2. The target Ability System Component receives the gameplay effect.
3. The execution calculation evaluates damage, armor, penetration, resistance,
   blocking, and critical-hit values.
4. The Attribute Set consumes incoming damage.
5. Health is reduced and fatal damage triggers the combat death interface.
6. Non-fatal damage can trigger hit reaction and knockback.
7. Successful debuffs apply their duration, frequency, damage, and type.
8. The player controller displays floating damage text with blocked and
   critical-hit state.
9. Fatal damage can generate XP and trigger loot behavior.

Projectile actors use collision and Ability System lookups to identify valid
targets, prevent friendly-fire interactions, apply damage, and spawn impact
effects.

## Attributes and progression

`UAuraAttributeSet` replicates and exposes the following attribute groups:

| Group | Attributes |
| --- | --- |
| Primary | Strength, Intelligence, Resilience, Vigor |
| Secondary | Armor, Armor Penetration, Block Chance, Critical Hit Chance, Critical Hit Damage, Critical Hit Resistance, Health Regeneration, Mana Regeneration, Max Health, Max Mana |
| Resistance | Fire, Lightning, Arcane, Physical |
| Vital | Health, Mana |
| Meta | Incoming Damage, Incoming XP |

`AAuraPlayerState` stores replicated player progression:

- Player level
- Experience points
- Spell points
- Attribute points
- Replication callbacks for UI updates
- Level-up data supplied through `ULevelUpInfo`

The attribute menu spends attribute points on primary attributes. The spell
menu uses ability information and gameplay tags to show ability status, spend
spell points, unlock abilities, and equip abilities into input slots. Ability
statuses include `Locked`, `Eligible`, `Unlocked`, and `Equipped`.

Character class data is centralized in `UCharacterClassInfo`. This allows
players and enemies to receive different default attributes, startup
abilities, common abilities, experience rewards, and damage configuration
without duplicating the underlying character code.

## Enemies and AI

`AAuraEnemy` derives from the same shared character base as the player while
adding enemy-specific behavior:

- AI Controller possession
- Behavior Tree execution
- Combat target storage
- Health bar widget component
- Enemy highlighting
- Hit reaction and stun handling
- Character class and level defaults
- Death and loot events

Enemy content currently includes:

- Demon Warrior
- Demon Ranger
- Goblin Spear
- Goblin Slingshot
- Shaman
- Ghoul assets
- Shroom assets

Enemy behavior uses the nearest-player service, melee and ranged attack tasks,
blackboard state, and EQS-based ranged positioning. Elementalist-style enemies
can use dedicated behavior trees and attack tasks for ranged and elemental
combat.

## Save, load, and world travel

`AAuraGameModeBase` manages maps, save slots, player starts, and in-game
progress. `UAuraGameInstance` carries the active slot and player start data
between level loads.

`ULoadScreenSaveGame` stores:

- Player name and save-slot status
- Current map name and map asset name
- Player start tag
- First-time-load state
- Level, XP, spell points, and attribute points
- Primary attribute values
- Saved ability class, ability tag, status, input slot, type, and level

The load flow is:

1. The load screen creates, reads, or deletes a save slot.
2. The selected map is opened through a soft object reference.
3. The active slot name and index are passed through the Game Instance.
4. The GameMode selects a `PlayerStart` matching the saved tag.
5. The player restores progression and ability data.

`Checkpoint` and `MapEntrance` actors save world progress before a map change.
Map names and map asset names must remain consistent with the soft object map
table configured in the Blueprint GameMode.

## UI architecture

The UI is divided into reusable widgets and C++ widget controllers:

- **Load screen:** save slots, player names, map selection, and slot status
- **Main menu:** entry point for starting or loading a game
- **Overlay:** health, mana, XP, ability slots, cooldowns, and gameplay messages
- **Attribute menu:** attribute rows and point spending
- **Spell menu:** ability tree, ability status, spell points, and equipment
- **Combat feedback:** floating damage text, effect messages, and health bars

The C++ widget controller layer subscribes to delegates from the Ability System
Component, Attribute Set, and Player State. It then broadcasts presentation
data through Blueprint-assignable delegates, keeping gameplay state separate
from widget layout and visual styling.

## Project structure

```text
Qura.uproject
Config/
  DefaultEngine.ini          Maps, GameMode, GameInstance, renderer, and Asset Manager
  DefaultGame.ini            CommonUI and Ability System globals
  DefaultGameplayTags.ini    Ability, attribute, input, cooldown, and debuff tags
  DefaultInput.ini            Unreal input defaults
Source/Qura/
  Public/                    C++ public headers
  Private/                   C++ implementations
  AbilitySystem/             Abilities, effects, attributes, data, and GAS helpers
  Actor/                     Projectiles, fireballs, and effect actors
  Character/                 Shared player and enemy character classes
  AI/                        AI controllers, Behavior Tree tasks, and services
  Game/                      GameMode, GameInstance, and save-game code
  Checkpoint/                Checkpoint and map entrance logic
  Player/                    Player Controller and Player State
  UI/                        HUDs, widgets, ViewModels, and widget controllers
  Interaction/               Combat, player, enemy, and highlight interfaces
Content/
  Blueprints/                Ability, character, AI, Game, checkpoint, and UI assets
  Assets/                    Meshes, animations, materials, sounds, and Niagara assets
  Input/                     Enhanced Input actions and mapping contexts
  Levels/                    Main menu, load menu, test, and gameplay maps
```

## Important C++ classes

| Area | Classes |
| --- | --- |
| Module and assets | `Qura`, `UAuraAssetManager` |
| GAS core | `UAuraAbilitySystemComponent`, `UAuraAbilitySystemGlobals`, `UAuraAttributeSet`, `UAuraAbilitySystemLibrary` |
| Abilities | `UAuraGameplayAbility`, `UAuraDamageGameplayAbility`, `UAuraProjectileSpell`, `UAuraBeamSpell`, `UAuraMeleeAttack`, `UAuraSummonAbility` |
| Characters | `AAuraCharacterBase`, `AAuraCharacter`, `AAuraEnemy` |
| Combat actors | `AAuraProjectile`, `AAuraFireBall`, `AAuraEffectActor` |
| AI | `AAuraAIController`, `UBTService_FindNearestPlayer`, `UBTTask_Attack` |
| Game flow | `AAuraGameModeBase`, `UAuraGameInstance`, `ULoadScreenSaveGame` |
| Player | `AAuraPlayerController`, `AAuraPlayerState` |
| UI | `AAuraHUD`, `LoadScreenHUD`, ViewModels, widget controllers |
| Interfaces | `ICombatInterface`, `IPlayerInterface`, `IEnemyInterface`, `IHighlightInterface` |

## Setup and running

### Requirements

1. Windows
2. Unreal Engine **5.8**
3. Visual Studio or the C++ toolchain installed with Unreal Engine
4. Sufficient disk space for Unreal Engine shader compilation and derived
   asset data

### Open in Unreal Editor

1. Open `Qura.uproject` with Unreal Engine 5.8.
2. Allow Unreal Editor to generate or update project files if requested.
3. Compile the `Qura` module when prompted.
4. Open `Content/Levels/Lvl_Test_1.umap`.
5. Start a Play In Editor session.

The project can also be opened from PowerShell:

```powershell
& "C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe" `
  "C:\Projects\Unreal\GAS_Oura\Qura.uproject"
```

## Input assets and controls

The exact key bindings are configured in:

- `Content/Input/IMC_AuraContext.uasset`
- `Content/Input/DA_InputConfig.uasset`
- `Content/Input/InputActions/*.uasset`

The main input surfaces are:

| Action | Asset |
| --- | --- |
| Movement | `IA_MoveActions` |
| Ability slots | `IA_LMB`, `IA_RMB`, `IA_1`, `IA_2`, `IA_3`, `IA_4` |
| Targeting and movement modifier | `IA_Shift` |
| Camera zoom | `IA_Zoom` |

The final keyboard and mouse mapping is defined by the Enhanced Input Mapping
Context, so the assets should be checked when bindings are changed.

## Development workflow

When adding a new ability, connect all of the following:

1. C++ ability base or Blueprint Gameplay Ability
2. Gameplay tags for ability, input, cooldown, and type
3. Cost and cooldown Gameplay Effects
4. Ability information used by the UI
5. Input action or input slot mapping
6. Animation montage and combat socket data when required
7. Niagara and sound feedback
8. Character class startup or passive ability lists

When adding a new enemy, configure the character Blueprint, class defaults,
attributes, startup abilities, AI Controller, Blackboard, Behavior Tree,
attack tasks, health bar, animation blueprint, and loot behavior together.

When changing maps or checkpoints, verify the soft object map table, map asset
name, save slot data, and `PlayerStartTag` values as one flow. Broken
relationships between these values can cause a save to load into the wrong
map or player start.
