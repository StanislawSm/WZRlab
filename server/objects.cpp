/*********************************************************************
	Simulation obiektów fizycznych ruchomych np. samochody, statki, roboty, itd.
	+ obs³uga obiektów statycznych np. env.
	**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "objects.h"


#pragma warning( disable : 4996)

MovableObject::MovableObject()             // konstruktor                   
{
	iID = (unsigned int)(rand() % 1000);  // identyfikator obiektu

	// zmienne zwi¹zame z akcjami kierowcy
	F = Fb = 0;	// si³y dzia³aj¹ce na obiekt 
	breaking_factor = 0;			// stopieñ hamowania
	steer_wheel_speed = 0;  // prêdkoœæ krêcenia kierownic¹ w rad/s
	if_keep_steer_wheel = 0;  // informacja czy kieronica jest trzymana

	// sta³e samochodu
	mass_own = 12.0;			// masa obiektu [kg]
	//Fy = mass_own*9.81;        // si³a nacisku na podstawê obiektu (na ko³a pojazdu)
	length = 9.0;
	width = 5.0;
	height = 1.7;
	clearance = 0.0;     // wysokoœæ na której znajduje siê podstawa obiektu
	front_axis_dist = 1.0;     // odleg³oœæ od przedniej osi do przedniego zderzaka 
	back_axis_dist = 0.2;       // odleg³oœæ od tylniej osi do tylniego zderzaka
	steer_wheel_ret_speed = 0.5; // prêdkoœæ powrotu kierownicy w rad/s (gdy zostateie puszczona)

	// parametry stateu auta:
	state.steering_angle = 0;
	state.vPos.y = clearance + height / 2 + 20; // wysokoœæ œrodka ciê¿koœci w osi pionowej pojazdu
	state.vPos.x = 0;
	state.vPos.z = 0;
	quaternion qObr = AsixToQuat(Vector3(0, 1, 0), 0.1*PI / 180.0); // obrót obiektu o k¹t 30 stopni wzglêdem osi y:
	state.qOrient = qObr*state.qOrient;
}

MovableObject::~MovableObject()            // destruktor
{
}

void MovableObject::ChangeState(ObjectState __state)  // przepisanie podanego stateu 
{                                                // w przypadku obiektów, które nie s¹ symulowane
	state = __state;
}

ObjectState MovableObject::State()                // metoda zwracaj¹ca state obiektu ³¹cznie z iID
{
	return state;
}
