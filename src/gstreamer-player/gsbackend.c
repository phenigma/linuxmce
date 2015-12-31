#include "gsbackend.h"
#include "globals.h"

struct Media *media;

static gboolean handler_message (GstBus *bus, GstMessage *message , gpointer data)
{
  switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR:
      {
         GError *err;
         gchar *debug;
         gst_message_parse_error(message,&err,&debug);
         g_print("\n Message:%s\n",err->message);
         g_error_free(err);
         g_free(debug);
         gst_message_unref (message);
         gst_object_unref (bus);
       }    
       break;

     case GST_MESSAGE_EOS:
       g_message("Got end of stream message..");
       gst_element_set_state (media->pipeline, GST_STATE_NULL);
       break;

     default:
      /* Unhandled message */
       break;
   }
   return TRUE;
}

void backend_pause()
{
  gst_element_set_state(media->pipeline, GST_STATE_PAUSED);  
  g_message("Paused....");

}

void backend_play()
{
  gst_element_set_state(media->pipeline, GST_STATE_PLAYING);  
  g_message("Playing....");
}

void backend_seek()
{
}

GstBusSyncReply CreateWindow (GstBus *bus,GstMessage *message,gpointer data)
{

  if (GST_MESSAGE_TYPE (message) != GST_MESSAGE_ELEMENT)
    return GST_BUS_PASS;
  if ( !gst_structure_has_name (message->structure, "prepare-xwindow-id")) 
    return GST_BUS_PASS;
  
  if (Window_Xid != 0) { 
   GstXOverlay *xoverlay;
   xoverlay = GST_X_OVERLAY (GST_MESSAGE_SRC (message));
   gst_x_overlay_set_xwindow_id (xoverlay, Window_Xid);
  } else {
     g_warning ("Should have obtained Window_Xid by now!");
  }

  gst_message_unref (message);
  return GST_BUS_DROP;   
}


gboolean backend_start (gchar *uri)
{
  media->uri = uri;
  media->pipeline = gst_element_factory_make ("playbin2", "playbin2");
  if (!media->pipeline)
  {
    g_message("Failed to create the pipeline element,playbin2...!");
    return FALSE;
  }

  g_object_set(G_OBJECT(media->pipeline),"uri",media->uri,NULL);

  media->bus = gst_pipeline_get_bus(GST_PIPELINE(media->pipeline));
  gst_bus_set_sync_handler (media->bus, (GstBusSyncHandler)CreateWindow, NULL);
  gst_bus_add_watch(media->bus, handler_message, NULL);
  gst_object_unref (media->bus);

  backend_play();
  return TRUE;
}

gboolean backend_init (int *argc,char **argv[])
{
  gst_init (argc, argv);

  media = (struct Media*)malloc(sizeof(Media));
  if (media != NULL)
    return TRUE;
  else 
    return FALSE;	
}

void backend_stop ()
{

  gst_element_set_state(media->pipeline, GST_STATE_NULL);
  gst_object_unref(GST_OBJECT (media->pipeline));
  gst_deinit();

}
