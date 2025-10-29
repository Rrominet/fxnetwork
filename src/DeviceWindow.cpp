#include "./DeviceWindow.h"
#include "dbus/nm.h"
#include "mlgui.2/src/Label.h"
#include "mlgui.2/src/ListWidget.h"
#include "mlgui.2/src/AskPropertyGroupDialog.h"

#include "mlgui.2/src/Box.hpp"
#include "mlgui.2/src/Property.h"
#include "mlgui.2/src/Scrollable.hpp"
#include "mlgui.2/src/enums.h"
#include "mlgui.2/src/MenuBar.h"

 DeviceWindow::DeviceWindow(ml::App* app): ml::Window(app)
{
	
}

 DeviceWindow::DeviceWindow(ml::App* app, ml::Window* parent) : ml::Window(app, parent)
{
	
}

 DeviceWindow::~DeviceWindow()
{
	
}

void DeviceWindow::init()
{
    lg("DeviceWindow::init()");
    ml::Window::init();

    this->createProps();
    this->createCommands();
    this->setSize(520, 480);

    this->createMenus();

    _main->addCssClass("device-window");
    _title = _main->createLabel("Unkown").get();
    _title->addCssClass("title");
    _title->setHAlign(ml::CENTER);
    _type = _main->createLabel("Unkown").get();
    _type->addCssClass("type");
    _type->setHAlign(ml::RIGHT);
    _connectedToo = ml::labelValue(_main.get(), "Connected too", "None");
    _networksLbl = _main->createLabel("Avialable Networks :").get();
    _networksLbl->addCssClass("networks-label");
    _networks = _main->createComposedWidget<ml::ListWidget>(_main.get()).get();

    _foot = _main->createBox().get();
    _foot->addCssClass("device-foot");
    _foot->setOrient(ml::HORIZONTAL);
    _foot->appendCommand(_cmds.command("connect-hidden").get());
    _foot->appendCommand(_cmds.command("scan-networks").get());

    //changed and let the polling system sill run without a crash - not really optimized for memory for who cares for a network manager that you will close in minutes.
    this->hideOnClose(true); 
}

void DeviceWindow::createCommands()
{
    auto cmd = _cmds.createCommand<ml::GuiCommand>("Scan Networks", "scan-networks");
    cmd->setHelp("Scan the available networks");
    cmd->setExec([this](const std::any&){this->scanNetworks();});

    cmd = _cmds.createCommand<ml::GuiCommand>("Connect To Hidden Network", "connect-hidden");
    cmd->setHelp("Connect to a hidden network.\nYou need to know its SSID and password.");
    cmd->setExec([this](const std::any&){this->connectToHiddenNetwork();});
}

void DeviceWindow::createMenus()
{
    auto ntwsMenu = ml::app()->menusFactory().create("networks", "Networks");	
    ntwsMenu->addCommand(_cmds.command("connect-hidden").get());
    ntwsMenu->addSeparator();
    ntwsMenu->addCommand(_cmds.command("scan-networks").get());
    _menuBar->addMenu("networks");
}

void DeviceWindow::createProps()
{
    auto ssid = ml::app()->props().create<ml::StringProperty>("SSID", "", "Network SSID");	
    auto password = ml::app()->props().create<ml::StringProperty>("Password", "", "Network Password");
    _networkConnectionProps = ml::app()->props().createGroup({ssid, password});
}

void DeviceWindow::scanNetworks()
{
    if (_dtype != dbus::nm::DeviceType::WIFI)
    {
        ml::app()->error("Only wifi devices can scan networks.");
        return;
    }

    if (_polling)
    {
        ml::app()->error("A scan is already in progress.\nWait a bit please.");
        return;
    }

    _polling = true;

    this->setWorking(true)	;
    this->setInfos("Scanning networks...");

    auto onscanned = [this](dbus::nm::Device, ml::Vec<dbus::nm::WifiNetwork>& networks)
    {
        this->drawNetworks(networks);
        auto interval = [this]{
            _dev.networks([this](dbus::nm::Device, ml::Vec<dbus::nm::WifiNetwork>& networks){
                this->drawNetworks(networks);
                });
        };

        auto onintervalend = [this]{
            _polling = false;
            this->setWorking(false)	;
            this->setInfos("Networks scanned.");
        };

        ml::app()->setInterval(interval, 1000, 10, onintervalend);
    };
    _dev.scanNetworks(onscanned);
}

void DeviceWindow::setFromData(const json& data)
{
    lg("DeviceWindow::setFromData()");
    lg(data.dump(4));
    if (data.contains("name"))
    {
        _title->setValue(data["name"].get<std::string>());
        this->setTitle(data["name"].get<std::string>());
    }
    if (data.contains("rtype"))
        _type->setValue(data["rtype"].get<std::string>());
    if (data.contains("path"))
        _path = data["path"].get<std::string>();
    if (data.contains("type"))
        _dtype = data["type"].get<dbus::nm::DeviceType>();

    if (_dtype == dbus::nm::DeviceType::WIFI)
    {
        _connectedToo.show();
        _networksLbl->show();
        _networks->show();
        _foot->show();
    }
    else
    {
        _connectedToo.hide();
        _networksLbl->hide();
        _networks->hide();
        _foot->hide(); 
    }

    _dev = dbus::nm::Device(_path);

    try
    {
        this->getConnectedToo();
    }
    catch(const std::exception& e)
    {
        lg(e.what());
    }

    if(_dtype == dbus::nm::DeviceType::WIFI)
        this->scanNetworks();
}

void DeviceWindow::getConnectedToo()
{
    if (_dtype != dbus::nm::DeviceType::WIFI)
        return;
    std::string connectedPath = _dev.currentConnected();
    lg(connectedPath);
    dbus::nm::WifiNetwork net = dbus::nm::WifiNetwork(connectedPath, "org.freedesktop.NetworkManager.Connection.Active");
    lg(_connectedToo.value);
    lg(_connectedToo.value->value());
    lg(net.ssid());
    _connectedToo.value->setValue(net.ssid());
}

void DeviceWindow::clearNetworks()
{
    _networks->clear();	
}

void DeviceWindow::drawNetworks(const ml::Vec<dbus::nm::WifiNetwork>& networks)
{
    this->clearNetworks();
    for(const auto& network : networks)
        this->drawNetwork(network);
}

void DeviceWindow::drawNetwork(const dbus::nm::WifiNetwork& network)
{
    auto ssid = network.ssid();
    if (ssid.empty())
        return;
    lg("DeviceWindow::drawNetwork()");
    auto box = ml::app()->widgetsFactory().createBox();	
    box->addCssClass("network");
    box->setOrient(ml::HORIZONTAL);

    auto left = box->createBox();
    left->addCssClass("left");
    auto right = box->createBox();
    right->addCssClass("right");

    right->setOrient(ml::VERTICAL);
    left->setOrient(ml::VERTICAL);

    left->setHExpand();

    auto ssidlbl = left->createLabel(ssid).get();
    ssidlbl->addCssClass("ssid");

    auto str = network.strength();
    auto strength = right->createLabel(std::to_string(str) + "%");
    strength->addCssClass("strength");

    if (str > 70)
        strength->addCssClass("strong");
    else if (str > 30)
        strength->addCssClass("medium");
    else
        strength->addCssClass("weak");
    
    auto frequency = right->createLabel(network.readableFrequency());
    frequency->addCssClass("frequency");

    auto psw = right->createLabel();
    if (network.needPassword())
        psw->setValue("Password Protected.");
    psw->addCssClass("protected");

    strength->setHAlign(ml::RIGHT);
    frequency->setHAlign(ml::RIGHT);
    psw->setHAlign(ml::RIGHT);

    box->setCursor("pointer");
    _networks->add(box);

    box->addEventListener(ml::LEFT_UP, [this, network](ml::EventInfos&){this->connectToNetWork(network);});
}

void DeviceWindow::connectToNetWork(const dbus::nm::WifiNetwork& network)
{
    if (_dtype != dbus::nm::DeviceType::WIFI)
    {
        ml::app()->error("Device is not a wifi device.\nCannot connect to network.");
        return;
    }
    _networkConnectionProps->get<ml::StringProperty>("SSID")->set(network.ssid()); //changed
    auto askw = ml::app()->ask(_networkConnectionProps, "Connect to Network");
    askw->events().add("ok", [this, network](){
                this->setWorking(true);
                this->setInfos("Connecting to " + network.ssid() + " ...");
                auto onconnected = [this, network]
                {
                    auto timeout = [this, network]{
                        this->setWorking(false);
                        try
                        {
                            this->getConnectedToo();
                            this->setInfos("Connected to " + network.ssid());
                        }
                        catch(const std::exception& e)
                        {
                            _connectedToo.value->setValue("None");
                            this->setInfos("Connection failed. Typically due to bad password.");
                            ml::app()->error("Connection failed. Typically due to bad password.");
                        }
                    };

                    // the connection is typicly pretty long...
                    // TODO : a better way would be to listen to the signal for the state changes..
                    ml::app()->setTimeout(timeout, 5000);
                };

                auto onerror = [this](const std::string& error)
                {
                    this->setWorking(false);
                    this->setInfos("");
                    ml::app()->error(error);
                };

                _dev.connect(network, _networkConnectionProps->get<ml::StringProperty>("Password")->value(), onconnected, onerror);
            });
}

void DeviceWindow::connectToHiddenNetwork()
{
    if (_dtype != dbus::nm::DeviceType::WIFI)
    {
        ml::app()->error("Device is not a wifi device.\nCannot connect to network.");
        return;
    }
    auto askw = ml::app()->ask(_networkConnectionProps, "Connect to Hidden Network");
    askw->events().add("ok", [this](){lg("Connecting ...");});
}

