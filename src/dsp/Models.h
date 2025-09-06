#pragma once
#include <array>

enum ModalModels {
	String,
	Beam,
	Squared,
	Membrane,
	Plate,
	Drumhead,
	Marimba,
	OpenTube,
	ClosedTube,
	Marimba2,
	Bell,
	Djembe
};

class Models
{
public:
    Models();
	~Models() {};

	std::array<double, 64> getGains(ModalModels model);

    // triggered on model ratio param changes
    void recalcBeam(bool resA, double ratio);
    void recalcMembrane(bool resA, double ratio);
    void recalcPlate(bool resA, double ratio);

	std::array<double, 64> calcDjembe(double freq, double ratio);

    std::array<double, 64> bFree;
    std::array<std::array<double, 64>, 12> aModels;
    std::array<std::array<double, 64>, 12> bModels;
	std::array<std::array<double, 64>, 4> modelGains;
};