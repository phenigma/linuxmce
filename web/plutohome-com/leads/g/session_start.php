<?
// This file starts the session if it has not already been started
// It loads classes that are stored in $_SESSION before starting the session

if (!isset($_SESSION))
{
	require_once('g/messages.php'); // (load class definition before session starts)
	session_start();
}