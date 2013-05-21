/*
* (c) Disney Enterprises, Inc.  All rights reserved.
*
* This file is licensed under the terms of the Microsoft Public License (MS-PL)
* as defined at: http://opensource.org/licenses/MS-PL.
*
* A complete copy of this license is included in this distribution as the file
* LICENSE.
*/
#ifndef _noise_h_
#define _noise_h_
namespace SeExpr{

//! One octave of non-periodic Perlin noise
template<int d_in,int d_out,class T>
void Noise(const T* in,T* out);

//! One octave of periodic noise
//! period gives the integer period before tiles repease
template<int d_in,int d_out,class T>
void PNoise(const T* in,const int* period,T* out);

//! Fractional Brownian Motion. If turbulence is true then turbulence computed.
template<int d_in,int d_out,bool turbulence,class T> 
void FBM(const T* in,T* out,int octaves,T lacunarity,T gain);

//! Cellular noise with input and output dimensionality
template<int d_in,int d_out,class T>
void CellNoise(const T* in,T* out);

}
#endif
