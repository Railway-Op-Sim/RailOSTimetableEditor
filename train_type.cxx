#include "train_type.hxx"

QString TrainType::summarise()
{
    return _tops_classification+"(MASS: "+_mass+"te, MAX_SPEED: "+_max_speed+"kph, MAX_POWER: "+_max_power+"kW, MAX_BRAKING_FORCE: "+_max_brake+"te)";
}
