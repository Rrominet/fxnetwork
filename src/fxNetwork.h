#pragma once
#include "mlgui.2/src/App.h"

class MainWindow;
class fxNetwork : public ml::App
{
    public:
        fxNetwork(int argc, char *argv[]);
        virtual ~fxNetwork();
        void createCommands();
        void createWindows();
        void setEvents();
        void reloadDevices();

        void enableWifi();
        void disableWifi();

    private : 
        MainWindow* _fxnetworkW=nullptr; //bp cg
        json _devicesData = json::array(); //bp cg

    public : 
#include "./fxNetwork_gen.h"
};

namespace fxnetwork
{
    fxNetwork* get();
}
