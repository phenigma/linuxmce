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
    private:
        Atom rp_command;
        Atom rp_command_request;
        Atom rp_command_result;

    public:

        bool makeActive(string windowName)
        {
            return commandRatPoison(string(":select ") + windowName);
        }

    protected:
        bool commandRatPoison(string command)
        {
            Display *display = static_cast<T*>(this)->getDisplay();

            g_pPlutoLogger->Write(LV_STATUS, "Instructing ratpoison to do this: \"%s\"", command.c_str());

            rp_command          = XInternAtom (display, "RP_COMMAND", True);
            rp_command_request  = XInternAtom (display, "RP_COMMAND_REQUEST", True);
            rp_command_result   = XInternAtom (display, "RP_COMMAND_RESULT", True);

            if ( rp_command == None || rp_command_request == None || rp_command_result == None )
            {
                g_pPlutoLogger->Write(LV_WARNING, "Ratpoison window manager does not seem to be running on this server!");
                return false;
            }

            Window commandWindow = 0;

            commandWindow = XCreateSimpleWindow (display, DefaultRootWindow (display), 0, 0, 1, 1, 0, 0, 0);

            // wait for propertyChanges here.
            XSelectInput (display, commandWindow, PropertyChangeMask);

            XChangeProperty (display, commandWindow,
                        rp_command, XA_STRING, 8, PropModeReplace,
                        (unsigned char *)command.c_str(), command.size() + 2);

            XChangeProperty (display, DefaultRootWindow (display),
                        rp_command_request, XA_WINDOW, 8, PropModeAppend,
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
                    if (ev.xproperty.atom == rp_command_result && ev.xproperty.state == PropertyNewValue && ev.xproperty.window == commandWindow)
                    {
                        g_pPlutoLogger->Write(LV_STATUS, "Reading command result.");
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

            /* First, find out how big the property is. */
            status = XGetWindowProperty (display, commandWindow, rp_command_result,
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
                            rp_command_result,
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

#endif
