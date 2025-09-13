# Keylogger

**settings page is still wip!!! don't use that shit for now**

## Installation

### Build from scratch

1. Clone repository

```bash
git clone https://github.com/WarrenWu4/keylogger.git
```

2. Run Make at root (assume you have it already installed)

```bash
cd keylogger/windows
make
```

3. Run install.bat (requires admin powershell)
```powershell
.\install.bat
```

## Uninstall

### Manual uninstall

1. Delete "C:\Program Files\Keydisplay" folder

```powershell
Remove-Item -Path "C:\Program Files\Keydisplay" -Recurse -Force
```

2. Delete "C:\Users\%USER%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Keydisplay.lnk" file

```powershell
Remove-Item -Path "C:\Users\%USER%\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Keydisplay.lnk" -Force
```

### Uninstall script

Run uninstall script "C:\Program Files\Keydisplay\uninstall.bat" (need admin permission)