# YimMenu
A mod menu for Grand Theft Auto V Legacy

## How to use
1. Download the latest version of FSL from [here](https://www.unknowncheats.me/forum/grand-theft-auto-v/616977-fsl-local-gtao-saves.html) and place WINMM.dll in your GTA V directory
2. Download YimMenu from [GitHub Releases](https://github.com/Mr-X-GTA/YimMenu/releases/tag/nightly)
3. Download an injector, such as [Xenos](https://www.unknowncheats.me/forum/general-programming-and-reversing/124013-xenos-injector-v2-3-2-a.html)
4. Open Rockstar Launcher, select Grand Theft Auto V Legacy, go to settings, and disable BattlEye. If you are using Steam or Epic Games, you may have to pass the -nobattleye command line parameter as well
5. Launch GTA V, then use your injector to inject YimMenu.dll at the main menu

## 已知问题 | Known Issues

在禁用 BattlEye 的情况下，使用 YimMenu 运行传承版（Grand Theft Auto V Legacy）时，若尝试进入“公开战局”或在“仅限邀请战局”中邀请好友，可能会导致游戏程序崩溃。
When running *Grand Theft Auto V Legacy* with YimMenu and BattlEye disabled, the game may crash upon joining a Public Session or when inviting friends into an Invite-Only Session.

## 解决方案 | Workaround

请启用 **FSL** 功能以解决此问题。
**具体操作：** 将 `WINMM.dll` 文件放置于 *Grand Theft Auto V Legacy* 的游戏安装根目录下。
To resolve this instability, please enable **FSL**.
**Instructions:** Place the `WINMM.dll` file into the root directory of your *Grand Theft Auto V Legacy* installation.
