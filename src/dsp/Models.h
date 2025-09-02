#pragma once
#include <array>

enum ModelNames {
	String,
	Beam,
	Squared,
	Membrane,
	Plate,
	Drumhead,
	Marimba,
	OpenTube,
	ClosedTube
};

class Models
{
public:
    Models();
	~Models() {};

    // triggered on model ratio param changes
    void recalcBeam(bool resA, double ratio);
    void recalcMembrane(bool resA, double ratio);
    void recalcPlate(bool resA, double ratio);

    std::array<double, 64> bFree;
    std::array<std::array<double, 64>, 9> aModels;
    std::array<std::array<double, 64>, 9> bModels;
};