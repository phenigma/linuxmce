#!/bin/bash

sed -r '/syntax on/ s/^"//; /set background/ s/^"//; /if has\("autocmd"\)/,/^$/ s/^"//; /set showcmd/ s/^"//; /set showmatch/ s/^"//' /etc/vim/vimrc >/etc/vim/vimrc.$$
mv /etc/vim/vimrc{.$$,}
