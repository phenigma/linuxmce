#!/bin/bash


mkdir -p ~root/.gimv/

echo "Making the ~/root/.gimv/keyconf file";
cat > ~root/.gimv/keyconf  <<EOF
[Common]
common_auto_completion1=
common_auto_completion2=
common_popup_menu=

[Image Window]
imgwin_zoomin=
imgwin_zoomout=
imgwin_fit_img=
imgwin_zoom10=
imgwin_zoom25=
imgwin_zoom50=
imgwin_zoom75=
imgwin_zoom100=
imgwin_zoom125=
imgwin_zoom150=
imgwin_zoom175=
imgwin_zoom200=

[Thumbnail Window]
thumbwin_disp_mode[0]=
thumbwin_disp_mode[1]=
thumbwin_disp_mode[2]=
thumbwin_disp_mode[3]=
thumbwin_disp_mode[4]=
thumbwin_disp_mode[5]=
thumbwin_disp_mode[6]=
thumbwin_disp_mode[7]=
thumbwin_disp_mode[8]=
thumbwin_disp_mode[9]=
thumbwin_disp_mode[10]=
thumbwin_disp_mode[11]=
thumbwin_disp_mode[12]=
thumbwin_disp_mode[13]=
thumbwin_disp_mode[14]=
thumbwin_disp_mode[15]=
EOF

echo "Making the ~root/.gimv/keyaccelrc file"
cat > ~root/.gimv/keyaccelrc <<EOF
; gimv GtkAccelMap rc-file         -*- scheme -*-
; this file is an automated accelerator map dump
;
(gtk_accel_path "<ImageWinMoveSubMenu>/Last" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/10%(1)" "")
; (gtk_accel_path "<ImageWinZoomSubMenu>/---" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Fit to Image" "")
(gtk_accel_path "<ImageWinMoveSubMenu>/Previous" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/100%(5)" "")
(gtk_accel_path "<ImageWinMoveSubMenu>/First" "")
; (gtk_accel_path "<ImageWinRotateSubMenu>/tear" "")
(gtk_accel_path "<ImageWinMainMenu>/File/Open Image Window" "")
(gtk_accel_path "<ImageWinRotateSubMenu>/Rotate 90 degrees CCW" "")
(gtk_accel_path "<ImageWinRotateSubMenu>/Rotate 180 degrees" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/Fit Height" "")
; (gtk_accel_path "<ImageWinMainMenu>/Edit/---" "")
; (gtk_accel_path "<ImageWinViewSubMenu>/Player/Hide" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/25%(2)" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Reverse" "")
; (gtk_accel_path "<ImageWinMainMenu>/Edit/Scan EXIF Data..." "")
(gtk_accel_path "<ImageWinZoomSubMenu>/150%(7)" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/Keep aspect ratio" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/Zoom Out" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Slide Show Player" "")
; (gtk_accel_path "<HelpSubMenu>/About" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Maximize" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/75%(4)" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Eject" "")
; (gtk_accel_path "<ImageWinViewSubMenu>/Ignore Alpha Channel" "")
; (gtk_accel_path "<ImageWinMainMenu>/File/---" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/125%(6)" "")
(gtk_accel_path "<ImageWinRotateSubMenu>/Rotate 90 degrees CW" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/---" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Play" "")
; (gtk_accel_path "<ImageWinZoomSubMenu>/tear" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/Fit to Window" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Forward" "")
; (gtk_accel_path "<ImageWinViewSubMenu>/---" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/175%(8)" "")
(gtk_accel_path "<ImageWinMainMenu>/File/Memory Buffer" "")
(gtk_accel_path "<ImageWinMoveSubMenu>/Next" "")
(gtk_accel_path "<ImageWinMainMenu>/File/Open Thumbnail Window" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/tear" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/Fit Width" "")
(gtk_accel_path "<ImageWinMainMenu>/File/Open..." "")
; (gtk_accel_path "<HelpSubMenu>/Manual" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Continuance" "")
; (gtk_accel_path "<ImageWinViewSubMenu>/tear" "")
; (gtk_accel_path "<ImageWinViewSubMenu>/Player/Show" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Stop" "")
; (gtk_accel_path "<ImageWinMainMenu>/Edit/Edit Comment..." "")
(gtk_accel_path "<ImageWinViewSubMenu>/Menu Bar" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Status Bar" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/200%(9)" "")
(gtk_accel_path "<ImageWinMainMenu>/File/Quit" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Scroll Bar" "")
(gtk_accel_path "<ImageWinMainMenu>/Edit/Create Thumbnail" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/Zoom In" "")
(gtk_accel_path "<ImageWinMainMenu>/File/Close" "")
; (gtk_accel_path "<ImageWinMovieSubMenu>/Pause" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Tool Bar" "")
(gtk_accel_path "<ImageWinViewSubMenu>/Full Screen" "")
(gtk_accel_path "<ImageWinMainMenu>/Edit/Options..." "")
; (gtk_accel_path "<ImageWinViewSubMenu>/Player/Auto" "")
(gtk_accel_path "<ImageWinZoomSubMenu>/50%(3)" "")
EOF

echo "Making the ~root/.gimv/gimvrc file"
cat > ~root/.gimv/gimvrc <<EOF
[Common]
default_file_open_window=0
default_dir_open_window=0
default_arc_open_window=0
scan_dir_recursive=FALSE
recursive_follow_link=FALSE
recursive_follow_parent=FALSE
read_dotfile=FALSE
detect_filetype_by_ext=TRUE
disp_filename_stdout=TRUE
interpolation=1
fast_scale_down=FALSE
conv_rel_path_to_abs=TRUE
iconset=default
textentry_font=-alias-fixed-medium-r-normal--14-*-*-*-*-*-*-*
charset_locale=default
charset_internal=default
charset_auto_detect_lang=0
charset_filename_mode=1
charset_filename=default
imgtype_disables=
imgtype_user_defs=

[Startup]
startup_read_dir=FALSE
startup_open_thumbwin=FALSE
startup_no_warning=TRUE

[Image View]
imgwin_save_win_state=TRUE
imgwin_width=1102
imgwin_height=919
imgwin_fit_to_image=FALSE
imgwin_open_new_win=FALSE
imgwin_raise_window=TRUE
imgwin_show_menubar=FALSE
imgwin_show_toolbar=FALSE
imgwin_show_player=FALSE
imgwin_show_statusbar=FALSE
imgwin_toolbar_style=0
imgwin_set_bg=FALSE
imgwin_bg_red=0
imgwin_bg_green=0
imgwin_bg_blue=0
imgwin_fullscreen_set_bg=FALSE
imgwin_fullscreen_bg_red=0
imgwin_fullscreen_bg_green=0
imgwin_fullscreen_bg_blue=0
imgview_default_zoom=3
imgview_default_rotation=0
imgview_keep_aspect=TRUE
imgview_scale=100.000000
imgview_buffer=FALSE
imgview_scrollbar=FALSE
imgview_player_visible=0
imgview_scroll_nolimit=FALSE
imgview_movie_continuance=FALSE
movie_default_view_mode=
imgview_mouse_button=0,0,0,0; -1,-2,0,0; -2,0,0,0; 3,0,0,0; 2,0,0,0; 1,0,0,0

[Thumbnail View]
thumbwin_save_win_state=TRUE
thumbwin_width=626
thumbwin_height=507
thumbwin_layout_type=0
thumbwin_pane1_horizontal=TRUE
thumbwin_pane2_horizontal=TRUE
thumbwin_pane2_attach_1=FALSE
thumbwin_widget[0]=1
thumbwin_widget[1]=2
thumbwin_widget[2]=3
thumbwin_pane_size1=200
thumbwin_pane_size2=200
thumbwin_show_dir_view=TRUE
thumbwin_show_preview=FALSE
thumbwin_show_menubar=FALSE
thumbwin_show_toolbar=FALSE
thumbwin_show_statusbar=FALSE
thumbwin_show_tab=FALSE
thumbwin_show_preview_tab=FALSE
thumbwin_raise_window=FALSE
thumbwin_toolbar_style=0
thumbwin_disp_mode=Album
thumbwin_tab_pos=2
thumbwin_move_to_newtab=TRUE
thumbwin_show_tab_close=TRUE
thumbwin_tab_fullpath=TRUE
thumbwin_force_open_tab=FALSE
thumbwin_thumb_size=96
thumbwin_redraw_interval=1
thumbwin_show_progress_detail=TRUE
thumbwin_sort_type=0
thumbwin_sort_reverse=0
thumbwin_sort_ignore_case=0
thumbwin_sort_ignore_dir=0
thumbview_show_dir=FALSE
thumbview_show_archive=TRUE
thumbview_mouse_button=3,0,0,0; -2,0,0,0; -3,0,0,0; 1,0,0,0; 0,0,0,0; 0,0,0,0

[Thumbnail Album]
thumbalbum_row_space=8
thumbalbum_col_space=8

[Directory View]
dirview_show_toolbar=TRUE
dirview_show_dotfile=FALSE
dirview_show_current_dir=FALSE
dirview_show_parent_dir=FALSE
dirview_line_style=2
dirview_expander_style=1
dirview_auto_scroll=TRUE
dirview_auto_scroll_time=50
dirview_auto_expand=FALSE
dirview_auto_expand_time=500
dirview_mouse_button=1,0,0,0; 0,0,0,0; 1,0,0,0; 3,0,0,0; 0,0,0,0; 0,0,0,0

[Preview]
preview_zoom=3
preview_rotation=0
preview_keep_aspect=TRUE
preview_scale=100.000000
preview_scrollbar=TRUE
preview_player_visible=2
preview_buffer=FALSE
preview_mouse_button=0,0,0,0; -1,-2,0,0; -4,0,0,0; 5,0,0,0; 2,0,0,0; 1,0,0,0

[Cache]
cache_read_list=GImageView,Nautilus,Nautilus-2.0,Konqueror (Large),Konqueror (Medium),Konqueror (Small),GQview,Electric Eyes (Preview),Electric Eyes (Icon),XV thumbnail
cache_write_type=GImageView

[Comment]
comment_key_list=
comment_charset_read_mode=3
comment_charset_write_mode=3
comment_charset=default

[Slide Show]
slideshow_zoom=3
slideshow_rotation=0
slideshow_img_scale=100.000000
slideshow_screen_mode=0
slideshow_menubar=FALSE
slideshow_toolbar=FALSE
slideshow_player=FALSE
slideshow_statusbar=FALSE
slideshow_scrollbar=FALSE
slideshow_keep_aspcet=TRUE
slideshow_interval=5.000000
slideshow_repeat=FALSE
slideshow_set_bg=FALSE
slideshow_bg_red=0
slideshow_bg_green=0
slideshow_bg_blue=0

[Search]
search_similar_accuracy=0.970000
search_similar_open_collectiony=TRUE
simwin_sel_thumbview=TRUE
simwin_sel_preview=TRUE
simwin_sel_new_win=FALSE
simwin_sel_shared_win=FALSE

[Drag and Drop]
dnd_enable_to_external=TRUE
dnd_enable_from_external=TRUE
dnd_refresh_list_always=FALSE

[Wallpaper]
wallpaper_menu=GNOME1,background-properties-capplet;GNOME2,gnome-background-properties;KDE,kcmshell background;KDE(RedHat8),kcmshell kde-background

[External Program]
progs[0]=Gimp,gimp-remote -n,FALSE
progs[1]=XPaint,xpaint,FALSE
progs[2]=ElectricEyes,ee,FALSE
progs[3]=animate,animate,FALSE
progs[4]=enfle,enfle,FALSE
progs[5]=
progs[6]=
progs[7]=
progs[8]=
progs[9]=
progs[10]=
progs[11]=
progs[12]=
progs[13]=
progs[14]=
progs[15]=
web_browser=mozilla %s
text_viewer=emacs
use_internal_text_viewer=TRUE
scripts_use_default_search_dir_list=TRUE
scripts_search_dir_list=/usr/share/gimageview/scripts,/root/.gimv/scripts,/root/.gimv/scripts
scripts_show_dialog=FALSE

[Plugin]
plugin_use_default_search_dir_list=TRUE
plugin_search_dir_list=/usr/lib/gimageview/archiver,/usr/lib/gimageview/image_loader,/usr/lib/gimageview/image_saver,/usr/lib/gimageview/io_stream,/usr/lib/gimageview/thumbnail,/usr/lib/gimageview/image_view,/usr/lib/gimageview/thumbnail_view

EOF
