#
# This file is called by /etc/init.d/vdr
#

mkdir -p /var/run/vdr
chown -R vdr:vdr /var/run/vdr

getplugins ()
{
    local plugin_order
    local installed_plugins
    local ordered_plugins
    local last_plugins
    local plugin
    local i
    local arguments
    local plugins
    local packages
    local leftout
    local leftout2
    local vdrcmd
    local version

    vdrcmd="/usr/bin/vdr -u $USER $OPTIONS"

    CACHE_DIR="/var/cache/vdr"
    CACHE_FILE="${CACHE_DIR}/plugin_search_cache"
    CACHE_MD5="${CACHE_FILE}.md5"

    version=`eval "$vdrcmd -V -L/usr/bin/vdr 2>/dev/null | sed 's/.*(\(.*\)).*/\1/'"`
    test "$version" || version="unknown version"

    PLUGINS=""

    echo -ne "\nSearching for plugins (VDR $version) "

    # find installed plugins
    version=`echo "$version" | sed 's:.*/::'`
    # cached plugin index up to date ?
    if [ -e ${CACHE_MD5} ] && \
       [ -e ${CACHE_FILE} ] && \
       md5sum ${PLUGIN_DIR}/${PLUGIN_PREFIX}*.so.${version} ${CACHE_FILE} 2>&1 | cmp -s - ${CACHE_MD5}
    then
        plugins=`cat ${CACHE_FILE}`
        echo -ne "(cache hit):"
    else
        echo -ne "(cache miss):"
        # clear stale cache files
        rm -f ${CACHE_FILE} ${CACHE_MD5}
        plugins=(`find ${PLUGIN_DIR} -maxdepth 1 \
                       -name "${PLUGIN_PREFIX}*.so.${version}" | \
                   xargs -r dpkg -S 2>&1 | \
                   sed "s/^dpkg:/'':/" | \
                   sed "s/:.*${PLUGIN_PREFIX}\([^\.]\+\)\.so\.${version}.*$/:\1/"`)
        # write results into cache
        echo ${plugins[@]} > ${CACHE_FILE}
        md5sum ${PLUGIN_DIR}/${PLUGIN_PREFIX}*.so.${version} ${CACHE_FILE} > ${CACHE_MD5} 2>&1
    fi
    installed_plugins=(`echo ${plugins[@]} | sed 's/[^ ]*://g'`)
    packages=(   vdr   `echo ${plugins[@]} | sed 's/:[^ ]*//g'`)

    if [ "$PLUGIN_CHECK_STARTABLE" = "yes" ]; then

        # move not startable plugins to $leftout2
        for (( i=${#installed_plugins[@]}, i-- ; i >= 0 ; i-- )); do
            if ! eval "$vdrcmd -V -L $PLUGIN_DIR -P ${installed_plugins[$i]}" \
                2>/dev/null | grep -q "^${installed_plugins[$i]} "; then
                leftout2="${leftout2} ${installed_plugins[$i]}"
                unset installed_plugins[$i]
            fi
        done

        # cleanup the installed_plugins array
        installed_plugins=( "${installed_plugins[@]}" )
    fi

    if [ -r "$PLUGIN_CFG_DIR/order.conf" ]; then
        # load plugin order
        plugin_order=( `cat $PLUGIN_CFG_DIR/order.conf | sed "s/#.*$//"` )

        # move ordered plugins to list of ordered plugins
        for plugin in ${plugin_order[@]}; do
            for (( i=0 ; i<${#installed_plugins[@]} ; i++ )); do
                if [ "$plugin" = "-${installed_plugins[$i]}" -o "$plugin" = "-*${installed_plugins[$i]}" ]; then
                    unset installed_plugins[$i]
                    installed_plugins=( "${installed_plugins[@]}" )
                    break
                fi
                if [ "$plugin" = "*${installed_plugins[$i]}" ]; then
                    last_plugins=( "${last_plugins[@]}" "${installed_plugins[$i]}" ) 
                    unset installed_plugins[$i]
                    installed_plugins=( "${installed_plugins[@]}" )
                    break
                fi
                if [ "$plugin" = "${installed_plugins[$i]}" ]; then
                    ordered_plugins=( "${ordered_plugins[@]}" "$plugin" )
                    unset installed_plugins[$i]
                    installed_plugins=( "${installed_plugins[@]}" )
                    break
                fi
            done
        done
    fi

    # append unordered and last plugins to ordered plugins
    ordered_plugins=( "${ordered_plugins[@]}" "${installed_plugins[@]}" "${last_plugins[@]}" )

    # add the command line arguments for each plugin
    for plugin in ${ordered_plugins[@]}; do
        echo -n " $plugin"
        if [ -r "$PLUGIN_CFG_DIR/plugin.$plugin.conf" ] ; then
            arguments=( `cat $PLUGIN_CFG_DIR/plugin.$plugin.conf | sed "s/#.*$//"` )
            PLUGINS="$PLUGINS -P \"$plugin ${arguments[*]}\""
        else
            PLUGINS="$PLUGINS -P $plugin"
        fi
    done

    # warn about incompatible plugins
    if [ ! -z "$leftout" ]; then
        echo -ne "\nWARNING: The following plugins have been left out due to"\
             "possible binary incompatibility: "
        echo -n $leftout
    fi

    # warn about not startable plugins
    if [ ! -z "$leftout2" ]; then
        echo -ne "\nWARNING: The following plugins have been left out due to"\
             "really binary incompatibility: "
        echo -n $leftout2
    fi
}
