/*
   This file contains all data structures and definitions associated
   with facility message usage and the ROSE components included
   within those messages.

   by Matthew Fredrickson <creslin@digium.com>
   Copyright (C) Digium, Inc. 2004
*/

#ifndef _PRI_FACILITY_H
#define _PRI_FACILITY_H

/* Protocol Profile field */
#define Q932_PROTOCOL_ROSE			0x11	/* X.219 & X.229 */
#define Q932_PROTOCOL_CMIP			0x12	/* Q.941 */
#define Q932_PROTOCOL_ACSE			0x13	/* X.217 & X.227 */
#define Q932_PROTOCOL_GAT			0x16
#define Q932_PROTOCOL_EXTENSIONS	0x1F

/* Argument values */
#define ROSE_NAME_PRESENTATION_ALLOWED_SIMPLE	0x80
#define ROSE_NAME_PRESENTATION_RESTRICTED_NULL	0x87
#define ROSE_NAME_NOT_AVAIL						0x84

/* Component types */
#define COMP_TYPE_INTERPRETATION			0x8B
#define COMP_TYPE_NETWORK_PROTOCOL_PROFILE	0x92
#define COMP_TYPE_INVOKE					0xA1
#define COMP_TYPE_RETURN_RESULT				0xA2
#define COMP_TYPE_RETURN_ERROR				0xA3
#define COMP_TYPE_REJECT					0xA4
#define COMP_TYPE_NFE						0xAA

/* Operation ID values */
/* Q.952 ROSE operations */
#define ROSE_DIVERTING_LEG_INFORMATION1		18
#define ROSE_DIVERTING_LEG_INFORMATION2		15
#define ROSE_DIVERTING_LEG_INFORMATION3		19
/* Q.SIG operations */
#define SS_CNID_CALLINGNAME					0
#define SS_DIVERTING_LEG_INFORMATION2		22
#define SS_MWI_ACTIVATE						80
#define SS_MWI_DEACTIVATE					81
#define SS_MWI_INTERROGATE					82

/* ROSE definitions and data structures */
#define INVOKE_IDENTIFIER			0x02
#define INVOKE_LINKED_IDENTIFIER	0x80
#define INVOKE_NULL_IDENTIFIER		__USE_ASN1_NULL

/* ASN.1 Data types */
#define ASN1_BOOLEAN			0x01
#define ASN1_INTEGER			0x02
#define ASN1_BITSTRING			0x03
#define ASN1_OCTETSTRING		0x04
#define ASN1_NULL				0x05
#define ASN1_OBJECTIDENTIFIER	0x06
#define ASN1_OBJECTDESCRIPTER	0x07
#define ASN1_EXTERN				0x08
#define ASN1_REAL				0x09
#define ASN1_ENUMERATED			0x0a
#define ASN1_EMBEDDEDPDV		0x0b
#define ASN1_UTF8STRING			0x0c
#define ASN1_RELATIVEOBJECTID	0x0d
/* 0x0e & 0x0f are reserved for future ASN.1 editions */
#define ASN1_SEQUENCE			0x10
#define ASN1_SET				0x11
#define ASN1_NUMERICSTRING		0x12
#define ASN1_PRINTABLESTRING	0x13
#define ASN1_TELETEXSTRING		0x14
#define ASN1_IA5STRING			0x16
#define ASN1_UTCTIME			0x17
#define ASN1_GENERALIZEDTIME	0x18

#define INVOKE_OPERATION_INT	__USE_ASN1_INTEGER
#define INVOKE_OBJECT_ID		__USE_ASN1_OBJECTIDENTIFIER

/* Q.952 Divert cause */
#define Q952_DIVERT_REASON_UNKNOWN		0x00
#define Q952_DIVERT_REASON_CFU			0x01
#define Q952_DIVERT_REASON_CFB			0x02
#define Q952_DIVERT_REASON_CFNR			0x03
#define Q952_DIVERT_REASON_CD			0x04
#define Q952_DIVERT_REASON_IMMEDIATE	0x05
/* Q.SIG Divert cause. Listed in ECMA-174 */
#define QSIG_DIVERT_REASON_UNKNOWN		0x00	/* Call forward unknown reason */
#define QSIG_DIVERT_REASON_CFU			0x01	/* Call Forward Unconditional (other reason) */
#define QSIG_DIVERT_REASON_CFB			0x02	/* Call Forward Busy */
#define QSIG_DIVERT_REASON_CFNR			0x03	/* Call Forward No Reply */

/* Q.932 Type of number */
#define Q932_TON_UNKNOWN				0x00
#define Q932_TON_INTERNATIONAL			0x01
#define Q932_TON_NATIONAL				0x02
#define Q932_TON_NET_SPECIFIC			0x03
#define Q932_TON_SUBSCRIBER				0x04
#define Q932_TON_ABBREVIATED			0x06

struct rose_component {
	u_int8_t type;
	u_int8_t len;
	u_int8_t data[0];
};

#define GET_COMPONENT(component, idx, ptr, length) \
	if ((idx)+2 >= (length)) \
		break; \
	(component) = (struct rose_component*)&((ptr)[idx]); \
	if ((idx)+(component)->len+2 > (length)) { \
		if ((component)->len != 128) \
			pri_message("Length (%d) of 0x%X component is too long\n", (component)->len, (component)->type); \
	}
/*
	pri_message("XX Got component %d (0x%02X), length %d\n", (component)->type, (component)->type, (component)->len); \
	if ((component)->len > 0) { \
		int zzz; \
		pri_message("XX  Data:"); \
		for (zzz = 0; zzz < (component)->len; ++zzz) \
			pri_message(" %02X", (component)->data[zzz]); \
		pri_message("\n"); \
	}
*/

#define NEXT_COMPONENT(component, idx) \
	(idx) += (component)->len + 2

#define SUB_COMPONENT(component, idx) \
	(idx) += 2

#define CHECK_COMPONENT(component, comptype, message) \
	if ((component)->type && ((component)->type&0x1f) != (comptype)) { \
		pri_message((message), (component)->type); \
		break; \
	}
	
#define ASN1_GET_INTEGER(component, variable) \
	{ \
		int comp_idx; \
		(variable) = 0; \
		for (comp_idx = 0; comp_idx < (component)->len; ++comp_idx) \
			(variable) = ((variable) << 8) | (component)->data[comp_idx]; \
	}

#define ASN1_ADD_SIMPLE(component, comptype, ptr, idx) \
	(component) = (struct rose_component *)&((ptr)[(idx)]); \
	(component)->type = (comptype); \
	(component)->len = 0; \
	(idx) += 2;

#define ASN1_ADD_BYTECOMP(component, comptype, ptr, idx, value) \
	(component) = (struct rose_component *)&((ptr)[(idx)]); \
	(component)->type = (comptype); \
	(component)->len = 1; \
	(component)->data[0] = (value); \
	(idx) += 3;

#define ASN1_PUSH(stack, stackpointer, component) \
	(stack)[(stackpointer)++] = (component);

#define ASN1_FIXUP(stack, stackpointer, data, idx) \
	--(stackpointer); \
	(stack)[(stackpointer)]->len = (unsigned char *)&((data)[(idx)]) - (unsigned char *)(stack)[(stackpointer)] - 2;

/* Decoder fo the invoke part of a ROSE request
   It currently only support calling name decode */
extern int rose_invoke_decode(struct pri *pri, struct q931_call *call, unsigned char *data, int len);
extern int asn1_string_encode(unsigned char asn1_type, void *data, int len, int max_len, void *src, int src_len);
int typeofnumber_from_q931(struct pri *pri, int ton);
int redirectingreason_from_q931(struct pri *pri, int redirectingreason);

#endif /* _PRI_FACILITY_H */
