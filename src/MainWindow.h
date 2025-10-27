#pragma once
#include "mlgui.2/src/Window.h"

class Device;

namespace ml
{
    class ListWidget;
}

class MainWindow : public ml::Window
{
    public:
        MainWindow(ml::App* app);
        MainWindow(ml::App* app, ml::Window* parent);
        virtual ~MainWindow();

        virtual void init() override;
        void createMenus();
        void redrawDevices(const json& data = json::array());

        void clearDevices();

    protected : 
        void _setEvents();

        ml::ListWidget* _devicesls = nullptr;
        ml::Vec<std::unique_ptr<Device>> _devices;
};
