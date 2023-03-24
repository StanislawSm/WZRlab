/*********************************************************************
	Simulation obiekt�w fizycznych ruchomych np. samochody, statki, roboty, itd.
	+ obs�uga obiekt�w statycznych np. env.
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

	// zmienne zwi�zame z akcjami kierowcy
	F = Fb = 0;	// si�y dzia�aj�ce na obiekt 
	breaking_factor = 0;			// stopie� hamowania
	steer_wheel_speed = 0;  // pr�dko�� kr�cenia kierownic� w rad/s
	if_keep_steer_wheel = 0;  // informacja czy kieronica jest trzymana

	// sta�e samochodu
	mass_own = 12.0;			// masa obiektu [kg]
	//Fy = mass_own*9.81;        // si�a nacisku na podstaw� obiektu (na ko�a pojazdu)
	length = 9.0;
	width = 5.0;
	height = 1.7;
	clearance = 0.0;     // wysoko�� na kt�rej znajduje si� podstawa obiektu
	front_axis_dist = 1.0;     // odleg�o�� od przedniej osi do przedniego zderzaka 
	back_axis_dist = 0.2;       // odleg�o�� od tylniej osi do tylniego zderzaka
	steer_wheel_ret_speed = 0.5; // pr�dko�� powrotu kierownicy w rad/s (gdy zostateie puszczona)

	// parametry stateu auta:
	state.steering_angle = 0;
	state.vPos.y = clearance + height / 2 + 20; // wysoko�� �rodka ci�ko�ci w osi pionowej pojazdu
	state.vPos.x = 0;
	state.vPos.z = 0;
	quaternion qObr = AsixToQuat(Vector3(0, 1, 0), 0.1*PI / 180.0); // obr�t obiektu o k�t 30 stopni wzgl�dem osi y:
	state.qOrient = qObr*state.qOrient;
}

MovableObject::~MovableObject()            // destruktor
{
}

void MovableObject::ChangeState(ObjectState __state)  // przepisanie podanego stateu 
{                                                // w przypadku obiekt�w, kt�re nie s� symulowane
	state = __state;
}

ObjectState MovableObject::State()                // metoda zwracaj�ca state obiektu ��cznie z iID
{
	return state;
}
