/* 
   Routines for dealing with facility messages and their respective
   components (ROSE)

   by Matthew Fredrickson <creslin@digium.com>
   Copyright (C) 2004 Digium, Inc
*/

#include "compat.h"
#include "libpri.h"
#include "pri_internal.h"
#include "pri_q921.h"
#include "pri_q931.h"
#include "pri_facility.h"

#include <stdlib.h>
#include <string.h>

#undef DEBUG

struct addressingdataelements_presentednumberunscreened {
	char partyAddress[21];
	char partySubaddress[21];
	int  npi;
	int  ton;
	int  pres;
};

int redirectingreason_from_q931(struct pri *pri, int redirectingreason)
{
	switch(pri->switchtype) {
	case PRI_SWITCH_QSIG:
		switch(redirectingreason) {
		case PRI_REDIR_UNKNOWN:
			return QSIG_DIVERT_REASON_UNKNOWN;
		case PRI_REDIR_FORWARD_ON_BUSY:
			return QSIG_DIVERT_REASON_CFB;
		case PRI_REDIR_FORWARD_ON_NO_REPLY:
			return QSIG_DIVERT_REASON_CFNR;
		case PRI_REDIR_UNCONDITIONAL:
			return QSIG_DIVERT_REASON_CFU;
		case PRI_REDIR_DEFLECTION:
		case PRI_REDIR_DTE_OUT_OF_ORDER:
		case PRI_REDIR_FORWARDED_BY_DTE:
			pri_message("!! Don't know how to convert Q.931 redirection reason %d to Q.SIG\n", redirectingreason);
			/* Fall through */
		default:
			return QSIG_DIVERT_REASON_UNKNOWN;
		}
	default:
		switch(redirectingreason) {
		case PRI_REDIR_UNKNOWN:
			return Q952_DIVERT_REASON_UNKNOWN;
		case PRI_REDIR_FORWARD_ON_BUSY:
			return Q952_DIVERT_REASON_CFB;
		case PRI_REDIR_FORWARD_ON_NO_REPLY:
			return Q952_DIVERT_REASON_CFNR;
		case PRI_REDIR_DEFLECTION:
			return Q952_DIVERT_REASON_CD;
		case PRI_REDIR_UNCONDITIONAL:
			return Q952_DIVERT_REASON_CFU;
		case PRI_REDIR_DTE_OUT_OF_ORDER:
		case PRI_REDIR_FORWARDED_BY_DTE:
			pri_message("!! Don't know how to convert Q.931 redirection reason %d to Q.952\n", redirectingreason);
			/* Fall through */
		default:
			return Q952_DIVERT_REASON_UNKNOWN;
		}
	}
}

static int redirectingreason_for_q931(struct pri *pri, int redirectingreason)
{
	switch(pri->switchtype) {
	case PRI_SWITCH_QSIG:
		switch(redirectingreason) {
		case QSIG_DIVERT_REASON_UNKNOWN:
			return PRI_REDIR_UNKNOWN;
		case QSIG_DIVERT_REASON_CFU:
			return PRI_REDIR_UNCONDITIONAL;
		case QSIG_DIVERT_REASON_CFB:
			return PRI_REDIR_FORWARD_ON_BUSY;
		case QSIG_DIVERT_REASON_CFNR:
			return PRI_REDIR_FORWARD_ON_NO_REPLY;
		default:
			pri_message("!! Unknown Q.SIG diversion reason %d\n", redirectingreason);
			return PRI_REDIR_UNKNOWN;
		}
	default:
		switch(redirectingreason) {
		case Q952_DIVERT_REASON_UNKNOWN:
			return PRI_REDIR_UNKNOWN;
		case Q952_DIVERT_REASON_CFU:
			return PRI_REDIR_UNCONDITIONAL;
		case Q952_DIVERT_REASON_CFB:
			return PRI_REDIR_FORWARD_ON_BUSY;
		case Q952_DIVERT_REASON_CFNR:
			return PRI_REDIR_FORWARD_ON_NO_REPLY;
		case Q952_DIVERT_REASON_CD:
			return PRI_REDIR_DEFLECTION;
		case Q952_DIVERT_REASON_IMMEDIATE:
			pri_message("!! Dont' know how to convert Q.952 diversion reason IMMEDIATE to PRI analog\n");
			return PRI_REDIR_UNKNOWN;	/* ??? */
		default:
			pri_message("!! Unknown Q.952 diversion reason %d\n", redirectingreason);
			return PRI_REDIR_UNKNOWN;
		}
	}
}

int typeofnumber_from_q931(struct pri *pri, int ton)
{
	switch(ton) {
	case PRI_TON_INTERNATIONAL:
		return Q932_TON_INTERNATIONAL;
	case PRI_TON_NATIONAL:
		return Q932_TON_NATIONAL;
	case PRI_TON_NET_SPECIFIC:
		return Q932_TON_NET_SPECIFIC;
	case PRI_TON_SUBSCRIBER:
		return Q932_TON_SUBSCRIBER;
	case PRI_TON_ABBREVIATED:
		return Q932_TON_ABBREVIATED;
	case PRI_TON_RESERVED:
	default:
		pri_message("!! Unsupported Q.931 TypeOfNumber value (%d)\n", ton);
		/* fall through */
	case PRI_TON_UNKNOWN:
		return Q932_TON_UNKNOWN;
	}
}

static int typeofnumber_for_q931(struct pri *pri, int ton)
{
	switch (ton) {
	case Q932_TON_UNKNOWN:
		return PRI_TON_UNKNOWN;
	case Q932_TON_INTERNATIONAL:
		return PRI_TON_INTERNATIONAL;
	case Q932_TON_NATIONAL:
		return PRI_TON_NATIONAL;
	case Q932_TON_NET_SPECIFIC:
		return PRI_TON_NET_SPECIFIC;
	case Q932_TON_SUBSCRIBER:
		return PRI_TON_SUBSCRIBER;
	case Q932_TON_ABBREVIATED:
		return PRI_TON_ABBREVIATED;
	default:
		pri_message("!! Invalid Q.932 TypeOfNumber %d\n", ton);
		return PRI_TON_UNKNOWN;
	}
}

int asn1_string_encode(unsigned char asn1_type, void *data, int len, int max_len, void *src, int src_len)
{
	struct rose_component *comp = NULL;
	
	if (len < 2 + src_len)
		return -1;

	if (max_len && (src_len > max_len))
		src_len = max_len;

	comp = (struct rose_component *)data;
	comp->type = asn1_type;
	comp->len = src_len;
	memcpy(comp->data, src, src_len);
	
	return 2 + src_len;
}

static int rose_number_digits_decode(struct pri *pri, q931_call *call, unsigned char *data, int len, struct addressingdataelements_presentednumberunscreened *value)
{
	int i = 0;
	struct rose_component *comp = NULL;
	unsigned char *vdata = data;

	do {
		GET_COMPONENT(comp, i, vdata, len);
		CHECK_COMPONENT(comp, ASN1_NUMERICSTRING, "Don't know what to do with PublicPartyNumber ROSE component type 0x%x\n");
		if(comp->len > 20) {
			pri_message("!! Oversized NumberDigits component (%d)\n", comp->len);
			return -1;
		}
		memcpy(value->partyAddress, comp->data, comp->len);
		value->partyAddress[comp->len] = '\0';

		return 0;
	}
	while(0);
	
	return -1;
}

static int rose_public_party_number_decode(struct pri *pri, q931_call *call, unsigned char *data, int len, struct addressingdataelements_presentednumberunscreened *value)
{
	int i = 0;
	struct rose_component *comp = NULL;
	unsigned char *vdata = data;
	int ton;

	if (len < 2)
		return -1;

	do {
		GET_COMPONENT(comp, i, vdata, len);
		CHECK_COMPONENT(comp, ASN1_ENUMERATED, "Don't know what to do with PublicPartyNumber ROSE component type 0x%x\n");
		ASN1_GET_INTEGER(comp, ton);
		NEXT_COMPONENT(comp, i);
		ton = typeofnumber_for_q931(pri, ton);

		if(rose_number_digits_decode(pri, call, &vdata[i], len-i, value))
			return -1;
		value->ton = ton;

		return 0;

	} while(0);
	return -1;
}

static int rose_address_decode(struct pri *pri, q931_call *call, unsigned char *data, int len, struct addressingdataelements_presentednumberunscreened *value)
{
	int i = 0;
	struct rose_component *comp = NULL;
	unsigned char *vdata = data;

	do {
		GET_COMPONENT(comp, i, vdata, len);

		switch(comp->type) {
		case 0xA0:	/* unknownPartyNumber */
			if(rose_number_digits_decode(pri, call, comp->data, comp->len, value))
				return -1;
			value->npi = PRI_NPI_UNKNOWN;
			value->ton = PRI_TON_UNKNOWN;
			break;
		case 0xA1:	/* publicPartyNumber */
			if(rose_public_party_number_decode(pri, call, comp->data, comp->len, value) != 0)
				return -1;
			value->npi = PRI_NPI_E163_E164;
			break;
		case 0xA2:	/* nsapEncodedNumber */
			pri_message("!! NsapEncodedNumber isn't handled\n");
			return -1;
		case 0xA3:	/* dataPartyNumber */
			if(rose_number_digits_decode(pri, call, comp->data, comp->len, value))
				return -1;
			value->npi = PRI_NPI_X121 /* ??? */;
			value->ton = PRI_TON_UNKNOWN /* ??? */;
			pri_message("!! dataPartyNumber isn't handled\n");
			return -1;
		case 0xA4:	/* telexPartyNumber */
			if (rose_number_digits_decode(pri, call, comp->data, comp->len, value))
				return -1;
			value->npi = PRI_NPI_F69 /* ??? */;
			value->ton = PRI_TON_UNKNOWN /* ??? */;
			pri_message("!! telexPartyNumber isn't handled\n");
			return -1;
		case 0xA5:	/* priavePartyNumber */
			pri_message("!! privatePartyNumber isn't handled\n");
			value->npi = PRI_NPI_PRIVATE;
			return -1;
		case 0xA8:	/* nationalStandardPartyNumber */
			if (rose_number_digits_decode(pri, call, comp->data, comp->len, value))
				return -1;
			value->npi = PRI_NPI_NATIONAL;
			value->ton = PRI_TON_NATIONAL;
			break;
		default:
			pri_message("!! Unknown Party number component received 0x%X\n", comp->type);
			return -1;
		}
		NEXT_COMPONENT(comp, i);
		if(i < len)
			pri_message("!! not all information is handled from Address component\n");
		return 0;
	}
	while (0);

	return -1;
}

static int rose_presented_number_unscreened_decode(struct pri *pri, q931_call *call, unsigned char *data, int len, struct addressingdataelements_presentednumberunscreened *value)
{
	int i = 0;
	struct rose_component *comp = NULL;
	unsigned char *vdata = data;

	/* Fill in default values */
	value->ton = PRI_TON_UNKNOWN;
	value->npi = PRI_NPI_E163_E164;
	value->pres = -1;	/* Data is not available */

	do {
		GET_COMPONENT(comp, i, vdata, len);

		switch(comp->type) {
		case 0xA0:		/* [0] presentationAllowedNumber */
			value->pres = PRES_ALLOWED_USER_NUMBER_NOT_SCREENED;
			return rose_address_decode(pri, call, comp->data, comp->len, value);
		case 0x81:		/* [1] IMPLICIT presentationRestricted */
			if (comp->len != 0) { /* must be NULL */
				pri_error("!! Invalid PresentationRestricted component received (len != 0)\n");
				return -1;
			}
			value->pres = PRES_PROHIB_USER_NUMBER_NOT_SCREENED;
			return 0;
		case 0x82:		/* [2] IMPLICIT numberNotAvailableDueToInterworking */
			if (comp->len != 0) { /* must be NULL */
				pri_error("!! Invalid NumberNotAvailableDueToInterworking component received (len != 0)\n");
				return -1;
			}
			value->pres = PRES_NUMBER_NOT_AVAILABLE;
			return 0;
		case 0xA3:		/* [3] presentationRestrictedNumber */
			value->pres = PRES_PROHIB_USER_NUMBER_NOT_SCREENED;
			return rose_address_decode(pri, call, comp->data, comp->len, value);
		default:
			pri_message("Invalid PresentedNumberUnscreened component 0x%X\n", comp->type);
		}
		return -1;
	}
	while (0);

	return -1;
}

static int rose_diverting_leg_information2_decode(struct pri *pri, q931_call *call, unsigned char *data, int len)
{
	int i = 0;
	int diversion_counter;
	int diversion_reason;
	struct addressingdataelements_presentednumberunscreened divertingNr;
	struct addressingdataelements_presentednumberunscreened originalCalledNr;
	struct rose_component *comp = NULL;
	unsigned char *vdata = data;

	do {
		/* diversionCounter stuff */
		GET_COMPONENT(comp, i, vdata, len);
		CHECK_COMPONENT(comp, ASN1_INTEGER, "Don't know what to do it diversionCounter is of type 0x%x\n");
		ASN1_GET_INTEGER(comp, diversion_counter);
		NEXT_COMPONENT(comp, i);

		/* diversionReason stuff */
		GET_COMPONENT(comp, i, vdata, len);
		CHECK_COMPONENT(comp, ASN1_ENUMERATED, "Invalid diversionReason type 0x%X of ROSE divertingLegInformation2 component received\n");
		ASN1_GET_INTEGER(comp, diversion_reason);
		NEXT_COMPONENT(comp, i);

		diversion_reason = redirectingreason_for_q931(pri, diversion_reason);
	
#ifdef DEBUG
		if(pri->debug)
			pri_message("    Redirection reason: %d, total diversions: %d\n", diversion_reason, diversion_counter);
#endif

		for(; i < len; NEXT_COMPONENT(comp, i)) {
			GET_COMPONENT(comp, i, vdata, len);
			switch(comp->type) {
			case 0xA1:		/* divertingNr: PresentedNumberUnscreened */
				if(rose_presented_number_unscreened_decode(pri, call, comp->data, comp->len, &divertingNr) != 0)
					return -1;
#ifdef DEBUG
				if (pri->debug) {
					pri_message("    Received divertingNr '%s'\n", divertingNr.partyAddress);
					pri_message("      ton = %d, pres = %d, npi = %d\n", divertingNr.ton, divertingNr.pres, divertingNr.npi);
				}
#endif
				break;
			case 0xA2:		/* originalCalledNr: PresentedNumberUnscreened */
				if(rose_presented_number_unscreened_decode(pri, call, comp->data, comp->len, &originalCalledNr) != 0)
					return -1;
#ifdef DEBUG
				if (pri->debug) {
					pri_message("    Received originalCalledNr '%s'\n", originalCalledNr.partyAddress);
					pri_message("      ton = %d, pres = %d, npi = %d\n", originalCalledNr.ton, originalCalledNr.pres, originalCalledNr.npi);
				}
#endif
				break;
			default:
				pri_message("!! Invalid DivertingLegInformation2 component received 0x%X\n", comp->type);
				return -1;
			}
		}
		if (i < len)
			return -1;	/* Aborted before */

		if (divertingNr.pres >= 0) {
			call->redirectingplan = divertingNr.npi;
			call->redirectingpres = divertingNr.pres;
			call->redirectingreason = diversion_reason;
			strncpy(call->redirectingnum, divertingNr.partyAddress, sizeof(call->redirectingnum)-1);
			call->redirectingnum[sizeof(call->redirectingnum)-1] = '\0';
		}
		else if (originalCalledNr.pres >= 0) {
			call->redirectingplan = originalCalledNr.npi;
			call->redirectingpres = originalCalledNr.pres;
			call->redirectingreason = diversion_reason;
			strncpy(call->redirectingnum, originalCalledNr.partyAddress, sizeof(call->redirectingnum)-1);
			call->redirectingnum[sizeof(call->redirectingnum)-1] = '\0';
		}
		return 0;
	}
	while (0);

	return -1;
}

int rose_invoke_decode(struct pri *pri, q931_call *call, unsigned char *data, int len)
{
	int i = 0;
	int operation_tag;
	unsigned char *vdata = data;
	struct rose_component *comp = NULL, *invokeid = NULL, *operationid = NULL;

	do {
		/* Invoke ID stuff */
		GET_COMPONENT(comp, i, vdata, len);
		CHECK_COMPONENT(comp, ASN1_INTEGER, "Don't know what to do if first ROSE component is of type 0x%x\n");
		invokeid = comp;
		NEXT_COMPONENT(comp, i);

		/* Operation Tag */
		GET_COMPONENT(comp, i, vdata, len);
		CHECK_COMPONENT(comp, ASN1_INTEGER, "Don't know what to do if second ROSE component is of type 0x%x\n");
		operationid = comp;
		ASN1_GET_INTEGER(comp, operation_tag);
		NEXT_COMPONENT(comp, i);

		/* No argument - return with error */
		if (i >= len) 
			return -1;

		/* Arguement Tag */
		GET_COMPONENT(comp, i, vdata, len);
		if (!comp->type)
			return -1;

#ifdef DEBUG
		pri_message("  [ Handling operation %d ]\n", operation_tag);
#endif
		switch (operation_tag) {
		case SS_CNID_CALLINGNAME:
#ifdef DEBUG
			if (pri->debug)
				pri_message("  Handle Name display operation\n");
#endif
			switch (comp->type) {
			case ROSE_NAME_PRESENTATION_ALLOWED_SIMPLE:
				memcpy(call->callername, comp->data, comp->len);
				call->callername[comp->len] = 0;
#ifdef DEBUG
				if (pri->debug)
				  pri_message("    Received caller name '%s'\n", call->callername);
#endif
				return 0;
			default:
				pri_message("Do not handle argument of type 0x%X\n", comp->type);
				return -1;
			}
			break;
		case ROSE_DIVERTING_LEG_INFORMATION2:
#ifdef DEBUG
			if (pri->debug)
				pri_message("  Handle DivertingLegInformation2\n");
#endif
			if (comp->type != 0x30) { /* Sequence */
				pri_message("Invalid DivertingLegInformation2Type argument\n");
				return -1;
			}
			return rose_diverting_leg_information2_decode(pri, call, comp->data, comp->len);
		default:
			pri_message("!! Unable to handle ROSE operation %d\n", operation_tag);
			return -1;
		}
	} while(0);
	
	return -1;
}

