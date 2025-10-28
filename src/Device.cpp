#include "./Device.h"
#include "dbus/nm.h"
#include "mlgui.2/src/Widget.h"
#include "mlgui.2/src/App.h"
#include "mlgui.2/src/WidgetsFactory.h"
#include "mlgui.2/src/Box.h"
#include "mlgui.2/src/Label.h"
#include "mlgui.2/src/Button.h"

#include "mlgui.2/src/Box.hpp"
#include "mlgui.2/src/enums.h"
#include "./DeviceWindow.h"

#include "mlgui.2/src/App.hpp"

 Device::Device(const json& data)
{
    _box = ml::app()->widgetsFactory().createBox();
    _box->addCssClass("device");
    _box->setCursor("pointer");

    _head = _box->createBox().get();
    _head->addCssClass("head");
    _head->setOrient(ml::HORIZONTAL);

    _foot = _box->createBox().get();
    _foot->addCssClass("foot");
    _foot->setOrient(ml::HORIZONTAL);

    _name = _head->createLabel("Unkown").get();
    _name->addCssClass("name");

    _state = _head->createLabel("Unkown").get();
    _state->addCssClass("state");

    _disconnect = _head->createButton("Disconnect").get();
    _disconnect->setHAlign(ml::RIGHT);
    _disconnect->addCssClass("disconnect");
    _type = _foot->createLabel("Unavailable").get();
    _type->addCssClass("type");

    _type->setHExpand();
    _type->setHAlign(ml::RIGHT);

    this->update(data);
    _setEvents();
}

void Device::_setEvents()
{
    _box->addEventListener(ml::LEFT_UP, [this](ml::EventInfos& e){this->showWindow();});	
    _disconnect->addEventListener(ml::LEFT_UP, [this](ml::EventInfos& e){this->disconnect();});
}

void Device::showWindow()
{
    auto window = ml::app()->createWindow<DeviceWindow>(ml::app()->main());
    window->setFromData(_data);
    window->show();
}

void Device::update(const json& data)
{
    _data = data;
    if (data.contains("name"))
        _name->setValue(data["name"].get<std::string>());
    if (data.contains("state"))
        _state->setValue(data["state"].get<std::string>());
    if (data.contains("rtype"))
        _type->setValue(data["rtype"].get<std::string>());
}

void Device::disconnect()
{
    ml::app()->main()->setInfos("Disconnecting...");
    ml::app()->main()->setWorking(true);

    auto dev = dbus::nm::Device(_data["path"].get<std::string>());
    dev.disconnect([this](auto& infos){
                ml::app()->main()->setInfos(infos);
                ml::app()->main()->setWorking(false);
            });
}

 Device::~Device(){}
