#pragma once
#include "mlgui.2/src/Window.h"
#include "mlgui.2/src/utils.h"
#include "dbus/nm.h"

#include "mlgui.2/src/Property.h"
#include "mlgui.2/src/PropertyGroup.h"

namespace ml
{
    class Box;
    class Label;
    class LabelValue;
    class ListWidget;
}

class DeviceWindow : public ml::Window
{
    public : 
        DeviceWindow(ml::App* app);
        DeviceWindow(ml::App* app, ml::Window* parent);
        virtual ~DeviceWindow();

        virtual void init() override;
        void setFromData(const json& data);
        void createCommands();
        void createProps();
        void createMenus();

        void scanNetworks();
        void getConnectedToo();

        void clearNetworks();
        void drawNetworks(const ml::Vec<dbus::nm::WifiNetwork>& networks);
        void drawNetwork(const dbus::nm::WifiNetwork& network);

        void connectToNetWork(const dbus::nm::WifiNetwork& network);
        void connectToHiddenNetwork();

    protected: 
        void _setEvents();

        ml::Label* _title;
        ml::Label* _type;
        ml::Label* _networksLbl;
        ml::LabelValue _connectedToo;
        ml::Box* _foot;

        ml::ListWidget* _networks;

        //dbus path
        std::string _path;
        dbus::nm::DeviceType _dtype;
        dbus::nm::Device _dev;

        ml::PropertyGroup* _networkConnectionProps;

        bool _polling = false;
};
