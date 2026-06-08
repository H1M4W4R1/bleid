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

With a custom base UUID template:

```powershell
bleid.exe uuid "my device" --base "ae615XXX-XXXX-4XXX-8000-XXXXXXXXXXXX" --service 1 --characteristic 2 --descriptor 3
```

Placeholder layout:

```text
ae615[SVC]-[CHAR]-4[DESC]-8000-[NAME_HASH]
     XXX   XXXX    XXX        XXXXXXXXXXXX
```

For example, the command above fills:

```text
SVC       -> 001
CHAR      -> 0002
DESC      -> 003
NAME_HASH -> generated from "my device"
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

The check also detects UUID collisions against already-reserved names. This matters because different names can produce the same generated UUID suffix.

Use the same UUID options as generation when checking a specific UUID space:

```powershell
bleid.exe reserve --check "my device" --base "ae615XXX-XXXX-4XXX-8000-XXXXXXXXXXXX" --service 1 --characteristic 2 --descriptor 3
```

If the name is not reserved but its generated UUID collides with another reserved name, the command reports the colliding reserved name and UUID.

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
