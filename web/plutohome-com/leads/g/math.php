<?
class Math
{
	/**
	 * @return float
	 * @param celsius float
	 * @desc Get Fahrenheit from Celsius
	 */
	function Celsius_2_Fahrenheit($celsius)
	{
		return (9/5)*$celsius+32;
	}
	/**
	 * @return float
	 * @param fahrenheit float
	 * @desc Get Celsius from Fahrenheit
	 */
	function Fahrenheit_2_Celsius($fahrenheit)
	{
		return (5/9)*($fahrenheit-32);
	}
}