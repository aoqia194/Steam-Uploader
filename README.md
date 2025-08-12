# Steam Uploader (WIP)
Custom tool to update items on the Steam Workshop. Allows you to independently update specific elements like the item title, its content, the preview etc.

## Uploading
To use the tool, make sure Steam is launched and use with the following method in a command-line interpreter:
```bash
cd path/to/app
./app-{os} [--key value]
```

Below is the list of <span style="color: red">mandatory</span> arguments to pass.
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| AppID | <code>-a <span style="color:#0074D9">appID</span></code> | <code>--appID <span style="color:#0074D9">appID</span></code> | The [App ID](https://pzwiki.net/wiki/App_ID) for your game. <BR><BR> If it is not provided, the code tries to retrieve automatically but the process can fail if the item was set to a visibility other than public. |
| WorkshopID | <code>-w <span style="color:#0074D9">workshopID</span></code> | <code>--workshopID <span style="color:#0074D9">workshopID</span></code> | The [workshop ID](https://pzwiki.net/wiki/Workshop_ID) of your item. |

Below is a list of arguments you can use to update specific elements of your workshop item. At least one needs to be used up to as many you want.
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| Description | <code>-d <span style="color:red">path</span></code> | <code>--description <span style="color:red">path</span></code> | <span style="color:red">Path</span> to a text file which holds the description. <BR><BR> Currently there is not check for the size of the description so make sure it respects the Steam limitations. |
| Preview | <code>-p <span style="color:red">path</span></code> | <code>--preview <span style="color:red">path</span></code> | <span style="color:red">Path</span> to the preview image or gif to upload. Suggested formats include JPG, PNG and GIF and there seems to be no limitations regarding the dimensions of the preview but the file size needs to be lower than 1 MB. |
| Content | <code>-c <span style="color:red">path</span></code> | <code>--content <span style="color:red">path</span></code> | <span style="color:red">Path</span> to the folder of the content to upload. |
| Title | <code>-t <span style="color:red">title</span></code> | <code>--title <span style="color:red">title</span></code> | <span style="color:red">Title</span> of workshop item. |
| Visibility | <code>-v <span style="color:red">int</span></code> | <code>--visibility <span style="color:red">int</span></code> | Visibility of the item on the workshop: <ul> <li><code><span style="color:red">0</span></code> for public visibility (default),</li> <li><code><span style="color:red">1</span></code> for friends-only visibility,</li> <li><code><span style="color:red">2</span></code> for private (hidden) visibility.</li> <li><code><span style="color:red">3</span></code> for unlisted visibility.</li> </ul> |
| Title | <code>-T <span style="color:red">tag1,tag2,...,tagN</span></code> | <code>-tags <span style="color:red">tag1,tag2,...,tagN</span></code> | Used to set the <span style="color:red">tags</span> of the Workshop item. These are predefined by the developers of the game and thus only those should be used. You can find the list of tags in the main Workshop page of the game on the right under "Browse By Tags". Tags need to be separated by `,`. <BR><BR> This list can also be empty by doing `--tags ""` which will remove every tags. |

Below are optional parameters:
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| Patch note | <code>-P <span style="color:green">path</span></code> | <code>-patchNote <span style="color:green">path</span></code> | <span style="color:green">Path</span> to a text file which holds the patch note of the update. This is supposedly only needed if you upload new content. |

## Details
Tool was confirmed working on:
- Linux Mint 22.1 x86_64 (Kernel: 6.8.0-71-generic; Shell: bash 5.2.21)
- Windows 10 Pro 22H2

Built with the [Steamworks SDK v1.62](https://partner.steamgames.com/doc/sdk).

## Contact
The tool was created by me (discord: sirdoggyjvla) for the Project Zomboid modding community. You can find us on the [Unofficial Modding Discord](https://pzwiki.net/wiki/Unofficial_Modding_Discord) of Project Zomboid.