# Steam Uploader (WIP)
Custom tool to update items on the Steam Workshop. Allows you to independently update specific elements like the item title, its content, the preview etc.

## Uploading
To use the tool, make sure Steam is launched and use a command-line interpreter (CLI).
1. Open the CLI in the folder. This can be done with two ways:
    - On Windows, in the file explorer top bar with the full path of the folder you're in, you can type `cmd` to open the CLI inside this folder.
    - On Windows 11, in the file explorer, you can right click the background of a folder and select "Open in terminal"
    - On Windows, you can open a terminal by searching for the application `cmd`.
    - Use the following command to go in the folder:
```bash
cd path/to/app
```
For paths with spaces, make sure to put them in between quote marks `"`.
2. Use the application by calling it with the various arguments key-value pairs:
- Windows:
```bash
app-windows-latest -k value # short key version
app-windows-latest --key value # long key version
app-windows-latest --appID appid --workshopID id -p preview # multiple keys
```
- Linux, same thing but instead call the app by doing:
```bash
./app-ubuntu-latest #...
```

## Arguments
Below is the list of <span style="color: red">mandatory</span> arguments to pass.
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| AppID | <code>-a <span style="color:#0074D9">appID</span></code> | <code>--appID <span style="color:#0074D9">appID</span></code> | The [App ID](https://pzwiki.net/wiki/App_ID) for your game. |
| WorkshopID | <code>-w <span style="color:#0074D9">workshopID</span></code> | <code>--workshopID <span style="color:#0074D9">workshopID</span></code> | The [workshop ID](https://pzwiki.net/wiki/Workshop_ID) of your item. This parameter is overwritten by using `--new`. |

Below is a list of arguments you can use to update specific elements of your workshop item. At least one needs to be used up to as many you want.
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| Description | <code>-d <span style="color:red">path</span></code> | <code>--description <span style="color:red">path</span></code> | <span style="color:red">Path</span> to a text file which holds the description. The description uses [BBCode](https://pzwiki.net/wiki/BBCode). |
| Preview | <code>-p <span style="color:red">path</span></code> | <code>--preview <span style="color:red">path</span></code> | <span style="color:red">Path</span> to the preview image or gif to upload. Suggested formats include JPG, PNG and GIF and there seems to be no limitations regarding the dimensions of the preview but the file size needs to be lower than 1 MB. |
| Content | <code>-c <span style="color:red">path</span></code> | <code>--content <span style="color:red">path</span></code> | <span style="color:red">Path</span> to the folder of the content to upload. |
| Title | <code>-t <span style="color:red">title</span></code> | <code>--title <span style="color:red">title</span></code> | <span style="color:red">Title</span> of workshop item. |
| Visibility | <code>-v <span style="color:red">int</span></code> | <code>--visibility <span style="color:red">int</span></code> | Visibility of the item on the workshop: <ul> <li><code><span style="color:red">0</span></code> for public visibility (default),</li> <li><code><span style="color:red">1</span></code> for friends-only visibility,</li> <li><code><span style="color:red">2</span></code> for private (hidden) visibility.</li> <li><code><span style="color:red">3</span></code> for unlisted visibility.</li> </ul> |
| Tags | <code>-T <span style="color:red">tag1,tag2,...,tagN</span></code> | <code>--tags <span style="color:red">tag1,tag2,...,tagN</span></code> | Used to set the <span style="color:red">tags</span> of the Workshop item. These are predefined by the developers of the game and thus only those should be used. You can find the list of tags in the main Workshop page of the game on the right under "Browse By Tags". Tags need to be separated by `,`. <BR><BR> This list can also be empty by doing `--tags ""` which will remove every tags. |
| New workshop item | <code>-n</code> | <code>--new</code> | Creates a <span style="color:red">new</span> Workshop item and thus a new Workshop ID which will be used to update the other fields (description, preview etc). <BR><BR> This parameter can be called alone which will create an empty Workshop item without anything on the Workshop page which is hidden by default. |

Below are optional parameters:
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| Patch note | <code>-P <span style="color:green">path</span></code> | <code>--patchNote <span style="color:green">path</span></code> | <span style="color:green">Path</span> to a text file which holds the patch note of the update. This is supposedly only needed if you upload new content. The patch note uses [BBCode](https://pzwiki.net/wiki/BBCode). |

## Examples
For these examples, consider the Workshop ID `123456789`, a user name `user` on the computer and using the Project Zomboid's [mod structure](https://pzwiki.net/wiki/Mod_structure) (appID: 108600).

### Updating the content
To update the content of your mod while on Windows:
```bash
app-windows-latest -a 108600 -w 123456789 -c C:\Users\user\Zomboid\Workshop\ExampleMod\Contents
```
For Linux
```bash
./app-windows-latest -a 108600 -w 123456789 -c /home/user/Zomboid/Workshop/ExampleMod/Contents
```

### Updating the preview image
To update the preview of your mod with a gif (or other image formats):
```bash
./app-windows-latest -a 108600 -w 123456789 -p /home/user/Zomboid/Workshop/ExampleMod/preview.gif
```

### Updating the description
To update the description of your mod, you need to create a text file which holds the description in [BBCode](https://pzwiki.net/wiki/BBCode):
```bash
./app-windows-latest -a 108600 -w 123456789 -d "/home/user/Zomboid/Workshop/Path with space for example/description.txt"
```

### Updating the tags
To set the tags to "Audio" and "Build 42" (Project Zomboid's specific Workshop tags):
```bash
./app-windows-latest -a 108600 -w 123456789 -T "Animals,Build 42"
```

### Updating multiple things at once
You can combine multiple arguments at once to update different things in a single command:
```bash
./app-windows-latest -a 108600 -w 123456789 -c /home/user/Zomboid/Workshop/ExampleMod/Contents -p /home/user/Zomboid/Workshop/ExampleMod/preview.gif -T "Animals,Build 42"
```

## Details
Tool was confirmed working on:
- Linux Mint 22.1 x86_64 (Kernel: 6.8.0-71-generic; Shell: bash 5.2.21)
- Windows 10 Pro 22H2
- Windows 11 24H2

Built with the [Steamworks SDK v1.62](https://partner.steamgames.com/doc/sdk).

## Contact
The tool was created by me (discord: sirdoggyjvla) for the Project Zomboid modding community. You can find us on the [Unofficial Modding Discord](https://pzwiki.net/wiki/Unofficial_Modding_Discord) of Project Zomboid.