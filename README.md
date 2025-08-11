# Steam Uploader (WIP)
Custom tool to update items on the Steam Workshop. Allows you to independently update specific elements like the item title, its content, the preview etc.

## Uploading
To use the tool, make sure Steam is launched and use:
```bash
cd <path/to/app>
LD_LIBRARY_PATH=./SteamAPI/linux64 ./app [--key value]
```
Below is the list of <span style="color: red">mandatory</span> arguments to pass.

| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| AppID | <code>-a <span style="color:#0074D9">appID</span></code> | <code>--appID <span style="color:#0074D9">appID</span></code> | The [App ID](https://pzwiki.net/wiki/App_ID) for your game. <BR><BR> If it is not provided, the code tries to retrieve automatically but the process can fail if the item was set to a visibility other than public. |
| WorkshopID | <code>-w <span style="color:#0074D9">workshopID</span></code> | <code>--workshopID <span style="color:#0074D9">workshopID</span></code> | The [workshop ID](https://pzwiki.net/wiki/Workshop_ID) of your item. |

Below is a list of arguments you can use to update specific elements of your workshop item.
| Name | Arg (long) | Arg (short) | Description |
|--|--|--|--|
| Description | <code>-d <span style="color:red">path</span></code> | <code>--description <span style="color:red">path</span></code> | <span style="color:red">Path</span> to a text file which holds the description. <BR><BR> Currently there is not check for the size of the description so make sure it respects the Steam limitations. |
| Preview | <code>-p <span style="color:red">path</span></code> | <code>--preview <span style="color:red">path</span></code> | <span style="color:red">Path</span> to the preview image or gif to upload. |
| Content | <code>-c <span style="color:red">path</span></code> | <code>--content <span style="color:red">path</span></code> | <span style="color:red">Path</span> to the folder of the content to upload. |
| Title | <code>-t <span style="color:red">title</span></code> | <code>--title <span style="color:red">title</span></code> | <span style="color:red">Title</span> of workshop item. |
| Visibility | <code>-v <span style="color:red">int</span></code> | <code>--visibility <span style="color:red">int</span></code> | Visibility of the item on the workshop: <ul> <li><code><span style="color:red">0</span></code> for public visibility (default),</li> <li><code><span style="color:red">1</span></code> for friends-only visibility,</li> <li><code><span style="color:red">2</span></code> for private (hidden) visibility.</li> <li><code><span style="color:red">3</span></code> for unlisted visibility.</li> </ul> |

`-V` or `--verbose` also exists but is unused.