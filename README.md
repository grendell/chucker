# Chucker
![Lead Image](images/lead.jpg)

This program will modify a user-provided IL2CPP DLL of [Wild Blue Skies](https://store.steampowered.com/app/1921490/Wild_Blue_Skies/) to make it possible to unlock the ["Time For a Little Payback"](https://steamcommunity.com/stats/1921490/achievements) achievement.  You'll still need to perform the required action in game to earn the achievement.

This fix only requires 12 bytes of the game code to be modified.  This patch was created due to the [uncertainty](https://discord.com/channels/663955185894424586/1349430737614540870/1550197298729058342) of when or if the Chuhai Labs dev team will be able to publish an update.

## Requirements
* Wild Blue Skies GameAssembly.dll
* [make](https://www.gnu.org/software/make/)
* [clang](https://clang.llvm.org)
* Alternatively, prebuilt binaries and patches are available in [Releases](https://github.com/grendell/chucker/releases) as well as [Action artifacts](https://github.com/grendell/chucker/actions).

## Building
* Simply run `make` from inside the project directory.
* Depending on your operating system, a `bin/*/chucker` executable will be created.

## Running
* Example usage:  `bin\win\chucker.exe GameAssmebly.dll`
* Alternatively, apply the patch using a BPS-compatible patcher, such as [Rom Patcher JS](https://www.marcrobledo.com/RomPatcher.js).

## Output
* A modified DLL which can replace your existing DLL.

## How It Works
* The function that is responsible for unlocking this achievement, `WarDogs.Game.HubAllyEntity.OnTargetDied(Entity entity, Entity killerEntity)`, will early out if `entity.get_Type() != EntityType.Enemy`.  Unfortunately, `EntityType.Enemy` is `2` and `EntityType.Rival` is `2048`, and Wart is a Rival.
* `add esp, 8` is replaced with two `pop ecx` instructions to maintain the stack using fewer bytes.
    * `ecx` can safely be overwritten here because `call ecx` has just returned.
* `cmp  eax, 2` is replaced with `test ax, 0x0802` to include both `Rival` and `Enemy`.
* `jne  0x2E8CDA8` is replaced with `je  0x2E8CDA8` to match the updated logic.

## This project was made possible by...
* [Chuhai Labs](https://chuhailabs.com)
* [IL2CPPDumper](https://github.com/Perfare/Il2CppDumper)
* [Ghidra](https://github.com/nationalsecurityagency/ghidra)

Wild Blue Skies © 2026 Chuhai Labs - VITEI BACKROOM. Published by Good Games Group, Inc. d/b/a Balor Games. Balor Games and the Balor Games logo are trademarks or registered trademarks of Good Games Group, Inc. in the United States and/or other countries.