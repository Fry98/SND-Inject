# SND Inject for Doom Eternal
This is a modding tool for Doom Eternal and it enables audio modifications to the game. All the game audio is stored in a bunch of archives in the **.snd** format *(eg. sfx.snd, music.snd, vo_English(US).snd,...)*. Inside those, there are embedded audio files in Wwise's **WEM** format. This tool allows you to inject your own custom WEM file into the archive and use it as a replacement for a pre-existing one. That way you can replace any in-game music for your own.

## Usage
To use this program you need to know the internal ID of the audio file you want to replace. These IDs are located at the top of the SND archive. To start this program, run:
```
SND_Inject.exe [path_to_SND_archive] [SFX_ID] [path_to_custom_WEM_file]
```
Changes to the SND file cannot be reversed with this software so I suggest you create a backup before injecting your WEM files.
