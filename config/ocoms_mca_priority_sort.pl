#!/usr/bin/env perl
#
# Copyright (c) 2010      Sandia National Laboratories. All rights reserved.
#
# Copyright (c) 2011-2013 UT-Battelle, LLC. All rights reserved.
# Copyright (C) 2013      Mellanox Technologies Ltd. All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

my $components;
my @result;

while (@ARGV) {
    my $component;
    $component->{"name"} = shift(@ARGV);
    $component->{"value"} = shift(@ARGV);
    push(@{$components}, $component);
}

foreach my $component (sort { $b->{value} <=> $a->{value} } @{$components}) {
    push(@result, $component->{name});
}
sub commify_series {
    (@_ == 0) ? ''                                      :
    (@_ == 1) ? $_[0]                                   :
                join(", ", @_[0 .. ($#_-1)], "$_[-1]");
}

print commify_series(@result);
