# bleid

A small command-line helper for generating deterministic UUIDs from device names and managing a persistent list of reserved names.

The default UUID layout is:

```text
ae615000-0000-4000-8000-XXXXXXXXXXXX
```

The last 12 hex digits are derived from the provided name or phrase. Optional service, characteristic, and descriptor indexes can be placed into the UUID template.

## Usage

```powershell
bleid.exe help
bleid.exe uuid [device-name] [options]
bleid.exe reserve <name>
bleid.exe reserve --check <name>
bleid.exe reserve --list
bleid.exe free <name>
```

## Generate A UUID

```powershell
bleid.exe uuid "my device"
```

With indexes:

```powershell
bleid.exe uuid "my device" --service 1 --characteristic 2 --descriptor 3
```

Hexadecimal values are supported with a `0x` prefix:

```powershell
bleid.exe uuid "my device" --service 0x10 --characteristic 0x20
```

Available UUID options:

```text
--base, -b           Base UUID template
--service, --svc     Service index, 0..0xfff
--characteristic,
--char               Characteristic index, 0..0xffff
--descriptor,
--desc               Descriptor index, 0..0xfff
--name, -n,
--phrase             Device name or phrase
--help, -h           Show UUID command help
```

## Reserve Names

Reserve a name:

```powershell
bleid.exe reserve "my device"
```

Check a name:

```powershell
bleid.exe reserve --check "my device"
```

List all reserved names:

```powershell
bleid.exe reserve --list
```

Free a name:

```powershell
bleid.exe free "my device"
```

## Persistent Storage

Reserved names are stored in:

```text
%APPDATA%\bleid\reserved_names.txt
```

You can override the storage file with:

```powershell
$env:BLEID_RESERVATIONS = "C:\path\to\reserved_names.txt"
```

## Build

This project uses CMake and C++98.

```powershell
cmake -S . -B build
cmake --build build
```

## License

WTFPL. See [LICENSE.md](LICENSE.md).
