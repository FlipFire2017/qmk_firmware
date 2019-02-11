/* Copyright 2017 Jason Williams
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "color.h"
#include "led_tables.h"
#include "progmem.h"
#include "math.h"

RGB hsv_to_rgb( HSV hsv )
{
	
	float fHPrime = fmod(hsv.h / 60.0,6);
	float fC = hsv.s * hsv.v;
	float fX = fC * (1 - fabs(fmod(fHPrime,2) - 1));
	float fM = hsv.v - fC;

	fC += fM;
	fX += fM;
	RGB rgb;
	
	if(hsv.s == 0.0){
		rgb.r= hsv.v;
		rgb.g= hsv.v;
		rgb.b= hsv.v;
		return rgb;
	}

	if(0 <= fHPrime && fHPrime < 1){
		rgb.r = fC;
		rgb.g = fX;
		rgb.b = fM;
	}else if(1 <= fHPrime && fHPrime < 2){
		rgb.r = fX;
		rgb.g = fC;
		rgb.b = fM;
	}else if(2 <= fHPrime && fHPrime < 3){
		rgb.r = fM;
		rgb.g = fC;
		rgb.b = fX;
	}else if(3 <= fHPrime && fHPrime < 4){
		rgb.r = fM;
		rgb.g = fX;
		rgb.b = fC;
	}else if(4 <= fHPrime && fHPrime < 5){
		rgb.r = fX;
		rgb.g = fM;
		rgb.b = fC;
	}else if(5 <= fHPrime && fHPrime < 6){
		rgb.r = fX;
		rgb.g = fM;
		rgb.b = fC;
	}else{
		rgb.r = fM;
		rgb.b = fM;
		rgb.b = fM;
	}



	return rgb;
}

