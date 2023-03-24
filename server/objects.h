#include <stdio.h>
#include "quaternion.h"

#define PI 3.1415926

struct ObjectState
{
	Vector3 vPos;             // polozenie obiektu (œrodka geometrycznego obiektu) 
	quaternion qOrient;       // orientacja (polozenie katowe)
	Vector3 vV, vA;            // predkosc, przyspiesznie liniowe
	Vector3 vV_ang, vA_ang;   // predkosc i przyspieszenie liniowe
	float steering_angle;               // kat skretu kol w radianach (w lewo - dodatni)
};

// Klasa opisuj¹ca obiekty ruchome
class MovableObject
{
public:
	int iID;                  // identyfikator obiektu

	ObjectState state;

	float F, Fb;               // si³y dzia³aj¹ce na obiekt: F - pchajaca do przodu, Fb - w prawo
	float breaking_factor;    // stopieñ hamowania Fh_max = friction*Fy*ham
	float steer_wheel_speed;      // prêdkoœæ krêcenia kierownic¹
	bool if_keep_steer_wheel;       // czy kierownica jest trzymana (prêdkoœæ mo¿e byæ zerowa, a kierownica trzymana w pewnym po³o¿eniu nie wraca do po³. zerowego)

	float mass_own;				  // masa obiektu	
	float length, width, height; // rozmiary w kierunku lokalnych osi x,y,z
	float clearance;           // wysokoœæ na której znajduje siê podstawa obiektu
	float front_axis_dist;           // odleg³oœæ od przedniej osi do przedniego zderzaka 
	float back_axis_dist;             // odleg³oœæ od tylniej osi do tylniego zderzaka	
	float steer_wheel_ret_speed;    // prêdkoœæ powrotu kierownicy po puszczeniu

public:
	MovableObject();          // konstruktor
	~MovableObject();
	void ChangeState(ObjectState state);          // zmiana stateu obiektu
	ObjectState State();        // metoda zwracajaca state obiektu				
};