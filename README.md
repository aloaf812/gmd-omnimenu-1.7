# OMNImenu / OmniMenu

Mod Menu for older versions of Geometry Dash (pre-1.9).

Currently being tested on version 1.22.

## Special Thanks
- [**akqanile (Adelfa)**](https://github.com/cierra-kb) - 100 KB fix, font size calculator, lots of help with issues. He is regarded as the god of tradmodding for a reason.
- Eclipse Mod Menu team - many hack implementations
- Nikolyas, Caster, Tabbi - betatesters

## Build Instructions
Note: the minimum Android version is currently KitKat (4.4). Android 14+ devices won't work without solutions like Shizuku.

0. If building for 1.5+, you must change the android ABI to armeabi-v7a in CMakeLists.txt
1. Run build script (TODO instructions for Windows). The libmenu.so file will be in the `build` folder.
2. [Download and run APK Editor Studio](<https://qwertycube.com/apk-editor-studio/download/>)
3. Open Settings > Options or press Ctrl+P, go to Apktool and check "Decompile source code (smali)" if it isn't checked already
4. Load the desired version APK
5. (Optional, but recommended) Change the package name in apktool.yml: the line where it says `renameManifestPackage: null`, change `null` to your desired package name. Note that it **must have a length of 24 characters** (to match `com.robtopx.geometryjump`), no more, no less. Allowed characters are `[a-z0-9.]`. Open `libgame.so` (for pre-1.7) or `libcocos2dcpp.so` (for 1.7-1.8) in `lib/armeabi` (for pre-1.5) or `lib/armeabi-v7a` (for 1.5+) in a hex editor, search for an occurence of `com.robtopx.geometryjump` and replace it with your new package name.
6. In the bottom left corner of APK Editor Studio, click on File System, find the `lib` folder. Then, if you're building for pre-1.5, open the only folder: `armeabi`, otherwise open `armeabi-v7a` (and delete the other folders). Right-click the existing `.so` file and press "Open Resource Directory...", then paste the `libmenu.so` file from the `build` folder.
7. Copy all of the resources from the `resources` folder to the APK's `assets` folder.
8. In the `smali` folder of the APK, find the `GeometryJump.smali` file (usually at `/smali/com/robtopx/geometryjump/`). Find where it says:

```
const-string v0, "game"

invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V
```

Copy and paste these lines right below the original lines and replace `"game"` (or `"cocos2dcpp"`) with `"menu"`, so that it says something along the lines of:

```
.method static constructor <clinit>()V
    .locals 1

    const-string v0, "game"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

    const-string v0, "menu"

    invoke-static {v0}, Ljava/lang/System;->loadLibrary(Ljava/lang/String;)V

    return-void
.end method
```

(Note: this is slightly different for newer versions)

9. In the folder with the mod source, open `smali/GeometryJump.smali`, copy everything from it and paste it at the bottom of the APK's `GeometryJump.smali`. This is required for GDShare to work. If the APK's `GeometryJump.smali` is not located at `com/robtopx/geometryjump`, you should replace the 2 occurences of `com/robtopx/geometryjump/GeometryJump` in the smali code you're pasting in to `new/path/to/GeometryJump`.
10. (Optional, but recommended) Switch back to Resources at the bottom left of APK Editor Studio. Click on values, then strings, then the entry without a flag. Find `<string name="app_name">Geometry Dash</string>` and replace `Geometry Dash` with your desired app name. (Note: the recommended capitalizations are OMNImenu or OmniMenu)
11. Click File > Save APK... or press Ctrl+Alt+S and save the APK.
12. You're good to go!