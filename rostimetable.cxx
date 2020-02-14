#include "rostimetable.hxx"

ROSService* ROSTimetable::operator[](const QString& id)
{
    return _services[id];
}

ROSService* ROSTimetable::operator[](const int& i)
{
    int counter = 0;
    const int index = (i == -1) ? _services.size()-1 : i;
    for(auto srv : _services)
    {
        if(counter == index) return srv;
        ++counter;
    }

    return nullptr;
}
