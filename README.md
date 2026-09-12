# Custom Bugcheck Code
> [!CAUTION]
> Before even doing any of this, run Command Prompt as Administrator and run `bcdedit /set testsigning on`. **You can always change it back to off by changing "on" to "off". Remember to disable Secure Boot too if the driver still won't load after enabling test mode.**
*Also, remember to reboot to save changes.*
<br>
<i>You should be able to see "Test Mode" alongside your Windows version and type if it's active.</i>
<br>
<img width="349" height="186" alt="image" src="https://github.com/user-attachments/assets/8a70dcad-bfbf-4f9e-a44c-83f5a82dd1a0" />
<br>
<br>

I've found a way to make your own AND call other codes. The simple way is to use a driver (driver.c).
By changing "#define MY_BUGCHECK_CODE   0x0" in driver.c, you can call any 8-digit hexadecimal code (0-9, A-F ONLY) you'd like. 
Windows will recognize any code and trigger a bugcheck (BSOD).

A little tip, Windows WILL ONLY recognise codes in its list and will show, for example MEMORY MANAGEMENT, or a less known (or discontinued) one, PFN SHARE COUNT. You can still call that. If a code is not on its list, it will STILL show the bugcheck screen but it'll just tell you the hexadecimal code.

**How it works**: When the trigger sends a custom IOCTL to the driver (kernel-mode), the driver calls KeBugCheckEx() with your custom bugcheck code. Easy, right?

# How to make and trigger it
*You WILL need WDK and Visual Studio Community. Download driver.c and trigger.c.*
<br>
<br>
Name your project anything, make sure to use project type **"Kernel Mode Driver, Empty (KMDF)"** so we get a kernel-mode driver. User-mode drivers will NOT work as we're telling Windows to crash the entire operating system. *If you haven't already, save your work.*
<br>
<br>
In your project, navigate to Solution Explorer in your project. Go into your project and in Driver Files, right click on the folder and click Add > Existing Item. File Explorer will show up. If it's not already at Downloads, on your sidebar click Downloads. Click driver.c and click "Add".
<br>
<br>
If you'd like, you CAN change #define MY_BUGCHECK_CODE 0x0 to ANYTHING you like (0-9, A-F ONLY). You can keep 0x0, nobody's forcing you too.
<br>
<br>
Now we're making the trigger. Right click "Solution '{your project name}' (1 of 1 projects)" and click "Add". You should see "New Project...". Click on it, you should see "Console App". Click on Console App. Name your trigger anything, just be able to memorise it. Now, after creation the solution should say "Solution '{your project name}' (2 of 2 projects)".
<br>
<br>
Now, right click on your newly added project under your solution. In Source Files, delete the demo (or starting) script. Right click on Source Files. Click Add > "Existing Item...". File Explorer will show up. If it's not already at Downloads, on your sidebar click Downloads. Click trigger.c and click "Add".
<br>
<br>
You're now done! Don't touch anything else other than the bugcheck code (other if you're smart, go ahead). Right click on "Solution '{your name}' (2 of 2 projects)". Click "Build Solution". Wait until both succeeds. Now that it's built, it's time to **trigger it!**
<br>
<br>
## Triggering
Run Command Prompt as Admin. Run `"sc create CustomKE type= kernel start= auto binPath= C:\Users\[your name]\source\repos\[your project name]\x64\Debug\yourprojectname.sys"`
<br>
*Don't want it to start automatically on boot? Remove "start= auto" from the command."*
<br>
<br>
It should say `"[SC] CreateService SUCCESS"`. Now, start the service by running `"sc start CustomKE"`.
<br>
After running that, it should state ` STATE              : 4  RUNNING`. Doesn't? Follow the steps correctly.
<br>
<br>
And to trigger the bugcheck, run `C:\Users\[your name]\source\repos\[your project name]\x64\Debug\trigger.exe`.
<br>
It should've bugchecked now with your CUSTOM bugcheck code.
<br>
<br>
I'm still working on updating the drivers, and making the bugcheck say "What failed", but this is just a demo. I'll add it soon. Anyways, enjoy and have fun!
