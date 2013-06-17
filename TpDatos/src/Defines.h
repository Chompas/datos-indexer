/*
 * Defines.h
 *
 *  Created on: 25/05/2013
 *      Author: lara
 */

#ifndef DEFINES_H_
#define DEFINES_H_

#define kDOCUMENTOS 	"documentos"
#define kTERMINOS		"terminosIdx"
#define kLEXICO			"lexico"
#define kLISTATERMINOS	"terminosLista"

#define kSEPARADORES " ,.;:~{}[]+|-_–=?¿!¡/@#$%&¬()<>€çÇ\r\t\n\"'*”“’‘´±¹²³ºª·¢£¤¥¦§¨©«»®¯°µ¶¸¼½¾"

#define kNFRONTCODING	5

/*
 * TAMANIO DEL REGISTRO DE TERMINOS COMPLETOS:
 * 		- 2 BYTES PARA CANTIDAD DE CARACTERES
 * 		- 4 BYTES PARA OFFSET A LISTA DE TERMINOS
 * 		- 4 BYTES PARA OFFSET A LEXICO
 * 		- 4 BYTES PARA OFFSET A DOCUMENTOS
 */
#define TAM_TERMINO_REG 14


#endif /* DEFINES_H_ */
