# fxNetwork

A network management GUI application built with C++ and mlgui.2 framework.  
It's portable and standalone on any distros. Pretty useful if you don't use a *classic* window manager that don't have `poly-bar` or equivalent.

![Demo](https://motion-live.com/fxos/data/fxcommander-demo.gif)

## What the fuck is this?

fxNetwork is a desktop application for managing network devices and connections on Linux. It uses D-Bus to communicate with `NetworkManager` and provides a clean GUI to:

- View and manage network devices (Ethernet, WiFi, whatever)
- Scan for available WiFi networks
- Connect to WiFi networks (including hidden ones)
- Disconnect from networks
- Enable/disable WiFi globally

## Build and Installation

The project depends on several things but don't worry about it. 
Just execute the file `build-n-install` as root and you're good to go. 

For your convenience : 
```bash
git clone https://github.com/Rrominet/fxnetwork.git
cd ./fxnetwork
sudo chmod +x ./build-n-install
sudo ./build-n-install
```
> [!WARNING]
> For now the `build-n-install` script is bind to debian based distros (it uses `apt`) but I will port it soon on any others.
> So Stay tuned for that :)

### Dependencies

- [fxmake](https://github.com/Rrominet/py-utils/tree/main/build)
- [cpp-utils](https://github.com/Rrominet/cpp-utils)
- [fxos GUI lib](https://github.com/Rrominet/fxos_gui-lib)
- [nlohmann/json for JSON handling](https://github.com/nlohmann/json)
- Standard C++ library


## Usage 

Once installed you can launch FxNetwork by the command `fxnetwork` (pointing to `/usr/lib/local/fxnetwork` that itself point to `/opt/fxnetwork/fxnetwork`)

Just add it to your prefered launcher and you're good to go.
Why not [fxcommander](https://github.com/Rrominet/fxcommander.git) ? :)

> [!NOTE]
> A desktop file automaticly installed with the `build-n-install` script is on the way ! :)

## Architecture

The project follows a pretty straightforward structure:

### Main Components

- **fxNetwork** (`fxNetwork.cpp/h`): Main application class that handles initialization, commands, and global WiFi state
- **MainWindow** (`MainWindow.cpp/h`): Primary window displaying all available network devices
- **Device** (`Device.cpp/h`): Widget representing a single network device with its properties
- **DeviceWindow** (`DeviceWindow.cpp/h`): Detailed view for a specific device, shows available networks for WiFi devices

### Key Features

#### Device Management
- Lists all network devices (WiFi cards, Ethernet adapters)
- Shows device name, type, and connection state
- Click on a device to see detailed information

#### WiFi Networks
- Scan for available networks
- Display signal strength, frequency, and security status
- Connect to networks (with password prompt if needed)
- Connect to hidden networks
- Shows which network you're currently connected to

#### Commands & Shortcuts
- `load-devices` (Ctrl+R): Reload available devices
- `enable-wifi`: Enable WiFi globally
- `disable-wifi`: Disable WiFi globally
- `scan-networks`: Scan for WiFi networks (in device window)
- `connect-hidden`: Connect to hidden network (in device window)

## File Structure

```
src/
├── fxNetwork.cpp/h          # Main application
├── fxNetwork_gen.h          # Generated getters/setters
├── MainWindow.cpp/h         # Main window
├── Device.cpp/h             # Device widget
├── Device_gen.h             # Generated Device accessors
├── DeviceWindow.cpp/h       # Device detail window
└── main.cpp                 # Entry point
```

## Design Patterns

- **Event-driven architecture**: Uses custom event system for device loading, WiFi state changes
- **Async D-Bus calls**: NetworkManager operations are asynchronous with callbacks
- **Factory pattern**: Widget creation through fxgui's factory
- **Smart pointers**: Uses `std::unique_ptr` and `std::shared_ptr` for memory management

## CSS Styling

The application loads `fxnetwork.css` for styling. Custom CSS classes:
- `.device`, `.device .head`, `.device .foot`
- `.network`, `.network .ssid`, `.network .strength`
- `.device-window`, `.networks-label`
- Signal strength classes: `.strong`, `.medium`, `.weak`

## Notable Implementation Details

- **Network Polling**: When scanning WiFi networks, the app polls for updates every second for 10 iterations (not the best, but it works)
- **D-Bus Integration**: Direct communication with NetworkManager via D-Bus proxies in the `glibmm` library
- **Generated Code**: Some getter/setter code is auto-generated (files ending in `_gen.h`) via the software [fxmake]()


## TODO

 - [ ] Add the *desktop* file automaticly installed.
 - [ ] Certainly a lot of bugs to correct lol.
