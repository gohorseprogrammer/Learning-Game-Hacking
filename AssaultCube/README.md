# External Trainer
This is a external hack project for Assault Cube. It's a working project and will be improved over time.
It also serves as a reference for the future.

## External Steps for Gaining Access to the Game

When external, by definition, we are not in the context/space of the process.
We must first find a way to get the information that tells us where the process is. 
When it comes to games, all games have to have a window so that it can display the game on Windows. 
When any program creates a window, it has to give several informations about this window. 
One of them is the window name (the text that appears on top of every program window that isn't fullscreen, fullscreen window also has a name, it just doesn't appears). 

1. **Get WINDOW HANDLE to game window**
We can find this information with the following function, which returns a HWND (HANDLE to the window):
	> FindWindowA(LPCSTR lpClassName, LPCSTR lpWindowName)
	- **LPCSTR lpClassName** - name of the class of the window (one of the informations required to create a window)
	- **LPCSTR lpWindowName** - title/name of the window

 2. **Get ID of the window process thread**
	 With the HANDLE of the window, we can now find the ID of the process in which the window is running. We do that by using:
	 >GetWindowThreadProcessId(HWND hWnd, LPDWORD lpdwProcessId)
	 - **hWnd** - handle to the window from **STEP 1**
     - **lpdwProcessId** - reference to a DWORD variable to save the process ID
    
 3. **Get HANDLE to the game window process**
 With the process ID, we can now get a reference to the process itself.
	>OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
	- **dwDesiredAccess** - type of access you wish to have on this process. Usually value used is: **PROCESS_ALL_ACCESS**
    - **bInheritHandle** - this function (OpenProcess) returns a HANDLE to the process (specified by the ID), this parameter specifies if the HANDLE returned will be used when the process that you`re trying to get creates other processes. Usually set to **FALSE**.
    - **dwProcessId** - process ID from **STEP 2**

## Read data from game
With the access we gained from the previous steps, we can now retrieve or modify the game.
To read data from the game's memory environment, we use:
> ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T * lpNumberOfBytesRead)
- **HANDLE hProcess** - the HANDLE to the game process obtained earlier.
- **LPCVOID lpBaseAddress** - the address within the game's memory space that we want to retrieve. We can use hexadecimal value, for that:
	>(BYTE*) 0x12345678
- **LPVOID lpBuffer** - reference to the variable that will store the data.
- **SIZE_T nSize** - number of bytes to read. Usually, you want to read the size in bytes of the type you're reading. So, we can use :
	> sizeof(lpBuffer);
- **SIZE_T * lpNumberOfBytesRead** - reference to a variable that will store the number of bytes read in reading action. Usually set to **NULL**.