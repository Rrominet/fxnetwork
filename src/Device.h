#pragma once

#include "mlgui.2/src/Property.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace ml
{
    class Box;
    class Label;
    class Button;
}
class DeviceWindow;
class Device
{
    public : 
        Device(const json& data={});
        virtual ~Device();

        std::shared_ptr<ml::Box> box(){return _box;}

        void showWindow();

        void update(const json& data);
        void disconnect();

    private: 
        std::shared_ptr<ml::Box> _box = nullptr;
        ml::Box* _head = nullptr;
        ml::Box* _foot = nullptr;
        ml::Label* _name = nullptr;   //bp cg
        ml::Label* _type = nullptr;   //bp cg
        ml::Label* _state = nullptr;   //bp cg
        ml::Button* _disconnect = nullptr;

        json _data; //bp cg

        void _setEvents();
    public : 
#include "./Device_gen.h"
};
