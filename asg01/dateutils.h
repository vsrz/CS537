#include <ctime>
#include <string>
#include <iostream>


namespace dateutils
{

string getDateTime()
{
	// current date/time based on current system
	time_t now = time(0);
	stringstream ss;
	string str;	
	string s("");
	tm *tm = gmtime(&now);
	
	/* Display format for months and days of the week */
	const char * weekday[] = { "Sun", "Mon", "Tue", "Wed", 
		"Thu", "Fri", "Sat" };
	const char * month [] = { "Jan", "Feb", "Mar", "Apr", "May",
		"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		

	/* Weekday array format from above */
	ss << weekday[tm->tm_wday] << ", "  << tm->tm_mday << " ";
	
	/* Month from array, and current year + 1900 */
	ss << month[tm->tm_mon] << " " << (1900 + tm->tm_year) << " ";
	
	/* hr, min, and sec with leading zero check */
	ss << (tm->tm_hour < 10 ? "0" : "") << tm->tm_hour << ":";
	ss << (tm->tm_min < 10 ? "0" : "") << tm->tm_hour << ":";
	ss << (tm->tm_sec < 10 ? "0" : "") << tm->tm_sec;
	ss << " GMT";
	s += ss.str();
	return s;
}

/* Returns current date header specified in RFC 2822 */
void getDateTimeRfc2822(string &s)
{
	stringstream ss;

	/* http header */
	ss << "Date: ";
	ss << getDateTime();
	s += ss.str() + "\r\n";
	
}

void getLastModifiedRfc2822(string &s)
{
	stringstream ss;
	ss << getDateTime();
	s+= "Last-Modified: " + ss.str() + "\r\n";
}

}
