#ifndef TRAINTYPE_HXX
#define TRAINTYPE_HXX

#include <QString>

class TrainType
{
    private:
        const QString _tops_classification = "";
        const int _mass = -1;           // In short tonnes
        const int _max_power = -1;      // In kW
        const int _max_speed = -1;      // In kph
        const int _max_brake = -1;      // In short tonnes
    public:
        TrainType() {}
        TrainType(const QString& tops_class, const int& mass,
                  const int& power, const int& max_speed,
                  const int& max_brake) :
                    _tops_classification(tops_class), _mass(mass),
                    _max_power(power), _max_speed(max_speed), _max_brake(max_brake) {}
        int getMaxSpeed() const {return _max_speed;}
        int getMaxPower() const {return _max_power;}
        int getMaxBrake() const {return _max_brake;}
        int getMass() const {return _mass;}
        QString getClass() const {return _tops_classification;}
        QString summarise();
};

#endif // TRAINTYPE_HXX
