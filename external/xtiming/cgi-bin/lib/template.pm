###############################################################################
# Copyright (c) 2000 Ethan Fischer <allanon@crystaltokyo.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

use strict;
use POSIX;

my $verbose = 0;

sub translate {
	my ($page, $macro) = @_;
	$page =~ s/%%([^%]*)%%/exists $$macro{$1} ? $$macro{$1} : "%%$1%%"/eg;
	return $page;
}

sub context_new {
	my $context = {
		func   => { },
		macro  => { },
		fstack => [ ],
		mstack => [ ],
	};
	context_push($context);
	context_set_func($context, 'ifdef',    \&translate_ifdef);
	context_set_func($context, 'ifndef',   \&translate_ifndef);
	context_set_func($context, 'list',     \&translate_list);
	context_set_func($context, 'hashlist', \&translate_hashlist);
	return $context;
}

sub context_push {
	my ($context) = @_;
	push(@{$$context{fstack}}, { });
	push(@{$$context{mstack}}, { });
}

# Set a context function.
# If the function has not already been set in this context, save the old 
# function definition in fstack.  If the function exists already, store 
# its value.  If it does not exist, save undef.
sub context_set_func {
	my ($context,$funcname,$func) = @_;
	my ($fstack) = ${$$context{fstack}}[@{$$context{fstack}} - 1];
	if (!exists $$fstack{$funcname}) {
		if (exists ${$$context{func}}{$funcname}) {
			$$fstack{$funcname} = ${$$context{func}}{$funcname};
			print "defining function $funcname\n" if $verbose;
		} else {
			$$fstack{$funcname} = undef;
			print "undefining function $funcname\n" if $verbose;
		}
	}
	print "setting function $funcname\n" if $verbose;
	${$$context{func}}{$funcname} = $func;
}

# Set a context macro.
# If the macro has not already been set in this context, save the old 
# macro definition in fstack.  If the macro exists already, store 
# its value.  If it does not exist, save undef.
sub context_set_macro {
	my ($context,$macroname,$macro) = @_;
	my ($mstack) = ${$$context{mstack}}[@{$$context{mstack}} - 1];
	if (!exists $$mstack{$macroname}) {
		if (exists ${$$context{macro}}{$macroname}) {
			print "defining macro $macroname\n" if $verbose;
			$$mstack{$macroname} = ${$$context{macro}}{$macroname};
		} else {
			print "undefining macro $macroname\n" if $verbose;
			$$mstack{$macroname} = undef;
		}
	}
	print "setting macro $macroname\n" if $verbose;
	${$$context{macro}}{$macroname} = $macro;
}

# Set several macros at once.
sub context_set_macros {
	my ($context,$macro) = @_;
	foreach (keys %$macro) { context_set_macro($context, $_, $$macro{$_}); }
}

sub context_get_func {
	my ($context,$funcname) = @_;
	return ${$$context{func}}{$funcname} if (exists ${$$context{func}}{$funcname});
	return undef;
}

sub context_get_macro {
	my ($context,$macroname) = @_;
	return ${$$context{macro}}{$macroname} if (exists ${$$context{macro}}{$macroname});
	return undef;
}

sub context_pop {
	my ($context) = @_;
	my ($fstack) = pop(@{$$context{fstack}});
	foreach (keys %$fstack) {
		if (defined $$fstack{$_}) {
			${$$context{func}}{$_} = $$fstack{$_};
			print "restoring function $_\n" if $verbose;
		} else {
			delete ${$$context{func}}{$_};
			print "deleting function $_\n" if $verbose;
		}
	}
	my ($mstack) = pop(@{$$context{mstack}});
	foreach (keys %$mstack) {
		if (defined $$mstack{$_}) {
			${$$context{macro}}{$_} = $$mstack{$_};
			print "restoring macro $_\n" if $verbose;
		} else {
			delete ${$$context{macro}}{$_};
			print "deleting macro $_\n" if $verbose;
		}
	}
}

sub translate_fn {
	my ($page, $context) = @_;
	my $doc = html2doc($page, $context);
	print_doc_structure($doc, 0) if $verbose;
	return doc2html_expand($doc, $context);
}

sub print_doc_structure {
	my ($doc, $depth) = @_;
	if ($$doc{tag} eq "CDATA") {
		printf("%*s%s\n", $depth * 2, "", $$doc{tag});
	} else {
		printf("%*s<%s%s>\n", $depth * 2, "", $$doc{tag}, $$doc{parm} ? $$doc{parm} : "");
		foreach my $elem (@{$$doc{child}}) {
			print_doc_structure($elem, $depth + 1);
		}
		printf("%*s</%s>\n", $depth * 2, "", $$doc{tag});
	}
}

sub html2doc {
	my ($page, $context) = @_;
	my $elem = {
		tag   => 'html',
		indx  => 0,
		child => [ ],
	};
	my $tmp_pos = parse_html(\$page, $context, $elem);
	if ($tmp_pos < length($page)) {
		my $cdata = {
			tag  => 'CDATA',
			indx => $tmp_pos,
			parm => substr($page, $tmp_pos),
		};
		print "added CDATA element: \n" if $verbose;
		print "$$cdata{parm}\n" if $verbose > 1;
		push(@{$$elem{child}}, $cdata);
	}
	return $elem;
}

sub doc2html {
	my ($doc) = @_;
	my $str = "";
	if ($$doc{tag} eq "CDATA") {
		$str .= $$doc{parm};
	} else {
		$str .= sprintf("<%s%s>", $$doc{tag}, $$doc{parm} ? $$doc{parm} : "");
		foreach my $elem (@{$$doc{child}}) {
			$str .= doc2html($elem);
		}
		$str .= "</$$doc{tag}>";
	}
}

# Expand elements into CDATA.
sub doc2html_expand {
	my ($doc, $context) = @_;
	context_push($context);
	my $str = "";
	my $func = context_get_func($context, $$doc{tag});
	if ($func) {
		my $parm = $$doc{parm};
		$$doc{parm} = translate($$doc{parm}, $$context{macro});
		$str .= $func->($doc, $context);
		$$doc{parm} = $parm;
	} elsif ($$doc{tag} eq "CDATA") {
		$str .= translate($$doc{parm}, $$context{macro});
	} else {
		$str .= sprintf("<%s%s>", $$doc{tag}, $$doc{parm} ? $$doc{parm} : "");
		foreach my $elem (@{$$doc{child}}) {
			$str .= doc2html_expand($elem, $context);
		}
		$str .= "</$$doc{tag}>";
	}
	context_pop($context);
	return $str;
}

sub translate_ifdef {
	my ($doc, $context) = @_;
	my $opt = translate_args($$doc{parm});
	return "" if (!exists $$opt{name});
	if (defined context_get_macro($context, $$opt{name})) {
		my $str = "";
		foreach my $elem (@{$$doc{child}}) {
			$str .= doc2html_expand($elem, $context);
		}
		return $str;
	}
	return "";
}

sub translate_ifndef {
	my ($doc, $context) = @_;
	my $opt = translate_args($$doc{parm});
	return "" if (!exists $$opt{name});
	if (!defined context_get_macro($context, $$opt{name})) {
		my $str = "";
		foreach my $elem (@{$$doc{child}}) {
			$str .= doc2html_expand($elem, $context);
		}
		return $str;
	}
}

################################################################################
# <fn_list name start step end scoping></fn_list>
# name     => (required) name of list variable
# start    => (optional) first element in associated list to begin with; 
#             can be an integer or a percentage
# step     => (optional) offset to next element in associated list; can 
#             be an integer or a percentage
# end      => (optional) element in associated list to end with; can be 
#             an integer or a percentage
# scoping  => (optional) makes local macros more unique by prefixing them 
#             with the list name, then the value of scoping
#
# A list like:
# (joe,john,jacob)
# ... and a template like:
# <fn_list name=phone start=0 end=67% scoping=.>%%phone.name%% </fn_list>
# ... results in:
# joe john 
# Note that "jacob" is not included, as 67% equals 2 (which is jacob's 
# row) in this case, and everything up to (but not including) 67% is 
# listed.
#

sub translate_list {
	my ($doc, $context) = @_;
	my $opt = translate_args($$doc{parm});
	return "" if (!exists $$opt{name});
	my $macro = context_get_macro($context, $$opt{name});
	if ($macro) {
		my $str = "";
		my $scope = exists $$opt{scoping} ? "$$opt{name}$$opt{scoping}" : "";
		if (!ref($macro)) {
			context_set_macro($context, "${scope}listval", $macro);
			foreach my $elem (@{$$doc{child}}) {
				$str .= doc2html_expand($elem, $context);
			}
		} elsif (ref($macro) eq "ARRAY") {
			my $len = @$macro;
			my ($start,$end,$step) = (0,$len,1);
			if (exists $$opt{start} && defined $$opt{start}) {
				if ($$opt{start} =~ /([\d]+)(%)?/) {
					$start = int(clamp($2 ? $len * $1 / 100 : $1, 0, $len));
				}
			}
			if (exists $$opt{end} && defined $$opt{end}) {
				if ($$opt{end} =~ /([\d]+)(%)?/) {
					$end = POSIX::ceil(clamp($2 ? $len * $1 / 100 : $1, $start, $len));
				}
			}
			if (exists $$opt{step} && defined $$opt{step}) {
				if ($$opt{step} =~ /([\d]+)(%)?/) {
					$step = POSIX::ceil(clamp($2 ? $len * $1 / 100 : $1, $start, $len));
				}
			}
			foreach (@$macro) {
				context_set_macro($context, "${scope}listval", $_);
				foreach my $elem (@{$$doc{child}}) {
					$str .= doc2html_expand($elem, $context);
				}
			}
		}
		return $str;
	}
	return "";
}

################################################################################
# <fn_hashlist name start step end scoping></fn_hashlist>
# name     => (required) name of hashlist variable
# start    => (optional) first element in associated list to begin with; 
#             can be an integer or a percentage
# step     => (optional) offset to next element in associated list; can 
#             be an integer or a percentage
# end      => (optional) element in associated list to end with; can be 
#             an integer or a percentage
# scoping  => (optional) makes local macros more unique by prefixing them 
#             with the list name, then the value of scoping
#
# A hashlist like:
# id  name     phone
#  2  joe      111-1111
#  7  john     222-2222
#  9  jacob    333-3333
# ... and a template like:
# <fn_hashlist name=phone start=0 end=67%>%%name%% %%phone%%<br></fn_hashlist>
# ... results in:
# joe 111-1111<br>john 222-2222<br>
# Note that "jacob" is not included, as 67% equals 2 (which is jacob's 
# row) in this case, and everything up to (but not including) 67% is 
# listed.
#

sub translate_hashlist {
	my ($doc, $context) = @_;
	my $opt = translate_args($$doc{parm});
	return "" if (!exists $$opt{name});
	my $macro = context_get_macro($context, $$opt{name});
	if ($macro && ref($macro) eq "HASH") {
		my $str = "";
		my $len = scalar(@{$$macro{(keys %$macro)[0]}});
		my ($start,$end,$step) = (0,$len,1);
		if (exists $$opt{start} && defined $$opt{start}) {
			if ($$opt{start} =~ /([\d]+)(%)?/) {
				$start = int(clamp($2 ? $len * $1 / 100 : $1, 0, $len));
			}
		}
		if (exists $$opt{end} && defined $$opt{end}) {
			if ($$opt{end} =~ /([\d]+)(%)?/) {
				$end = POSIX::ceil(clamp($2 ? $len * $1 / 100 : $1, $start, $len));
			}
		}
		if (exists $$opt{step} && defined $$opt{step}) {
			if ($$opt{step} =~ /([\d]+)(%)?/) {
				$step = POSIX::ceil(clamp($2 ? $len * $1 / 100 : $1, $start, $len));
			}
		}
		my $scope = "";
		$scope = "$$opt{name}$$opt{scoping}" if exists $$opt{scoping};
		for (my $i = $start ; $i < $end ; $i += $step) {
			context_set_macro($context, "${scope}index", $i);
			foreach (keys %$macro) {
				context_set_macro($context, "${scope}$_", ${$$macro{$_}}[$i]);
			}
			foreach my $elem (@{$$doc{child}}) {
				$str .= doc2html_expand($elem, $context);
			}
		}
		return $str;
	}
	return "";
}

################################################################################
# Convert HTML tag options from a string to a hash.
# Handles both single and double quotes, and allows whitespace.
# Given an HTML line such as "<tag key1=val1 key2=val2>", pass this 
# function the options ("key1=val1 key2=val2") and it will return a 
# reference to the hash:
# { key1 => 'val1' , key2 => 'val2' }

sub translate_args {
	my ($str) = @_;
	my %hash;

	# Normalize the string to something like: key1%eq%"val1"%ws%key2%eq%"val2"
	# Use %ws% to represent the whitespace between key/value pairs, and
	# use %eq% to represent the equals sign between each key and its value.
	# This prevents whitespace and equals signs in the value from breaking 
	# the split()'s below.

	if ($str) {
		$str =~ s/^\s*(.*?)\s*$/$1/s;
		$str =~ s/([^\s]+)\s*=\s*([^\s"'][^\s]*)/$1%eq%"$2"/sg;
		$str =~ s/([^\s]+)\s*=\s*'(.*?)'/$1%eq%"$2"/sg;
		$str =~ s/([^\s]+)\s*=\s*"(.*?)"/$1%eq%"$2"/sg;
		$str =~ s/([^\s]+%eq%".*?")\s+/$1%ws%/sg;

		foreach (split('%ws%', $str)) {
			my ($key,$val) = split('%eq%');
			$val =~ s/^"(.*)"$/$1/s;
			$val =~ s/%([a-fA-F0-9][a-fA-F0-9])/pack("C", hex($1))/seg;
			$hash{lc($key)} = $val;
		}
	}

	return \%hash;
}

# 1. start with element (need tag and index)
# 2. start scanning from index, looking for a start or end tag
#   A. add CDATA element if necessary
#   B. if start tag, create new element, goto 1
#   C. if end tag, return

sub parse_html {
	my ($page,$context,$elem) = @_;
	my $last_index = $$elem{indx};
	pos($$page) = $last_index;
	while ($$page =~ m{<(/)?fn_(\w+)(\s+(?:[^>]*?(?:"[^"]*")|(?:'[^']*'))*[^>]*?)?(/)?>}sg) {
		next if !context_get_func($context, $2);
		my $start_tag = $1 ? 0 : 1;
		my $tag = lc $2;
		my $parm = $3;
		my $empty_tag = $4;
		my $tmp_pos = pos($$page);
		if ($last_index < length($`)) {
			my $cdata = {
				tag  => 'CDATA',
				indx => $last_index,
				parm => substr($$page, $last_index, length($`) - $last_index),
			};
			print "added CDATA element: \n" if $verbose;
			print "$$cdata{parm}\n" if $verbose > 1;
			push(@{$$elem{child}}, $cdata);
		}
		if ($start_tag) {
			my $new_elem = {
				tag  => $tag,
				indx => $tmp_pos,
				parm => $parm,
			};
			print "added ".uc($tag)." element: \n" if $verbose;
			print "<$$new_elem{tag}$$new_elem{parm}>\n" if $verbose > 1;
			push(@{$$elem{child}}, $new_elem);
			if (!$empty_tag) {
				$tmp_pos += parse_html($page, $context, $new_elem);
				pos($$page) = $tmp_pos;
			}
		}
		$last_index = $tmp_pos;
		last if !$start_tag && $tag eq $$elem{tag};
	}
	return $last_index - $$elem{indx};
}

sub clamp {
	my ($val,$min,$max) = @_;
	return ($val < $min) ? $min : ($val > $max) ? $max : $val;
}

1;
