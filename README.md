# bleid

A small command-line helper for generating deterministic UUIDs from device names and managing a persistent list of reserved names.

The default base UUID template is:

```text
ae615SSS-TTTT-4RRR-8000-XXXXXXXXXXXX
```

`S`, `T`, `R`, and `X` are placeholders for service, characteristic, descriptor, and generated hash values. Placeholder matching is case-insensitive. If service, characteristic, or descriptor values are not provided, their placeholders are filled with zeroes.

## Usage

```powershell
bleid.exe help
bleid.exe uuid [base-template] [device-name] [options]
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
bleid.exe uuid "my device" --base "ae615SSS-TTTT-4RRR-8000-XXXXXXXXXXXX" --service 1 --characteristic 2 --descriptor 3
```

The base can also be provided as the first positional argument:

```powershell
bleid.exe uuid "ae615SSS-TTTT-4RRR-8000-XXXXXXXXXXXX" "my device" --service 1 --characteristic 2 --descriptor 3
```

Placeholder layout:

```text
ae615[SSS]-[TTTT]-4[RRR]-8000-[XXXXXXXXXXXX]
     SSS   TTTT    RRR        XXXXXXXXXXXX
```

For example, the command above fills:

```text
SSS          -> 001
TTTT         -> 0002
RRR          -> 003
XXXXXXXXXXXX -> generated from "my device"
```

Placeholder characters do not need to appear in only one spot. The total count of each placeholder controls that value's width, and matching positions are filled from left to right across the whole template.

If the generated UUID matches a reserved name, `uuid` annotates the output:

```text
[GENERATED_UUID], already reserved
```

If it matches a different reserved name, it prints:

```text
[GENERATED_UUID], collides with [RESERVED_NAME]
```

Available UUID options:

```text
--base, -b           Base UUID template
--service, --svc     Service index
--characteristic,
--char               Characteristic index
--descriptor,
--desc               Descriptor index
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
bleid.exe reserve --check "my device" --base "ae615SSS-TTTT-4RRR-8000-XXXXXXXXXXXX" --service 1 --characteristic 2 --descriptor 3
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
