#include "./MainWindow.h"
#include "./fxNetwork.h"
#include "mlgui.2/src/EventInfos.h"
#include "mlgui.2/src/Scrollable.hpp"
#include "mlgui.2/src/enums.h"
#include "dbus/nm.h"
#include "mlgui.2/src/ListWidget.h"
#include "./Device.h"
#include "mlgui.2/src/Menu.h"
#include "mlgui.2/src/MenuBar.h"

MainWindow::MainWindow(ml::App* app) : ml::Window(app), _devices(){}
MainWindow::MainWindow(ml::App* app, ml::Window* parent) : ml::Window(app, parent), _devices(){}
MainWindow::~MainWindow(){}

void MainWindow::init()
{
    ml::Window::init();	
    this->setTitle("fxnetwork");
    this->createMenus();

    _setEvents();
    this->setSize(800, 600);

    auto dev_title = _main->createLabel("Devices : ");
    dev_title->addCssClass("devices-title");
    dev_title->setHAlign(ml::CENTER);
    _devicesls = _main->createComposedWidget<ml::ListWidget>(_main.get()).get();
}

void MainWindow::_setEvents()
{
    fxnetwork::get()->events().add("devices-loaded", [this](){this->redrawDevices(fxnetwork::get()->devicesData());});
}

void MainWindow::createMenus()
{
    auto devicesm = ml::app()->menusFactory().create("devices", "Devices");
    devicesm->addCommand("load-devices");
    _menuBar->addMenu("devices");

    auto wifim = ml::app()->menusFactory().create("wifi", "Wifi");
    wifim->addCommand("enable-wifi");
    wifim->addCommand("disable-wifi");
    _menuBar->addMenu("wifi");
}

void MainWindow::clearDevices()
{
    _devicesls->clear();	
    _devices.clear();
}

void MainWindow::redrawDevices(const json& data)
{
    this->clearDevices();
    for (const auto &d :  data)
    {
        auto dev = std::make_unique<Device>(d);
        _devicesls->add(dev->box());
        _devices.push_back(std::move(dev));
    }
}

