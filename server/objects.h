#include <stdio.h>
#include "quaternion.h"

#define PI 3.1415926

struct ObjectState
{
	Vector3 vPos;             // polozenie obiektu (�rodka geometrycznego obiektu) 
	quaternion qOrient;       // orientacja (polozenie katowe)
	Vector3 vV, vA;            // predkosc, przyspiesznie liniowe
	Vector3 vV_ang, vA_ang;   // predkosc i przyspieszenie liniowe
	float steering_angle;               // kat skretu kol w radianach (w lewo - dodatni)
};

// Klasa opisuj�ca obiekty ruchome
class MovableObject
{
public:
	int iID;                  // identyfikator obiektu

	ObjectState state;

	float F, Fb;               // si�y dzia�aj�ce na obiekt: F - pchajaca do przodu, Fb - w prawo
	float breaking_factor;    // stopie� hamowania Fh_max = friction*Fy*ham
	float steer_wheel_speed;      // pr�dko�� kr�cenia kierownic�
	bool if_keep_steer_wheel;       // czy kierownica jest trzymana (pr�dko�� mo�e by� zerowa, a kierownica trzymana w pewnym po�o�eniu nie wraca do po�. zerowego)

	float mass_own;				  // masa obiektu	
	float length, width, height; // rozmiary w kierunku lokalnych osi x,y,z
	float clearance;           // wysoko�� na kt�rej znajduje si� podstawa obiektu
	float front_axis_dist;           // odleg�o�� od przedniej osi do przedniego zderzaka 
	float back_axis_dist;             // odleg�o�� od tylniej osi do tylniego zderzaka	
	float steer_wheel_ret_speed;    // pr�dko�� powrotu kierownicy po puszczeniu

public:
	MovableObject();          // konstruktor
	~MovableObject();
	void ChangeState(ObjectState state);          // zmiana stateu obiektu
	ObjectState State();        // metoda zwracajaca state obiektu				
};