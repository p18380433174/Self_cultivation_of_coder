#include <iostream>
#include "faultFactory.hpp"

class Fault
{
public:
    virtual void detection() = 0;
    virtual ~Fault() {}
};
class VoltageAlarm : public Fault
{
public:
    VoltageAlarm() : _name("VoltageAlarm") {}
    void detection() override
    {
        cout << _name << " detecting...\n";
    }
private:
    std::string _name;
};
int main(int argc, char* argv[])
{
    FaultRegistrar<Fault, VoltageAlarm>::instance().reigsterFault("VoltageAlarm");
    FaultFactory<Fault>::instance().getFaultImplement("VoltageAlarm")->detection();
    return 0;
}