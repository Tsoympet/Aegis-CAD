; Inno Setup script for AegisCAD
#define AppName "AegisCAD"
#ifndef AppVersion
#define AppVersion "1.0"
#endif
#ifndef SourceDir
#define SourceDir "..\\install"
#endif
#ifndef OutputDir
#define OutputDir "..\\installer\\artifacts"
#endif

[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
DefaultDirName={pf64}\AegisCAD
DefaultGroupName=AegisCAD
OutputDir={#OutputDir}
OutputBaseFilename=AegisCAD-{#AppVersion}-Setup
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
Compression=lzma
SolidCompression=yes

[Files]
Source: "{#SourceDir}\\bin\\AegisCAD.exe"; DestDir: "{app}\\bin"; Flags: ignoreversion
Source: "{#SourceDir}\\bin\\*"; DestDir: "{app}\\bin"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#SourceDir}\\share\\*"; DestDir: "{app}\\share"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\\AegisCAD"; Filename: "{app}\\bin\\AegisCAD.exe"
Name: "{commondesktop}\\AegisCAD"; Filename: "{app}\\bin\\AegisCAD.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Run]
Filename: "{app}\\bin\\AegisCAD.exe"; Description: "Launch AegisCAD"; Flags: nowait postinstall skipifsilent
