//
// C++ Implementation: AssistedTransfer
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "AssistedTransfer.h"

using namespace DCE;

AssistedTransfer::AssistedTransfer(	const string & ext,
									const string & callid,
									const string & channelid,
									const string & mycallid )
	: TelecomTask(),
	  sExtDest(ext),
	  sCallID_Dest(callid),
	  sMyChannelID(channelid),
	  sMyCallID(mycallid)
{
	//1) {X,Y,A} & B => {X,Y} & {A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 

	//struct Task
	//{
	//	ext dest,
	//	call id dest,
	//	my channel,
	//	>> my call id
	//}

	//create "private chat"
	//daca failed -> msg to orbiter si refacere stare
}

AssistedTransfer::~AssistedTransfer()
{
}

bool AssistedTransfer::ProcessJob(const string &)
{
	//job "initialize" (internal)
	//1) {X,Y,A} & B => {X,Y} & {A,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 

	//job "transfer"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> drop A, {X,Y,B}
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y} & {A,B,C} 

	//job "conference"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A,B}

	//job "merge calls"
	//2) {X,Y,A} & {B,C} => {X,Y,A,B,C} 

	//job "cancel"
	//1) {X,Y,A} & B => {X,Y} & {A,B} -> {X,Y,A} & B
	//or 
	//2) {X,Y,A} & {B,C} => {X,Y,A} & {B,C} 
	
	return true;
}

