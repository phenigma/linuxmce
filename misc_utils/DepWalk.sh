#!/bin/bash

WorkDir=./files

RemoteSrc=()
LocalSrc=()
Packages=()
FinalList=()

Debug=Debug

Usage()
{
	echo "Syntax:"
	echo "  DepWalk [{--remote-src|--local-src} 'uri distribution components...' ]... --pkg package_name [--pkg package_name]..."
}

Debug()
{
	if [[ -n "$Debug" ]]; then
		echo "$@"
	fi
}

ParseSrc()
{
	local PackagesFile="$1" Src="$2"
	local line pkg_name pkg_var_name pkg

	pkg_name=
	pkg_var_name=
	while read line; do
		case "$line" in
			"Package: "*)
				pkg_name="${line#Package: }"
				pkg_var_name="${pkg_name//[-+.]/_}"
				Debug "ParseSrc: package=$pkg_name"
				eval "Pkg_${pkg_var_name}_Src=\"$Src\""
				eval "Pkg_${pkg_var_name}=\"${pkg_name}\""
			;;
			#"Essential: "*)
			#	eval "Pkg_${name}_Essential=essential"
			#;;
			"Depends: "*|"Pre-Depends: "*)
				line="${line#Depends: }"
				line="${line#Pre-Depends: }"
				line="${line// /}"
				line="${line//[|,]/ }"
				for pkg in $line; do
					pkg="${pkg%%(*}" # strip version restriction info (if it exists)
					eval "Pkg_${pkg_var_name}_Depends=(\"\${Pkg_${pkg_var_name}_Depends[@]}\" \"\$pkg\")"
				done
			;;
			"Filename: "*)
				line="${line#Filename: }"
				eval "Pkg_${pkg_var_name}_Filename=\"$line\""
			;;
		esac
	done < <(zcat "$PackagesFile")
}

LoadSrc()
{
	local SrcType="$1" Src="$2"
	local uri distribution components

	read uri distribution components < <(echo "$Src")

	Debug "LoadSrc: uri=$uri dist=$distribution comp=$components"
	local component
	case "$SrcType" in
		remote)
			for component in $components; do
				wget -O "$WorkDir"/Packages.gz "$uri"/dists/"$distribution"/"$component"/binary-i386/Packages.gz
				ParseSrc "$WorkDir"/Packages.gz "$uri $distribution $component"
				rm -f Packages.gz
			done
		;;
		local)
			if [[ "$distribution" == ./ ]]; then
				ParseSrc "$uri/Packages.gz" "$uri $distribution"
			else
				for component in $components; do
					ParseSrc "$uri"/dists/"$distribution"/"$component"/binary-i386/Packages.gz
				done
			fi
		;;
		*) return 1 ;;
	esac
}

AddToFinalList()
{
	local Package="$1"

	if [[ " ${FinalList[*]} " == *" $Package "* ]]; then
		return 0
	fi

	FinalList=("${FinalList[@]}" "$Package")
}

ComputeDependencies()
{
	local Package="$1"
	local Var
	local PkgDep

	if [[ " ${FinalList[*]} " == *" $Package "* ]]; then
		return 0
	fi
	
	local pkg_var_name="${Package//[-+.]/_}"

	Var="Pkg_${Package}"
	if [[ -z "${!Var}" ]]; then
		return 1
	fi

	Var="Pkg_${Package}_Depends[@]"
	AddToFinalList "$Package"
	for PkgDep in "${!Var}"; do
		ComputeDependencies "$PkgDep"
	done
}

PrepareEnvironment()
{
	rm -rf "$WorkDir"
	mkdir -p "$WorkDir"
}

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--remote-src) ((i++)); RemoteSrc=("${RemoteSrc[@]}" "${!i}") ;;
		--local-src) ((i++)); LocalSrc=("${LocalSrc[@]}" "${!i}") ;;
		--pkg) ((i++)); Packages=("${Packages[@]}" "${!i}") ;;
		*) echo "Unknown parameter '${!i}'"; Usage; exit 1 ;;
	esac
done

PrepareEnvironment
for src in "${LocalSrc[@]}"; do
	LoadSrc "local" "$src"
done

for src in "${RemoteSrc[@]}"; do
	LoadSrc "remote" "$src"
done

echo "Computing dependencies"
for pkg in "${Packages[@]}"; do
	ComputeDependencies "$pkg"
done

echo "${FinalList[@]}"
