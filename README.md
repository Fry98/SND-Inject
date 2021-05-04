# SND Inject for Doom Eternal
This is a modding tool for Doom Eternal and it enables audio modifications to the game. All the game audio is stored in a bunch of archives in the **.snd** format *(eg. sfx.snd, music.snd, vo_English(US).snd,...)*. Inside those, there are embedded audio files in either Wwise's **WEM** format *(used for music)* or **Opus** *(used for sound effects)*. This tool allows you to inject your own custom WEM and Opus files into the archive and use them as a replacement for pre-existing ones. That way you can replace any in-game sound for your own.

## Usage
To use this program you need to know the internal ID of the audio file you want to replace. These IDs are located at the top of the SND archive. To start this program, run:
```
SND_Inject.exe [path_to_SND_archive] [SFX_ID] [path_to_your_audio_file]
```
Supported extensions for audio files are **.wem**, **.opus** and **.ogg** *(which is an alternate extension for Opus files)*. For Opus files to work, you also need to have the file **opusdec.exe** *(Opus Decoder)* in the same folder as the executable of **SND Inject**.

Changes to the SND file cannot be reversed with this software so I suggest you create a backup before injecting your WEM files.
