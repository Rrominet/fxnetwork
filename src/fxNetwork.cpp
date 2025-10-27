#include "./fxNetwork.h"
#include "./MainWindow.h"
#include "giomm/dbusproxy.h"
#include "glibconfig.h"
#include "glibmm/refptr.h"
#include "glibmm/variantdbusstring.h"
#include "glibmm/varianttype.h"
#include "mlgui.2/src/GuiCommand.h"
#include "debug.h"
#include "mlgui.2/src/App.hpp"
#include <giomm.h>

#include <iostream>


#include "dbus/nm.h"

namespace fxnetwork
{
    fxNetwork* _fxnetwork = nullptr;
}

fxNetwork::fxNetwork(int argc,char *argv[]) : ml::App(argc,argv)
{
    fxnetwork::_fxnetwork = this;	
    this->createCommands();
    this->createWindows();
    this->setEvents();
    db::setLogFile(files::execDir() + files::sep() + "fxnetwork.log");

    auto later = [this]
    {
        this->addCss("fxnetwork.css");
    };

    this->setTimeout(later, 16);
    _cmds.command("load-devices")->exec();
}

void fxNetwork::setEvents()
{
    _events.add("wifi-state-changed", [this]{this->reloadDevices();});
}

void fxNetwork::createCommands()
{
//     auto cmd = _cmds.createCommand<ml::GuiCommand>("Create Command", "create-cmd");
//     cmd->aliases().concat({"ccmd", "ncmd"});
//     cmd->setHelp("Create a new command to execute...");
//     cmd->setKeybind("ctrl n");
//     cmd->setExec([this](const std::any&){auto w = this->createWindow<CommandWindow>(_main); w->show();});


    auto cmd = _cmds.createCommand<ml::GuiCommand>("Load Devices", "load-devices");
    cmd->aliases().concat({"devices"});
    cmd->setHelp("Realod the available devices for Network Management like Ethernet or Wifi cards.");
    cmd->setKeybind("ctrl r");
    cmd->setExec([this](const std::any&){this->reloadDevices();});

    cmd = _cmds.createCommand<ml::GuiCommand>("Enable Wifi", "enable-wifi");
    cmd->setHelp("Enable the Wifi globally.");
    cmd->setExec([this](const std::any&){this->enableWifi();});

    cmd = _cmds.createCommand<ml::GuiCommand>("Disable Wifi", "disable-wifi");
    cmd->setHelp("Disable the Wifi globally.");
    cmd->setExec([this](const std::any&){this->disableWifi();});
 }

void fxNetwork::createWindows()
{
    this->createMainWindow<MainWindow>();
    _fxnetworkW = (MainWindow*)_main;
}

void fxNetwork::reloadDevices()
{
    _fxnetworkW->setWorking(true)	;
    _fxnetworkW->setInfos("Loading devices...");

    _devicesData.clear();
    auto loaded = [this](ml::Vec<dbus::nm::Device>& devices)
    {
        for (auto& d : devices)
        {
            json dev;
            dev["name"] = d.interface();
            dev["type"] = d.type();
            dev["rtype"] = d.readableType();
            dev["state"] = d.readableState();
            dev["path"] = d.path();

            _devicesData.push_back(dev);
        }
        _events.emit("devices-loaded");
        _fxnetworkW->setWorking(false)	;
        _fxnetworkW->setInfos("Devices loaded.");
    };
    dbus::nm::all_devices(loaded);
}

void fxNetwork::enableWifi()
{
    _fxnetworkW->setWorking(true)	;
    _fxnetworkW->setInfos("Enabling Wifi...");

    auto loaded = [this]
    {
        _fxnetworkW->setWorking(false)	;
        _fxnetworkW->setInfos("Wifi enabled.");
        _events.emit("wifi-state-changed");
    };

    dbus::nm::enable_wifi(loaded);
}

void fxNetwork::disableWifi()
{
    _fxnetworkW->setWorking(true)	;
    _fxnetworkW->setInfos("Disabling devices...");

    auto loaded = [this]
    {
        _fxnetworkW->setWorking(false)	;
        _fxnetworkW->setInfos("Wifi disabled.");
        _events.emit("wifi-state-changed");
    };
    	
    dbus::nm::disable_wifi(loaded);
}

fxNetwork::~fxNetwork()
{
    fxnetwork::_fxnetwork = nullptr;	
}

namespace fxnetwork
{
    fxNetwork* get(){return _fxnetwork;}
}

