#ifndef __RATPOISONHANDLER_H__
#define __RATPOISONHANDLER_H__

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "Logger.h"

#include <string>
using std::string;

template<class T>
class RatpoisonHandler
{
    typedef enum
	{
		RP_COMMAND = 0,
		RP_COMMAND_REQUEST = 1,
		RP_COMMAND_RESULT = 2
	} ATOMS;

	static char *atom_names[3];


	private:
        Atom atoms[3];
		bool m_bIsRatpoisonAvailable;

    public:

        bool makeActive(string windowName)
        {
            return commandRatPoison(string(":select ") + windowName);
        }

    protected:
		bool grabMousePointer()
		{
			Display *display = static_cast<T>(this)->getDisplay();
			Windows *window = static_cast<T>(this)->getWindow();

			if (display == NULL )
				return false;

			XLockDisplay(display);
			XGrabPointer(display, window, False, PointerMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync, GrabModeAsync, DefaultRootWindow(display), None, CurrentTime);
			XUnlockDisplay(display);
		}

        bool commandRatPoison(string command)
        {
            Display *display = static_cast<T*>(this)->getDisplay();

			if ( display == NULL )
				return false;

            g_pPlutoLogger->Write(LV_STATUS, "Instructing ratpoison to do this: \"%s\"", command.c_str());


			XLockDisplay(display);
			if ( XInternAtoms(display, atom_names, 3, True, atoms) == 0 )
			{
                g_pPlutoLogger->Write(LV_WARNING, "Ratpoison window manager does not seem to be running on this server");
                XUnlockDisplay(display);
                return false;
            }

            Window commandWindow = 0;

            commandWindow = XCreateSimpleWindow (display, DefaultRootWindow (display), 0, 0, 1, 1, 0, 0, 0);

            // wait for propertyChanges here.
            XSelectInput (display, commandWindow, PropertyChangeMask);

            XChangeProperty (display, commandWindow,
                        atoms[RP_COMMAND], XA_STRING, 8, PropModeReplace,
                        (unsigned char *)command.c_str(), command.size() + 2);

            XChangeProperty (display, DefaultRootWindow (display),
                        atoms[RP_COMMAND_REQUEST], XA_WINDOW, 8, PropModeAppend,
                        (unsigned char *)&commandWindow, sizeof (Window));

            bool done = false;

            long timeSpent;
            struct timeval startTime, currentTime;

            gettimeofday(&startTime, NULL);
            while ( ! done )
            {
                XEvent ev;

                gettimeofday(&currentTime, NULL);

                timeSpent = (currentTime.tv_sec - startTime.tv_sec) * 1000000 + (currentTime.tv_usec - startTime.tv_usec);
                if ( XCheckMaskEvent (display, PropertyChangeMask, &ev) )
                {
                    if (ev.xproperty.atom == atoms[RP_COMMAND_RESULT] && ev.xproperty.state == PropertyNewValue && ev.xproperty.window == commandWindow)
                    {
                        readCommandResult(display, ev.xproperty.window);
                        done = 1;
                    }
                }
                else if ( timeSpent < 250000 ) // wait for at most 1 sec for the reply
                {
                    usleep(25000); // read the result 20 pe second
                }
                else // timeout reading command response
                {
                    g_pPlutoLogger->Write(LV_WARNING, "The ratpoison window manager didn't reply to the command. It is either crashed or not started at all");
                    done = 1;
                }
            }

            // cleanups
            XDestroyWindow (display, commandWindow);
            XUnlockDisplay(display);

            g_pPlutoLogger->Write(LV_STATUS, "Exiting rat poison command");
            return true;
        }

    protected:
        void RatpoisonHandler::readCommandResult(Display *display, Window commandWindow)
        {
            int status;
            Atom type_ret;
            int format_ret;

            unsigned long nitems;
            unsigned long bytes_after;
            unsigned char *result = NULL;

			if ( display == NULL )
				return;

            /* First, find out how big the property is. */
            status = XGetWindowProperty (display, commandWindow, atoms[RP_COMMAND_RESULT],
                            0, 0, False, XA_STRING,
                            &type_ret, &format_ret, &nitems, &bytes_after,
                            &result);

            /* Failed to retrieve property. */
            if (status != Success || result == NULL)
            {
                g_pPlutoLogger->Write(LV_WARNING, "Failed to get command result length for the ratpoison command output");
                return;
            }

            /* XGetWindowProperty always allocates one extra byte even if the property is zero length. */
            XFree (result);

            /* Now that we have the length of the message, we can get the whole message. */
            status = XGetWindowProperty (display, commandWindow,
                            atoms[RP_COMMAND_RESULT],
                            0, (bytes_after / 4) + (bytes_after % 4 ? 1 : 0),
                            True, XA_STRING,
                            &type_ret, &format_ret, &nitems,
                            &bytes_after, &result);

            /* Failed to retrieve property. */
            if (status != Success || result == NULL)
            {
                g_pPlutoLogger->Write(LV_WARNING, "Failed to get command result for the ratpoison command output");
                return;
            }

            /* If result is not the empty string, print it. */
            // if ( strlen ((char*)result) )
            g_pPlutoLogger->Write(LV_STATUS, "Command result: \"%s\"", result);

            /* Free the result. */
            XFree (result);
        }

        typedef RatpoisonHandler<T> RPHandler;
};


template<class T>
char * RatpoisonHandler<T>::atom_names[3] = {
	"RP_COMMAND",
	"RP_COMMAND_REQUEST",
	"RP_COMMAND_RESULT"
};

#endif
