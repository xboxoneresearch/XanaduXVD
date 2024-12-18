/**********************************************************/
/*  XanaduXVD: Monolithic XVD Parser / playground tool    */
/*                          2024                          */
/*                                                        */
/*   Contributors: TorusHyperV                            */
/*   File: XVDCrypto.h                                    */
/*   Desc: Header containing definitions and info         */
/*         about cryptographic constants, size, keys, etc */
/*         that are related to the XVD format             */
/*                                                        */
/**********************************************************/
#pragma once

///////////////////////////////////////
// C includes
///////////////////////////////////////
#include <stdio.h>
#include <stdint.h>

///////////////////////////////////////
// XanaduXVD includes
///////////////////////////////////////
#include "XVDTypes.h"

///////////////////////////////////////
// Constants (the C++ way)
///////////////////////////////////////
static const size_t cKeyIDLen = 16;
static const size_t cODKLen   = 32;

///////////////////////////////////////
// PUBLIC KEY MATERIALS              //
///////////////////////////////////////

// Signing Keys
static const size_t  cNumSignKeys = 3;
static const uint8_t cGreenXVCPublicSignKey[] = {0x00, 0x00, 0x00};
static const uint8_t cGreenXVDPublicSignKey[] = {
                                        0x00,0x00
                                    };
static const uint8_t cRedXVDPublicSignKey[]   = {0x00, 0x00, 0x00}; // Note: because of how public/private signing works, we could use the private key as well

// Array with all the known keys
static const uint8_t* cKnownSigningKeys[cNumSignKeys] = {cGreenXVCPublicSignKey, cGreenXVDPublicSignKey, cRedXVDPublicSignKey};


// Data encryption Keys (and ODKs here maybe)
// here be dragons